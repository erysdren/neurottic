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
#ifndef _APP_H_
#define _APP_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include <SDL3/SDL.h>

#include "math.h"

/*
 *
 * misc
 *
 */

#define ASIZE(a) (sizeof(a) / sizeof(a[0]))

#define print_vec2f(v) printf(#v ": %0.4f %0.4f\n", (v)->x, (v)->y)
#define print_vec2i(v) printf(#v ": %d %d\n", (v)->x, (v)->y)
#define print_vec3f(v) printf(#v ": %0.4f %0.4f %0.4f\n", (v)->x, (v)->y, (v)->z)
#define print_vec3i(v) printf(#v ": %d %d %d\n", (v)->x, (v)->y, (v)->z)
#define print_float(x) printf(#x ": %0.4f\n", *(x))
#define print_int(x) printf(#x ": %d\n", *(x))
#define print_hex(x) printf(#x ": 0x%x\n", *(x))
#define print_rect(r) printf(#r ": x=%d y=%d w=%d h=%d\n", (r)->x, (r)->y, (r)->w, (r)->h)
#define print_frect(r) printf(#r ": x=%0.4f y=%0.4f w=%0.4f h=%0.4f\n", (r)->x, (r)->y, (r)->w, (r)->h)

/*
 *
 * types
 *
 */

/*
 * entity
 */

typedef struct entity entity_t;
typedef struct app app_t;
typedef struct entity {
	SDL_bool active;
	int id;
	vec3f_t origin;
	vec3f_t angles;
	vec3f_t velocity;
	vec3f_t mins;
	vec3f_t maxs;
	int (*event)(app_t *a, entity_t *self, const SDL_Event *event);
	void (*draw)(app_t *a, entity_t *self);
	void (*touch)(app_t *a, entity_t *self, entity_t *other);
	void (*spawn)(app_t *a, entity_t *self);
	void (*despawn)(app_t *a, entity_t *self);
} entity_t;

/*
 * main application config
 */

typedef struct appconfig {
	const char *base_title;
	Uint32 base_width;
	Uint32 base_height;
	const char *log_filename;
	SDL_bool log_append;
	Uint32 len_stringstore;
	Uint32 len_stringtempstore;
	Uint32 num_entities;
	Uint32 max_entities;
} appconfig_t;

/*
 * main application state
 */

typedef struct app {
	struct {
		SDL_Window *window;
		const char *base_title;
		Uint32 base_width;
		Uint32 base_height;
		Uint32 scale;
	} video;
	struct {
		const Uint8 *keys;
	} input;
	struct {
		Uint64 then;
		Uint64 now;
		float dt;
		float time;
	} time;
	struct {
		FILE *file;
	} log;
	struct {
		char *store;
		size_t pos_store;
		size_t len_store;
		char *tempstore;
		size_t pos_tempstore;
		size_t len_tempstore;
	} strings;
	struct {
		Uint32 max_entities;
		Uint32 num_entities;
		entity_t *entities;
	} world;
	struct {
		size_t num_allocations;
		size_t bytes_allocated;
	} mem;
} app_t;

/*
 *
 * prototypes
 *
 */

/* create app with config structure */
app_t *app_create_ex(appconfig_t *config);

/* create app */
app_t *app_create(const char *base_title, int base_width, int base_height);

/* deallocate all memory and destroy app */
void app_destroy(app_t *a);

/* do frame */
int app_iterate(app_t *a);

/* process SDL event */
int app_event(app_t *a, const SDL_Event *event);

/* allocate memory */
void *app_mem_alloc(app_t *a, size_t sz);

/* free memory */
void app_mem_free(app_t *a, void *mem);

/* create a string. if temp == SDL_TRUE, the string will later be overwritten
 * by other temp strings. otherwise, it will be static for the lifetime of the
 * app. these strings should not be manually freed.
 */
const char *app_string_create(app_t *a, SDL_bool temp, const char *s, ...);

/* spawn new entitiy in the world */
entity_t *app_entity_spawn(app_t *a);

/* despawn entity from pointer */
void app_entity_despawn(app_t *a, entity_t *entity);

#ifdef __cplusplus
}
#endif
#endif /* _APP_H_ */
