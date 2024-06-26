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

#pragma once
#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_
#ifdef __cplusplus
extern "C" {
#endif

/* initialize audio manager */
int AU_Init(void);

/* shutdown audio manager */
void AU_Quit(void);

/* set current music volume between 0.0 and 1.0 */
void AU_SetMusicVolume(float volume);

/* play music from name */
int AU_PlayMusic(const char *name, SDL_bool loop);

/* stop currently playing music */
void AU_StopMusic(void);

#ifdef __cplusplus
}
#endif
#endif /* _AUDIO_MANAGER_H_ */
