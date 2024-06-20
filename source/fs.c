/*
MIT License

Copyright (c) 2024 erysdren (it/she/they)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <SDL3/SDL.h>
#include "wad.h"
#include "strings.h"

#ifdef _WIN32
static const char pathsep[] = "\\";
#else
static const char pathsep[] = "/";
#endif

static int num_wads = 0;
static wad_t **wads = NULL;

static int num_paths = 0;
static char **paths = NULL;

SDL_bool fs_init(void)
{
	/* doesnt do anything yet */
	return SDL_TRUE;
}

void fs_quit(void)
{
	for (int i = 0; i < num_wads; i++)
		wad_free(wads[i]);

	if (wads)
		SDL_free(wads);

	for (int i = 0; i < num_paths; i++)
		SDL_free(paths[i]);

	if (paths)
		SDL_free(paths);

	num_wads = 0;
	wads = NULL;

	num_paths = 0;
	paths = NULL;
}

/* add wad */
SDL_bool fs_add_wad(const char *filename)
{
	wad_t *wad;

	if (!filename)
		return SDL_FALSE;

	/* load wad */
	wad = wad_load(filename);
	if (!wad)
		return SDL_FALSE;

	/* add to wads list */
	num_wads++;
	wads = SDL_realloc(wads, sizeof(wad_t *) * num_wads);
	wads[num_wads - 1] = wad;

	return SDL_TRUE;
}

/* add searchpath for standalone lumps */
SDL_bool fs_add_path(const char *path)
{
	SDL_PathInfo info;

	if (!path)
		return SDL_FALSE;

	/* check if path exists */
	if (SDL_GetPathInfo(path, &info) != 0 || info.type != SDL_PATHTYPE_DIRECTORY)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
		return SDL_FALSE;
	}

	/* add to paths list */
	num_paths++;
	paths = SDL_realloc(wads, sizeof(char *) * num_paths);
	paths[num_paths - 1] = SDL_strdup(path);

	return SDL_TRUE;
}

/* check paths to open file */
SDL_IOStream *fs_file_open(const char *filename)
{
	char *path;
	SDL_IOStream *io;
	int i;

	/* check paths in reverse order */
	i = num_paths;
	while (--i >= 0)
	{
		path = stringf("%s%s%s%s", paths[i], pathsep, filename, ".lmp");

		io = SDL_IOFromFile(path, "rb");

		SDL_free(path);

		if (io)
			return io;
	}

	/* check wads in reverse order */
	i = num_wads;
	while (--i >= 0)
	{
		io = wad_get_io(wads[i], filename);
		if (io)
			return io;
	}

	return NULL;
}
