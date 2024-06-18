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

static const char iwad_magic[4] = "IWAD";
static const char pwad_magic[4] = "PWAD";

wad_t *wad_load(const char *filename)
{
	SDL_IOStream *io = NULL;
	void *data = NULL;
	wad_t *wad = NULL;
	size_t len_data;
	char magic[4];
	Sint32 num_lumps, ofs_lumps;
	Uint32 type;

	/* load the entire file into memory */
	data = SDL_LoadFile(filename, &len_data);
	if (!data)
		goto fail;

	/* create io */
	io = SDL_IOFromMem(data, len_data);
	if (!io)
		goto fail;

	/* check magic */
	SDL_ReadIO(io, magic, 4);
	if (SDL_memcmp(magic, iwad_magic, 4) == 0)
		type = WAD_IWAD;
	else if (SDL_memcmp(magic, pwad_magic, 4) == 0)
		type = WAD_PWAD;
	else
		goto fail;

	/* read header */
	SDL_ReadS32LE(io, &num_lumps);
	SDL_ReadS32LE(io, &ofs_lumps);

	/* allocate wad structure */
	wad = SDL_calloc(1, sizeof(wad_t));
	if (!wad)
		goto fail;

	/* setup wad fields */
	wad->type = type;
	wad->num_lumps = (Uint32)num_lumps;
	wad->data = data;

	/* setup wad lumps */
	wad->lumps = SDL_calloc(num_lumps, sizeof(struct wad_lump));
	if (!wad)
		goto fail;

	/* setup lumps */
	SDL_SeekIO(io, ofs_lumps, SDL_IO_SEEK_SET);
	for (int i = 0; i < num_lumps; i++)
	{
		Sint32 ofs, len;
		SDL_ReadS32LE(io, &ofs);
		SDL_ReadS32LE(io, &len);
		SDL_ReadIO(io, wad->lumps[i].name, 8);
		wad->lumps[i].size = (Uint32)len;
		wad->lumps[i].data = data + ofs;
		if (len && ofs)
		{
			wad->lumps[i].data = (Uint8 *)data + ofs;
			wad->lumps[i].io = SDL_IOFromConstMem((Uint8 *)data + ofs, len);
		}
	}

	goto done;

fail:
	if (data) SDL_free(data);
	if (wad)
	{
		if (wad->lumps) SDL_free(wad->lumps);
		SDL_free(wad);
	}
done:
	if (io) SDL_CloseIO(io);
	return wad;
}

void wad_free(wad_t *wad)
{
	if (wad)
	{
		for (int i = 0; i < wad->num_lumps; i++)
			if (wad->lumps[i].io)
				SDL_CloseIO(wad->lumps[i].io);

		SDL_free(wad->data);
		SDL_free(wad->lumps);
		SDL_free(wad);
	}
}

/* get lump index by name */
Sint32 wad_get_index(wad_t *wad, const char *name)
{
	if (!wad || !name)
		return -1;

	for (int i = 0; i < wad->num_lumps; i++)
	{
		if (SDL_strncmp(name, wad->lumps[i].name, 8) == 0)
			return i;
	}

	return -1;
}

/* get lump iostream by name */
SDL_IOStream *wad_get_io(wad_t *wad, const char *name)
{
	if (!wad || !name)
		return NULL;

	for (int i = 0; i < wad->num_lumps; i++)
	{
		if (SDL_strncmp(name, wad->lumps[i].name, 8) == 0)
			return wad->lumps[i].io;
	}

	return NULL;
}

/* get lump data by name */
void *wad_get_data(wad_t *wad, const char *name)
{
	if (!wad || !name)
		return NULL;

	for (int i = 0; i < wad->num_lumps; i++)
	{
		if (SDL_strncmp(name, wad->lumps[i].name, 8) == 0)
			return wad->lumps[i].data;
	}

	return NULL;
}

/* get lump size by name */
Uint32 wad_get_size(wad_t *wad, const char *name)
{
	if (!wad || !name)
		return 0;

	for (int i = 0; i < wad->num_lumps; i++)
	{
		if (SDL_strncmp(name, wad->lumps[i].name, 8) == 0)
			return wad->lumps[i].size;
	}

	return 0;
}
