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
#ifndef _RTL_H_
#define _RTL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>

/* rtl constants */
#define RTL_NUM_MAPS (100)
#define RTL_MAP_WIDTH (128)
#define RTL_MAP_HEIGHT (128)
#define RTL_PLANE_SIZE (RTL_MAP_WIDTH * RTL_MAP_HEIGHT * 2)

/* rtl flags */
enum {
	RTL_SINGLEPLAYER = 1 << 0,
	RTL_COMMBAT = 1 << 1,
	RTL_LUDICROUS = 1 << 2,
	RTL_SHAREWARE = 1 << 3,
	RTL_RANDROTT = 1 << 4
};

/* rtl map flags */
enum {
	RTL_MAP_OPEN_PUSHWALLS = 1 << 0
};

/* rtl structure */
typedef struct rtl {
	Uint32 flags;
	struct rtl_map {
		Uint32 used;
		Uint32 crc;
		Uint32 tag;
		Uint32 flags;
		Uint32 plane_offsets[3];
		Uint32 plane_sizes[3];
		char name[24];
	} maps[RTL_NUM_MAPS];
	void *data;
} rtl_t;

#ifdef __cplusplus
}
#endif
#endif /* _RTL_H_ */
