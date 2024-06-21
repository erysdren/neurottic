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

int SDL_AppInit(void **appstate, int argc, char **argv)
{
	/* initialize sdl */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		return -1;

	/* initialize sdl_net */
	if (SDLNet_Init() != 0)
		return -1;

	/* initialize lump manager */
	if (LM_Init() != 0)
		return -1;

	/* initialize audio manager */
	if (AU_Init() != 0)
		return -1;

	/* load game data */
	LM_AddWAD("darkwar.wad");

	/* set music volume */
	AU_SetMusicVolume(0.5);
	AU_PlayMusic("FASTWAY", SDL_FALSE);

	return 0;
}

void SDL_AppQuit(void *appstate)
{
	AU_StopMusic();
	AU_Quit();

	LM_Quit();

	SDLNet_Quit();
	SDL_Quit();
}

int SDL_AppIterate(void *appstate)
{
	return 0;
}

int SDL_AppEvent(void *appstate, const SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT)
		return 1;

	return 0;
}
