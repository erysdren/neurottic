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

static Uint8 global_palette[768] = {};

void R_SetPalette_REAL(Uint8 *palette);

void R_SetPalette(Uint8 *palette)
{
	SDL_memcpy(global_palette, palette, 768);
	R_SetPalette_REAL(global_palette);
}

#define NUM_PLANES (4)

/* create SDL_Surface from planar pic */
SDL_Surface *R_SurfaceFromPicIO(SDL_IOStream *io, SDL_bool closeio)
{
	Uint8 w, h;

	/* stoopid */
	if (!io)
	{
		LogError("R_SurfaceFromPicIO(): NULL pointer passed as IOStream");
		return NULL;
	}

	/* read width */
	if (SDL_ReadU8(io, &w) != SDL_TRUE)
	{
		LogError("R_SurfaceFromPicIO(): Failed to read width from IOStream");
		return NULL;
	}

	/* read height */
	if (SDL_ReadU8(io, &h) != SDL_TRUE)
	{
		LogError("R_SurfaceFromPicIO(): Failed to read height from IOStream");
		return NULL;
	}

	/* create surface */
	SDL_Surface *surface = SDL_CreateSurface(w * NUM_PLANES, h, SDL_PIXELFORMAT_INDEX8);
	if (!surface)
		return NULL;

	/* allocate some memory to read the source pixels */
	Uint8 *src = (Uint8 *)alloca(w);
	if (!src)
	{
		LogError("R_SurfaceFromPicIO(): alloca() failed with %zu bytes", w);
		SDL_DestroySurface(surface);
		return NULL;
	}

	/* plane by plane */
	for (int p = 0; p < NUM_PLANES; p++)
	{
		/* row by row */
		for (int y = 0; y < h; y++)
		{
			/* read plane */
			if (SDL_ReadIO(io, (void *)src, w) != w)
			{
				LogError("R_SurfaceFromPicIO(): %s", SDL_IOStream_StatusString(SDL_GetIOStatus(io)));
				SDL_DestroySurface(surface);
				return NULL;
			}

			/* get destination pointer */
			/* TODO: should this also be a temporary IOStream? */
			Uint8 *dst = &((Uint8 *)surface->pixels)[y * surface->pitch];

			/* plot pixels */
			for (int x = 0; x < w; x++)
			{
				dst[x * NUM_PLANES + p] = src[x];
			}
		}
	}

	/* close io stream if user requested */
	if (closeio)
		SDL_CloseIO(io);

	/* set surface key color */
	SDL_SetSurfaceColorKey(surface, SDL_TRUE, 0xFF);

	return surface;
}

/* draw console */
void R_DrawConsole(Uint8 color)
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
			R_DrawString(0, y, color, lines[i]);
			y += 8;

			/* leave room for input line */
			if (y >= RENDER_HEIGHT - 8)
				break;
		}
	}

	/* draw input line */
	R_DrawString(0, RENDER_HEIGHT - 8, color, input);
}

/* find closest approximation of RGB color in the indexed palette */
Uint8 R_FindColor(Uint8 r, Uint8 g, Uint8 b)
{
	int i, j;
	int best_index = -1;
	int best_dist = 0;
	Uint8 rgb[3] = {r, g, b};

	for (i = 0; i < 256; i++)
	{
		int dist = 0;

		for (j = 0; j < 3; j++)
		{
			int d = SDL_abs(rgb[j] - global_palette[i * 3 + j]);
			dist += d * d;
		}

		if (best_index == -1 || dist < best_dist)
		{
			best_index = i;
			best_dist = dist;
		}
	}

	return (Uint8)best_index;
}
