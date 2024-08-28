/*
MIT License

Copyright (c) 2024 erysdren (it/she/they)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software", to deal in
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

static char *FS_Paths[] = {
	[FS_PATH_BASE] = NULL,
	[FS_PATH_PREF] = NULL
};

int FS_Init(void)
{
	if ((FS_Paths[FS_PATH_BASE] = SDL_GetBasePath()) == NULL)
		return -1;

	if ((FS_Paths[FS_PATH_PREF] = SDL_GetPrefPath("Protostar LLC", "NEUROTTIC")) == NULL)
		return -1;

	return 0;
}

void FS_Quit(void)
{
	for (int i = 0; i < ASIZE(FS_Paths); i++)
	{
		if (FS_Paths[i] != NULL)
			SDL_free(FS_Paths[i]);
	}
}

SDL_IOStream *FS_OpenFile(const char *name, const char *mode, FS_Path path)
{
	static char filepath[1024];
	SDL_IOStream *io;

	/* checks */
	if (!name || !mode)
		return NULL;
	if (path < FS_PATH_BASE || path > FS_PATH_PREF)
		return NULL;

	/* open */
	SDL_snprintf(filepath, sizeof(filepath), "%s%s", FS_Paths[path], name);
	return SDL_IOFromFile(filepath, mode);
}
