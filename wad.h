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

#pragma once
#ifndef _WAD_H_
#define _WAD_H_
#ifdef __cplusplus
extern "C" {
#endif

enum {
	WAD_IWAD,
	WAD_PWAD
};

typedef struct wad {
	Uint32 type;
	Uint32 num_lumps;
	struct wad_lump {
		Uint32 size;
		char name[8];
		SDL_IOStream *io;
		void *data;
	} *lumps;
	void *data;
} wad_t;

/* load wad file into memory */
wad_t *wad_load(const char *filename);

/* free wad file data */
void wad_free(wad_t *wad);

/* get lump index by name */
Sint32 wad_get_index(wad_t *wad, const char *name);

/* get lump iostream by name */
SDL_IOStream *wad_get_io(wad_t *wad, const char *name);

/* get lump data by name */
void *wad_get_data(wad_t *wad, const char *name);

/* get lump size by name */
Uint32 wad_get_size(wad_t *wad, const char *name);

#ifdef __cplusplus
}
#endif
#endif /* _WAD_H_ */
