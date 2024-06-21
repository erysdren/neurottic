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

float rad2degf(float x)
{
	return x * 180.0f / M_PI;
}

float deg2radf(float x)
{
	return x * M_PI / 180.0f;
}

float wrapf(float value, float mod)
{
	float cmp = (float)(value < 0);
	return cmp * mod + SDL_fmodf(value, mod) - cmp;
}

float remapf(float value, float a1, float a2, float b1, float b2)
{
	return b1 + (value - a1) * (b2 - b1) / (a2 - a1);
}

float minf(float a, float b)
{
	return a < b ? a : b;
}

float maxf(float a, float b)
{
	return a > b ? a : b;
}

float clampf(float value, float min, float max)
{
	return minf(maxf(value, min), max);
}

float towardsf(float value, float change, float to)
{
	float diff;

	if (value < to)
		value += change;
	else
		value -= change;

	diff = SDL_fabs(value - to);

	if (diff < change)
		value = to;

	return value;
}

void Vec3f_anglevectors(Vec3f angles, Vec3f *forward, Vec3f *right, Vec3f *up)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = deg2radf(angles.y);
	sy = SDL_sinf(angle);
	cy = SDL_cosf(angle);

	angle = deg2radf(angles.x);
	sp = SDL_sinf(angle);
	cp = SDL_cosf(angle);

	angle = deg2radf(angles.z);
	sr = SDL_sinf(angle);
	cr = SDL_cosf(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = -1 * sr * sp * cy + -1 * cr * -sy;
		right->y = -1 * sr * sp * sy + -1 * cr * cy;
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

Vec3f Vec3f_crossproduct(Vec3f v1, Vec3f v2)
{
	Vec3f result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

float Vec3f_dotproduct(Vec3f v1, Vec3f v2)
{
	float result = 0.0f;
	result += v1.x * v2.x;
	result += v1.y * v2.y;
	result += v1.z * v2.z;
	return result;
}

float Vec3f_normalize(Vec3f *v)
{
	float w = SDL_sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= w;
	v->y /= w;
	v->z /= w;
	return w;
}
