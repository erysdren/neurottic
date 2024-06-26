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

static Mix_Music *current_chunk = NULL;

static const SDL_AudioSpec mixer_spec = {
	SDL_AUDIO_S16,
	2,
	44100
};

int AU_Init(void)
{
	/* setup audio device */
	if (Mix_OpenAudio(0, &mixer_spec) != 0)
		return -1;

	/* setup soundfonts */
	const char *soundfonts = Mix_GetSoundFonts();
	if (soundfonts)
	{
		Log("Using soundfonts string \"%s\"", soundfonts);
		Mix_SetSoundFonts(soundfonts);
	}
	else
	{
		LogWarning("AU_Init(): Failed to setup soundfonts");
	}

	return Log("Initialized audio manager");
}

void AU_Quit(void)
{
	AU_StopMusic();
	Mix_Quit();
}

void AU_SetMusicVolume(float volume)
{
	volume = clampf(volume, 0, 1);
	volume = remapf(volume, 0, 1, 0, MIX_MAX_VOLUME);
	Mix_VolumeMusic((int)volume);
}

int AU_PlayMusic(const char *name, SDL_bool loop)
{
	SDL_IOStream *io;
	Mix_Music *chunk;

	/* stoopid */
	if (!name)
		return LogError("AU_PlayMusic(): NULL pointer passed as music name");

	/* open lump io */
	io = LM_OpenLumpIO(name);
	if (!io)
		return -1;

	/* load music chunk */
	chunk = Mix_LoadMUS_IO(io, SDL_TRUE);
	if (!chunk)
		return -1;

	/* stop playing current music */
	AU_StopMusic();

	/* play new music */
	current_chunk = chunk;
	return Mix_PlayMusic(current_chunk, loop ? -1 : 0);
}

void AU_StopMusic(void)
{
	if (Mix_PlayingMusic() || Mix_PausedMusic())
		Mix_HaltMusic();

	if (current_chunk)
		Mix_FreeMusic(current_chunk);

	current_chunk = NULL;
}
