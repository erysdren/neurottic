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
#ifndef _MAPSET_H_
#define _MAPSET_H_
#ifdef __cplusplus
extern "C" {
#endif

/* load mapset by filename */
int MS_LoadMapSet(const char *filename);

/* unload current map and mapset */
void MS_UnloadMapSet(void);

/* load map from current mapset */
int MS_LoadMap(int map);

/* unload current map */
void MS_UnloadMap(void);

/* get pointer to decompressed plane from current map (valid range is 0-2) */
Uint16 *MS_GetCurrentMapPlane(int plane);

#ifdef __cplusplus
}
#endif
#endif /* _MAPSET_H_ */
