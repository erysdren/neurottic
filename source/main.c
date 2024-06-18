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

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "app.h"
#include "wad.h"

static int die(const char *error)
{
	fprintf(stderr, "%s\n", error);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", error, NULL);
	return -1;
}

int SDL_AppInit(void **appstate, int argc, char **argv)
{
	/* init sdl */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		return die(SDL_GetError());

	/* init app */
	app_t *a = app_create("NEUROTTIC", 640, 480);
	if (!a)
		return die(SDL_GetError());

	/* give it back to sdl */
	*appstate = a;

	return 0;
}

void SDL_AppQuit(void *appstate)
{
	app_t *a = (app_t *)appstate;
	app_destroy(a);
	SDL_Quit();
}

int SDL_AppIterate(void *appstate)
{
	app_t *a = (app_t *)appstate;
	int r;

	if ((r = app_iterate(a)) != 0)
		return r;

	app_frame_start(a);
	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	app_frame_end(a);

	return 0;
}

int SDL_AppEvent(void *appstate, const SDL_Event *event)
{
	app_t *a = (app_t *)appstate;
	return app_event(a, event);
}
