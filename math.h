
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
#ifndef _MATH_H_
#define _MATH_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * vec2
 */

typedef struct vec2f {
	float x, y;
} vec2f_t;

typedef struct vec2i {
	int32_t x, y;
} vec2i_t;

/*
 * vec3
 */

typedef struct vec3f {
	float x, y, z;
} vec3f_t;

typedef struct vec3i {
	int32_t x, y, z;
} vec3i_t;

/*
 * vec4
 */

typedef struct vec4f {
	float x, y, z, w;
} vec4f_t;

typedef struct vec4i {
	int32_t x, y, z, w;
} vec4i_t;

/*
 * mat4
 */

typedef struct mat4f {
	union {
		struct {
			vec4f_t x, y, z, w;
		};
		float m[4][4];
	};
} mat4f_t;

typedef struct mat4i {
	union {
		struct {
			vec4i_t x, y, z, w;
		};
		int32_t m[4][4];
	};
} mat4i_t;

float rad2degf(float x);
float deg2radf(float x);
float wrapf(float value, float mod);
float minf(float a, float b);
float maxf(float a, float b);
float clampf(float value, float min, float max);
float towardsf(float value, float change, float to);
void vec3f_anglevectors(vec3f_t angles, vec3f_t *forward, vec3f_t *right, vec3f_t *up);
vec3f_t vec3f_crossproduct(vec3f_t v1, vec3f_t v2);
float vec3f_dotproduct(vec3f_t v1, vec3f_t v2);
float vec3f_normalize(vec3f_t *v);

#ifdef __cplusplus
}
#endif
#endif /* _MATH_H_ */
