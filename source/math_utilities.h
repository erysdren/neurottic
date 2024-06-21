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
#ifndef _MATH_UTILITIES_H_
#define _MATH_UTILITIES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>

/*
 * vec2
 */

typedef struct Vec2f {
	float x, y;
} Vec2f;

typedef struct Vec2i {
	Sint32 x, y;
} Vec2i;

/*
 * vec3
 */

typedef struct Vec3f {
	float x, y, z;
} Vec3f;

typedef struct Vec3i {
	Sint32 x, y, z;
} Vec3i;

/*
 * vec4
 */

typedef struct Vec4f {
	float x, y, z, w;
} Vec4f;

typedef struct Vec4i {
	Sint32 x, y, z, w;
} Vec4i;

/*
 * mat4
 */

typedef union Mat4f {
	struct {
		Vec4f x, y, z, w;
	};
	float m[4][4];
} Mat4f;

typedef union Mat4i {
	struct {
		Vec4i x, y, z, w;
	};
	Sint32 m[4][4];
} Mat4i;

float rad2degf(float x);
float deg2radf(float x);
float wrapf(float value, float mod);
float remapf(float value, float a1, float a2, float b1, float b2);
float minf(float a, float b);
float maxf(float a, float b);
float clampf(float value, float min, float max);
float towardsf(float value, float change, float to);

void Vec3f_anglevectors(Vec3f angles, Vec3f *forward, Vec3f *right, Vec3f *up);
Vec3f Vec3f_crossproduct(Vec3f v1, Vec3f v2);
float Vec3f_dotproduct(Vec3f v1, Vec3f v2);
float Vec3f_normalize(Vec3f *v);

#ifdef __cplusplus
}
#endif
#endif /* _MATH_UTILITIES_H_ */
