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
#ifndef _RENDERER_H_
#define _RENDERER_H_
#ifdef __cplusplus
extern "C" {
#endif

#define RENDER_WIDTH (640)
#define RENDER_HEIGHT (480)
#define RENDER_HZ (35)

/* initialize renderer */
int R_Init(void);

/* shutdown renderer */
void R_Quit(void);

/* clear screen */
void R_Clear(Uint8 color);

/* draw scene */
int R_Draw(void);

/* draw filled rect */
void R_DrawRect(int x, int y, int w, int h, Uint8 color);

/* draw surface */
int R_DrawSurface(int x, int y, int w, int h, SDL_Surface *surface);

/* draw console */
void R_DrawConsole(Uint8 color);

/* flip to visible screen */
void R_Flip(void);

/* set palette colors */
void R_SetPalette(Uint8 *palette);

/* set render position */
void R_SetPosition(float x, float y, float z);

/* set render angles */
void R_SetAngles(float x, float y, float z);

/* draw string at x,y with color */
void R_DrawString(int x, int y, Uint8 color, const char *fmt, ...);

/* create SDL_Surface from planar pic */
SDL_Surface *R_SurfaceFromPic(int w, int h, Uint8 *pixels);

/* create SDL_Surface from planar pic */
SDL_Surface *R_SurfaceFromPicIO(SDL_IOStream *io, SDL_bool closeio);

/* find closest approximation of RGB color in indexed palette */
Uint8 R_FindColor(Uint8 r, Uint8 g, Uint8 b);

#ifdef __cplusplus
}
#endif
#endif /* _RENDERER_H_ */
