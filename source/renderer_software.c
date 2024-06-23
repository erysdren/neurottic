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

#include "thirdp/font8x8_basic.h"

/*
 * software renderer implementation
 */

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define RENDER_WIDTH (640)
#define RENDER_HEIGHT (400)

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Surface *surface8 = NULL;
static SDL_Surface *surface24 = NULL;
static SDL_Texture *texture = NULL;

static Uint16 *planes[3] = {};

/* initialize renderer */
int R_Init(void)
{
	Uint32 format;

	planes[0] = MS_GetCurrentMapPlane(0);
	planes[1] = MS_GetCurrentMapPlane(1);
	planes[2] = MS_GetCurrentMapPlane(2);

	if (!planes[0] || !planes[1] || !planes[2])
		return LogError("R_Init(): No map loaded");

	window = SDL_CreateWindow("NEUROTTIC", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (!window)
		return -1;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)
		return -1;

	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH, SDL_SCALEMODE_NEAREST);

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
	window = NULL;
	renderer = NULL;
	surface8 = NULL;
	surface24 = NULL;
	texture = NULL;
}

/* clear screen */
void R_Clear(Uint8 color)
{
	SDL_FillSurfaceRect(surface8, NULL, color);
}

/* draw scene */
void R_Draw(void)
{

}

/* draw console */
void R_DrawConsole(void)
{
	int num_lines;
	char **lines = Console_GetLines(&num_lines);
	int y = 0;

	for (int i = 0; i < num_lines; i++)
	{
		if (lines[i])
		{
			R_DrawString(0, y, 0xFF, lines[i]);
			y += 8;
		}
	}
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

void R_SetPalette(Uint8 *palette)
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

static void draw_font8x8(int x, int y, Uint8 color, char *bitmap)
{
	int xx, yy;

	for (yy = y; yy < y + 8; yy++)
	{
		for (xx = x; xx < x + 8; xx++)
		{
			if (xx < 0 || yy < 0)
				continue;
			if (xx >= surface8->w || yy >= surface8->h)
				break;

			if (bitmap[yy - y] & 1 << xx - x)
				((Uint8 *)surface8->pixels)[yy * surface8->pitch + xx] = color;
		}
	}
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
		draw_font8x8(x, y, color, font8x8_basic[buffer[i]]);
		x += 8;
	}
}
