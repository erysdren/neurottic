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

/* initialize renderer */
int R_Init(void);

/* shutdown renderer */
void R_Quit(void);

/* clear screen */
void R_Clear(Uint8 color);

/* draw scene */
void R_Draw(void);

/* draw console */
void R_DrawConsole(void);

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

#ifdef __cplusplus
}
#endif
#endif /* _RENDERER_H_ */
