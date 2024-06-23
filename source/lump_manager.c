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

#include "neurottic.h"

/*
 *
 * types
 *
 */

static const char iwad_magic[4] = "IWAD";
static const char pwad_magic[4] = "PWAD";

typedef struct wad {
	char *filename;
	char magic[4];
	Sint32 num_lumps;
	Sint32 ofs_lumps;
	struct wad_lump {
		Sint32 ofs;
		Sint32 len;
		char name[8];
	} *lumps;
	void *data;
} wad_t;

/*
 *
 * globals
 *
 */

/* is lump manager running? */
static SDL_bool started = SDL_FALSE;

/* wad file handles */
static int num_wads = 0;
static wad_t **wads = NULL;

/* paths strings */
static int num_paths = 0;
static char **paths = NULL;

/*
 *
 * public functions
 *
 */

/* startup lump manager */
int LM_Init(void)
{
	if (started)
		return LogError("LM_Init(): Lump manager already started");

	started = SDL_TRUE;

	return 0;
}

/* shutdown lump manager */
void LM_Quit(void)
{
	if (wads)
	{
		for (int i = 0; i < num_wads; i++)
		{
			if (wads[i])
			{
				if (wads[i]->lumps)
					SDL_free(wads[i]->lumps);

				if (wads[i]->filename)
					SDL_free(wads[i]->filename);

				if (wads[i]->data)
					SDL_free(wads[i]->data);

				SDL_free(wads[i]);
			}
		}

		SDL_free(wads);
	}

	if (paths)
	{
		for (int i = 0; i < num_paths; i++)
		{
			if (paths[i])
				SDL_free(paths[i]);
		}

		SDL_free(paths);
	}

	num_wads = 0;
	wads = NULL;

	num_paths = 0;
	paths = NULL;

	started = SDL_FALSE;
}

/* add wad to lump manager */
int LM_AddWAD(const char *filename)
{
	SDL_IOStream *io = NULL;
	wad_t *wad = NULL;
	void *data = NULL;
	size_t size = 0;
	int ret = -1;

	/* stoopid */
	if (!filename)
	{
		ret = LogError("LM_AddWAD(): NULL pointer passed as filename");
		goto fail;
	}

	/* load file */
	data = SDL_LoadFile(filename, &size);
	if (!data)
		goto fail;

	/* create io */
	io = SDL_IOFromConstMem(data, size);
	if (!io)
		goto fail;

	/* allocate wad */
	wad = SDL_calloc(1, sizeof(wad_t));
	if (!wad)
	{
		ret = LogError("LM_AddWAD(): Memory allocation of %zu bytes failed", sizeof(wad_t));
		goto fail;
	}

	/* check magic */
	SDL_ReadIO(io, wad->magic, 4);
	if (SDL_memcmp(wad->magic, iwad_magic, 4) != 0 && SDL_memcmp(wad->magic, pwad_magic, 4) != 0)
	{
		ret = LogError("LM_AddWAD(): WAD magic \"%.*s\" does not match expected", 4, wad->magic);
		goto fail;
	}

	/* read data */
	SDL_ReadS32LE(io, &wad->num_lumps);
	SDL_ReadS32LE(io, &wad->ofs_lumps);

	/* allocate lumps */
	wad->lumps = SDL_calloc(wad->num_lumps, sizeof(struct wad_lump));
	if (!wad->lumps)
	{
		ret = LogError("LM_AddWAD(): Memory allocation of %zu bytes failed", wad->num_lumps * sizeof(struct wad_lump));
		goto fail;
	}

	/* read lumps */
	SDL_SeekIO(io, wad->ofs_lumps, SDL_IO_SEEK_SET);
	SDL_ReadIO(io, wad->lumps, wad->num_lumps * sizeof(struct wad_lump));

	/* assign other data */
	wad->filename = SDL_strdup(filename);
	wad->data = data;

	/* add to wads list */
	num_wads++;
	wads = SDL_realloc(wads, sizeof(wad_t *) * num_wads);
	wads[num_wads - 1] = wad;

	/* done */
	ret = Log("Successfully added WAD \"%s\"", filename);
	goto done;

fail:
	if (wad)
	{
		if (wad->lumps)
			SDL_free(wad->lumps);
		SDL_free(wad);
	}
	if (data)
		SDL_free(data);
done:
	if (io)
		SDL_CloseIO(io);
	return ret;
}

/* add filesystem path to lump manager */
int LM_AddPath(const char *path)
{
	SDL_PathInfo info;

	/* stoopid */
	if (!path)
		return LogError("LM_AddPath(): NULL pointer passed as path name");

	/* check if path exists */
	if (SDL_GetPathInfo(path, &info) != 0)
		return -1;

	/* check if its a directory */
	if (info.type != SDL_PATHTYPE_DIRECTORY)
		return LogError("LM_AddPath(): \"%s\" is not a directory", path);

	/* add to paths list */
	num_paths++;
	paths = SDL_realloc(paths, sizeof(char *) * num_paths);
	paths[num_paths - 1] = SDL_strdup(path);

	return Log("Successfully added path \"%s\"", path);
}

/* load entire lump into buffer. this data should be freed with SDL_free */
void *LM_LoadLump(const char *name, size_t *sz)
{
	int i, x;

	/* search paths */
	i = num_paths;
	while (--i >= 0)
	{
		static char path[256];
		SDL_snprintf(path, sizeof(path), "%s/%s", paths[i], name);

		void *data = NULL;
		if ((data = SDL_LoadFile(path, sz)) != NULL)
			return data;
	}

	/* search wads */
	i = num_wads;
	while (--i >= 0)
	{
		for (x = 0; x < wads[i]->num_lumps; x++)
		{
			/* null lumps can't be loaded */
			if (!wads[i]->lumps[x].ofs || !wads[i]->lumps[x].len)
				continue;

			/* simple name search */
			if (SDL_strncasecmp(name, wads[i]->lumps[x].name, 8) == 0)
			{
				void *ptr = (Uint8 *)wads[i]->data + (size_t)wads[i]->lumps[x].ofs;
				SDL_IOStream *io = SDL_IOFromConstMem(ptr, (size_t)wads[i]->lumps[x].len);
				return SDL_LoadFile_IO(io, sz, SDL_TRUE);
			}
		}
	}

	LogError("LM_LoadLump(): Lump \"%s\" was not found", name);
	return NULL;
}

/* open lump for reading with SDL_IOStream */
SDL_IOStream *LM_OpenLumpIO(const char *name)
{
	int i, x;

	/* search paths */
	i = num_paths;
	while (--i >= 0)
	{
		static char path[256];
		SDL_snprintf(path, sizeof(path), "%s/%s", paths[i], name);

		SDL_IOStream *io = NULL;
		if ((io = SDL_IOFromFile(path, "rb")) != NULL)
			return io;
	}

	/* search wads */
	i = num_wads;
	while (--i >= 0)
	{
		for (x = 0; x < wads[i]->num_lumps; x++)
		{
			/* null lumps can't be opened */
			if (!wads[i]->lumps[x].ofs || !wads[i]->lumps[x].len)
				continue;

			/* simple name search */
			if (SDL_strncasecmp(name, wads[i]->lumps[x].name, 8) == 0)
			{
				void *ptr = (Uint8 *)wads[i]->data + (size_t)wads[i]->lumps[x].ofs;
				return SDL_IOFromConstMem(ptr, (size_t)wads[i]->lumps[x].len);
			}
		}
	}

	LogError("LM_OpenLumpIO(): Lump \"%s\" was not found", name);
	return NULL;
}

/* get lump index by name */
Sint32 LM_GetLumpIndex(const char *name)
{
	int i, x;

	/* search wads */
	i = num_wads;
	while (--i >= 0)
	{
		for (x = 0; x < wads[i]->num_lumps; x++)
		{
			/* simple name search */
			if (SDL_strncasecmp(name, wads[i]->lumps[x].name, 8) == 0)
				return x;
		}
	}

	/* search paths */
	i = num_paths;
	while (--i >= 0)
	{
		static char path[256];
		SDL_snprintf(path, sizeof(path), "%s/%s", paths[i], name);

		/* exists on disk, but not in a wad */
		SDL_IOStream *io;
		if ((io = SDL_IOFromFile(path, "rb")) != NULL)
		{
			SDL_CloseIO(io);
			return LogError("LM_GetLumpIndex(): Lump \"%s\" has no valid index because it only exists on disk", name);
		}
	}

	return LogError("LM_GetLumpIndex(): Lump \"%s\" was not found", name);
}
