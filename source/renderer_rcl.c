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

/*
 * renderer implementation using raycastlib
 */

#define RCL_TEXTURE_VERTICAL_STRETCH 0
#define RCL_PIXEL_FUNCTION put_pixel
#include "thirdp/raycastlib.h"

#define RCL_UNIT(x) ((RCL_Unit)((x) * RCL_UNITS_PER_SQUARE))

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define RENDER_WIDTH (640)
#define RENDER_HEIGHT (400)

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Surface *surface8 = NULL;
static SDL_Surface *surface24 = NULL;
static SDL_Texture *texture = NULL;

static RCL_Camera camera = {};

static Uint16 *planes[3] = {};

void put_pixel(RCL_PixelInfo *pixel)
{
	Uint8 color;

	if (pixel->isWall)
	{
		color = (Uint8)(pixel->depth >> 8);
	}
	else
	{
		color = 0x00;
	}

	((Uint8 *)surface8->pixels)[pixel->position.y * surface8->pitch + pixel->position.x] = color;
}

RCL_Unit floor_at(int16_t x, int16_t y)
{
	if (x < 0 || x >= 128 || y < 0 || y >= 128)
		return RCL_UNIT(0);

	if (planes[0][y * 128 + x] > 0)
		return RCL_UNIT(1);

	return RCL_UNIT(0);
}

/* initialize renderer */
int R_Init(void)
{
	Uint32 format;

	planes[0] = MS_GetCurrentMapPlane(0);
	planes[1] = MS_GetCurrentMapPlane(1);
	planes[2] = MS_GetCurrentMapPlane(2);

	if (!planes[0] || !planes[1] || !planes[2])
		return -1;

	window = SDL_CreateWindow("NEUROTTIC", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH, SDL_SCALEMODE_NEAREST);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	surface8 = SDL_CreateSurface(RENDER_WIDTH, RENDER_HEIGHT, SDL_PIXELFORMAT_INDEX8);
	SDL_FillSurfaceRect(surface8, NULL, 0);

	format = SDL_GetWindowPixelFormat(window);
	surface24 = SDL_CreateSurface(RENDER_WIDTH, RENDER_HEIGHT, format);
	texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, RENDER_WIDTH, RENDER_HEIGHT);

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	RCL_initCamera(&camera);

	camera.position.x = RCL_UNIT(41.5);
	camera.position.y = RCL_UNIT(41.5);
	camera.shear = 0;
	camera.direction = 0;
	camera.height = RCL_UNIT(0.5);
	camera.resolution.x = RENDER_WIDTH;
	camera.resolution.y = RENDER_HEIGHT;

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

/* draw scene */
int R_Draw(void)
{
	RCL_RayConstraints c;
	RCL_initRayConstraints(&c);
	c.maxHits = 128;
	c.maxSteps = 128;

	RCL_renderSimple(camera, floor_at, NULL, NULL, c);

	SDL_BlitSurface(surface8, NULL, surface24, NULL);
	SDL_UpdateTexture(texture, NULL, surface24->pixels, surface24->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	return 0;
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
	camera.position.x = RCL_UNIT(x);
	camera.position.y = RCL_UNIT(y);
	camera.height = RCL_UNIT(z);
}

void R_SetAngles(float x, float y, float z)
{
	y = wrapf(y, 360);
	y = remapf(y, 0, 360, 0, 1);
	camera.direction = RCL_UNIT(y);
}
