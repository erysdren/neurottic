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

#include "font8x8.h"

/*
 * software renderer implementation
 */

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Surface *surface8 = NULL;
static SDL_Surface *surface24 = NULL;
static SDL_Texture *texture = NULL;
SDL_Surface *font = NULL;

/* initialize renderer */
int R_Init(void)
{
	Uint32 format;

	window = SDL_CreateWindow("NEUROTTIC", RENDER_WIDTH, RENDER_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window)
		return -1;

	SDL_SetWindowMinimumSize(window, RENDER_WIDTH, RENDER_HEIGHT);

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)
		return -1;

	SDL_SetRenderVSync(renderer, 1);
	SDL_SetRenderLogicalPresentation(renderer, RENDER_WIDTH, RENDER_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX, SDL_SCALEMODE_NEAREST);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	surface8 = SDL_CreateSurface(RENDER_WIDTH, RENDER_HEIGHT, SDL_PIXELFORMAT_INDEX8);
	if (!surface8)
		return -1;

	SDL_FillSurfaceRect(surface8, NULL, 0);

	format = SDL_GetWindowPixelFormat(window);

	surface24 = SDL_CreateSurface(RENDER_WIDTH, RENDER_HEIGHT, format);
	if (!surface24)
		return -1;

	texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, RENDER_WIDTH, RENDER_HEIGHT);
	if (!texture)
		return -1;

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	/* create font atlas */
	font = SDL_CreateSurface(256 * 8, 8, SDL_PIXELFORMAT_INDEX8);
	if (!font)
		return -1;

	SDL_FillSurfaceRect(font, NULL, 0);
	SDL_SetSurfaceColorKey(font, SDL_TRUE, 0);

	for (int c = 0; c < 256; c++)
	{
		char *bitmap = font8x8[c];

		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				if (bitmap[y] & 1 << x)
				{
					size_t ofs = y * font->pitch + x + (8 * c);
					((Uint8 *)font->pixels)[ofs] = 255;
				}
			}
		}
	}

	return 0;
}

/* shutdown renderer */
void R_Quit(void)
{
	if (window) SDL_DestroyWindow(window);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (surface8) SDL_DestroySurface(surface8);
	if (surface24) SDL_DestroySurface(surface24);
	if (texture) SDL_DestroyTexture(texture);
	if (font) SDL_DestroySurface(font);
	window = NULL;
	renderer = NULL;
	surface8 = NULL;
	surface24 = NULL;
	texture = NULL;
	font = NULL;
}

/* clear screen */
void R_Clear(Uint8 color)
{
	SDL_FillSurfaceRect(surface8, NULL, color);
}

/* draw scene */
int R_Draw(void)
{
	return 0;
}

/* draw filled rect */
int R_DrawRect(int x, int y, int w, int h, Uint8 color)
{
	const SDL_Rect rect = {x, y, w, h};
	return SDL_FillSurfaceRect(surface8, &rect, color);
}

/* draw surface */
int R_DrawSurface(int x, int y, int w, int h, SDL_Surface *surface)
{
	const SDL_Rect rect = {x, y, w, h};
	return SDL_SoftStretch(surface, NULL, surface8, &rect, SDL_SCALEMODE_NEAREST);
}

/* flip to visible screen */
void R_Flip(void)
{
	SDL_BlitSurface(surface8, NULL, surface24, NULL);
	SDL_UpdateTexture(texture, NULL, surface24->pixels, surface24->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void R_SetPalette_REAL(Uint8 *palette)
{
	SDL_Color colors[256];

	for (int i = 0; i < 256; i++)
	{
		colors[i].r = palette[i * 3];
		colors[i].g = palette[i * 3 + 1];
		colors[i].b = palette[i * 3 + 2];
	}

	SDL_SetPaletteColors(surface8->format->palette, colors, 0, 256);
}

void R_SetPosition(float x, float y, float z)
{

}

void R_SetAngles(float x, float y, float z)
{

}

/* draw string at x,y with color */
void R_DrawString(int x, int y, Uint8 color, const char *fmt, ...)
{
	static char buffer[1024];
	va_list ap;

	va_start(ap, fmt);
	SDL_vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	for (int i = 0; i < SDL_strlen(buffer); i++)
	{
		if (buffer[i] >= 0 && buffer[i] < 256)
		{
			SDL_Rect srcrect = {buffer[i] * 8, 0, 8, 8};
			SDL_Rect dstrect = {x, y, 8, 8};
			SDL_BlitSurface(font, &srcrect, surface8, &dstrect);
			x += 8;
		}
	}
}
