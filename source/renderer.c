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

#define NUM_PLANES (4)

/* create SDL_Surface from planar pic */
SDL_Surface *R_SurfaceFromPic(int w, int h, Uint8 *pixels)
{
	/* NOTE: this function is incredibly stupid! and probably dangerous! */

	if (!w || !h || !pixels)
		return NULL;

	SDL_Surface *surface = SDL_CreateSurface(w * NUM_PLANES, h, SDL_PIXELFORMAT_INDEX8);
	if (!surface)
		return NULL;

	/* copy plane by plane, pixel by pixel */
	for (int p = 0; p < NUM_PLANES; p++)
	{
		for (int y = 0; y < h; y++)
		{
			Uint8 *row = &((Uint8 *)surface->pixels)[y * surface->pitch];

			for (int x = 0; x < w; x++)
			{
				row[x * NUM_PLANES + p] = pixels[x];
			}

			pixels += w;
		}
	}

	SDL_SetSurfaceColorKey(surface, SDL_TRUE, 0xFF);

	return surface;
}

/* draw console */
void R_DrawConsole(void)
{
	int num_lines;
	char *input = Console_GetInputLine();
	char **lines = Console_GetLines(&num_lines);
	int y = 0;

	/* draw lines */
	for (int i = 0; i < num_lines; i++)
	{
		if (lines[i])
		{
			R_DrawString(0, y, 16, lines[i]);
			y += 8;

			/* leave room for input line */
			if (y >= RENDER_HEIGHT - 8)
				break;
		}
	}

	/* draw input line */
	R_DrawString(0, RENDER_HEIGHT - 8, 16, input);
}
