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

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

/* current game state enum */
enum {
	GAMESTATE_CONSOLE,
	GAMESTATE_MENU,
	GAMESTATE_LOADING,
	GAMESTATE_INGAME,
};

static int gamestate = GAMESTATE_CONSOLE;

/*
 * engine functions
 */

SDL_Surface *testpic;

void Quit(void)
{
	Log("Shutting down");
	R_Quit();
	MS_UnloadMapSet();
	AU_Quit();
	LM_Quit();
	Logging_Quit();
	Console_Quit();
	SDLNet_Quit();
	SDL_Quit();
}

int Start(void)
{
	/* sdl */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		return -1;

	/* sdl_net */
	if (SDLNet_Init() != 0)
		return -1;

	/* init console */
	if (Console_Init() != 0)
		return -1;

	/* start logging */
	if (Logging_Init("neurottic.log", SDL_FALSE) != 0)
		return -1;

	/* lump manager */
	if (LM_Init() != 0)
		return -1;

	/* audio manager */
	if (AU_Init() != 0)
		return -1;

	return 0;
}

int Restart(void)
{
	Quit();
	return Start();
}

void Die(const char *fmt, ...)
{
	static char error[2048];
	va_list ap;

	va_start(ap, fmt);
	SDL_vsnprintf(error, sizeof(error), fmt, ap);
	va_end(ap);

	LogCritical(error);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical Error", error, NULL);

	Quit();

	exit(1);
}

/*
 * sdl_main callbacks
 */

int SDL_AppInit(void **appstate, int argc, char **argv)
{
	/* startup systems */
	if (Start() != 0)
		Die(SDL_GetError());

	/* load game wad */
	if (LM_AddWAD("darkwar.wad") != 0)
		Die(SDL_GetError());

	/* add lumps searchpath */
	if (LM_AddPath("lumps") != 0)
		Die(SDL_GetError());

	/* load mapset */
	if (MS_LoadMapSet("darkwar.rtl") != 0)
		Die(SDL_GetError());

	/* load first map */
	if (MS_LoadMap(0) != 0)
		Die(SDL_GetError());

	/* initialize renderer */
	if (R_Init() != 0)
		Die(SDL_GetError());

	/* play music */
	AU_SetMusicVolume(0.5);
	if (AU_PlayMusic("FASTWAY", SDL_TRUE) != 0)
		Die(SDL_GetError());

	Log("Playing FASTWAY");

	/* set palette */
	Uint8 *palette = (Uint8 *)LM_LoadLump("PAL", NULL);
	R_SetPalette(palette);
	SDL_free(palette);

	/* load a pic */
	Uint8 *mmbk = (Uint8 *)LM_LoadLump("MMBK", NULL);
	testpic = R_SurfaceFromPic(mmbk[0], mmbk[1], &mmbk[2]);
	SDL_free(mmbk);

	return 0;
}

void SDL_AppQuit(void *appstate)
{
	SDL_DestroySurface(testpic);
	Quit();
}

int SDL_AppIterate(void *appstate)
{
	R_Clear(0x00);

	switch (gamestate)
	{
		case GAMESTATE_CONSOLE:
			R_Draw();
			R_DrawConsole();
			break;

		default:
			return -1;
	}

	if (R_DrawSurface(0, 0, testpic->w, testpic->h, testpic) != 0)
		Log(SDL_GetError());

	R_Flip();
	return 0;
}

int SDL_AppEvent(void *appstate, const SDL_Event *event)
{
	/* always quit */
	if (event->type == SDL_EVENT_QUIT)
		return 1;

	/* handle gamestate-specific events */
	switch (gamestate)
	{
		case GAMESTATE_CONSOLE:
			if (event->type == SDL_EVENT_KEY_DOWN)
				Console_HandleInput(event->key.keysym.sym);
			break;

		default:
			break;
	}

	return 0;
}
