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
				/* figure out what went wrong */
				switch (SDL_GetIOStatus(io))
				{
					case SDL_IO_STATUS_ERROR:
						LogError("R_SurfaceFromPicIO(): General IOStream error");
						break;

					case SDL_IO_STATUS_EOF:
						LogError("R_SurfaceFromPicIO(): Reached end of IOStream early");
						break;

					case SDL_IO_STATUS_NOT_READY:
						LogError("R_SurfaceFromPicIO(): IOStream not ready");
						break;

					case SDL_IO_STATUS_WRITEONLY:
						LogError("R_SurfaceFromPicIO(): Write-only IOStream");
						break;

					default:
						LogError("R_SurfaceFromPicIO(): Unknown IOStream error");
						break;
				}

				/* cleanup and exit */
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
