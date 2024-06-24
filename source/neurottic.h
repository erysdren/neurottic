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
#ifndef _NEUROTTIC_H_
#define _NEUROTTIC_H_
#ifdef __cplusplus
extern "C" {
#endif

/* libc */
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

/* SDL */
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <SDL3_mixer/SDL_mixer.h>

/* misc */
#define ASIZE(a) (sizeof(a) / sizeof(a[0]))

/* audio manager */
#include "audio_manager.h"

/* console */
#include "console.h"

/* logging */
#include "logging.h"

/* lump manager */
#include "lump_manager.h"

/* main */
#include "main.h"

/* mapset */
#include "mapset.h"

/* math utilities */
#include "math_utilities.h"

/* renderer */
#include "renderer.h"

/* rlew compression */
#include "rlew.h"

#ifdef __cplusplus
}
#endif
#endif /* _NEUROTTIC_H_ */
