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

#include "app.h"

static void log_func(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	static const char *priorities[SDL_NUM_LOG_PRIORITIES] = {
		[SDL_LOG_PRIORITY_VERBOSE] = "VERBOSE: ",
		[SDL_LOG_PRIORITY_DEBUG] = "DEBUG: ",
		[SDL_LOG_PRIORITY_INFO] = "INFO: ",
		[SDL_LOG_PRIORITY_WARN] = "WARNING: ",
		[SDL_LOG_PRIORITY_ERROR] = "ERROR: ",
		[SDL_LOG_PRIORITY_CRITICAL] = "CRITICAL: "
	};

	SDL_LogPriority current_priority;
	Uint64 when;
	double seconds;
	app_t *a = (app_t *)userdata;

	current_priority = SDL_GetLogPriority(category);
	if (priority < current_priority)
		return;

	when = SDL_GetTicks();

	seconds = (double)when / 1000.0f;

	fprintf(stdout, "[%09.4f] %s%s\n", seconds, priorities[priority], message);
	if (a->log.file)
		fprintf(a->log.file, "[%09.4f] %s\n", seconds, message);
}

static void get_window_dimensions(Uint32 bw, Uint32 bh, Uint32 *s, Uint32 *w, Uint32 *h)
{
	SDL_DisplayID display_id;
	Uint32 dw, dh, scale;
	const SDL_DisplayMode *display_mode;

	display_id = SDL_GetPrimaryDisplay();
	display_mode = SDL_GetDesktopDisplayMode(display_id);

	dw = display_mode->w / bw;
	dh = display_mode->h / bh;

	scale = dw < dh ? dw : dh;

	if (w) *w = bw * scale;
	if (h) *h = bh * scale;
	if (s) *s = scale;
}

/*
 * main
 */

app_t *app_create_ex(appconfig_t *config)
{
	app_t *a;
	int window_width, window_height;

	/* stoopid */
	if (!config)
		return NULL;

	/* check for SDL video init */
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		return NULL;

	/* allocate app structure */
	a = SDL_calloc(1, sizeof(app_t));
	if (!a)
		return NULL;

	/* initialize logging */
	SDL_SetLogOutputFunction(log_func, (void *)a);

	/* initialize logging to file */
	if (config->log_filename)
	{
		a->log.file = fopen(config->log_filename, config->log_append ? "ab" : "wb");
		if (!a->log.file)
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to open \"%s\" for logging\n", config->log_filename);
	}

	/* setup string stores */
	a->strings.len_store = config->len_stringstore;
	a->strings.len_tempstore = config->len_stringtempstore;
	a->strings.store = app_mem_alloc(a, a->strings.len_store);
	a->strings.tempstore = app_mem_alloc(a, a->strings.len_tempstore);
	if (!a->strings.store || !a->strings.tempstore)
		return NULL;

	/* allocate entities */
	a->world.num_entities = config->num_entities;
	a->world.max_entities = config->max_entities;
	a->world.entities = app_mem_alloc(a, a->world.num_entities * sizeof(entity_t));
	if (!a->world.entities)
		return NULL;

	/* clear new entities */
	for (int i = 0; i < a->world.num_entities; i++)
	{
		a->world.entities[i].active = SDL_FALSE;
		a->world.entities[i].id = i;
	}

	/* setup window dimensions */
	a->video.base_width = config->base_width;
	a->video.base_height = config->base_height;
	get_window_dimensions(a->video.base_width, a->video.base_height,
		&a->video.scale, &window_width, &window_height);

	/* create window */
	a->video.window = SDL_CreateWindow(config->base_title, window_width, window_height, SDL_WINDOW_OPENGL);
	if (!a->video.window)
		return NULL;

	/* create gl context */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);
	a->video.context = SDL_GL_CreateContext(a->video.window);
	if (!a->video.context)
		return NULL;

	/* setup input */
	a->input.keys = SDL_GetKeyboardState(NULL);

	/* start counting time */
	a->time.then = SDL_GetTicks();

	/* duplicate title */
	a->video.base_title = app_string_create(a, SDL_FALSE, config->base_title);
	SDL_Log("App \"%s\" initialized\n", a->video.base_title);

	return a;
}

app_t *app_create(const char *base_title, int base_width, int base_height)
{
	appconfig_t cfg;

	cfg.base_title = base_title;
	cfg.base_width = base_width;
	cfg.base_height = base_height;
	cfg.log_filename = NULL;
	cfg.log_append = SDL_FALSE;
	cfg.len_stringstore = 8192;
	cfg.len_stringtempstore = 8192;
	cfg.num_entities = 64;
	cfg.max_entities = 8192;

	return app_create_ex(&cfg);
}

void app_destroy(app_t *a)
{
	if (a)
	{
		SDL_Log("App \"%s\" shutting down\n", a->video.base_title);
		if (a->world.entities) app_mem_free(a, a->world.entities);
		if (a->strings.store) app_mem_free(a, a->strings.store);
		if (a->strings.tempstore) app_mem_free(a, a->strings.tempstore);
		if (a->video.window) SDL_DestroyWindow(a->video.window);
		if (a->video.context) SDL_GL_DeleteContext(a->video.context);
		if (a->log.file) fclose(a->log.file);
		SDL_free(a);
	}
}

int app_iterate(app_t *a)
{
	if (!a)
		return -1;

	/* update deltatime */
	a->time.now = SDL_GetTicks();
	a->time.time = (float)a->time.now / 1000.0f;
	a->time.dt = (float)(a->time.now - a->time.then) / 1000.0f;
	a->time.then = a->time.now;

	return 0;
}

int app_event(app_t *a, const SDL_Event *event)
{
	if (!a)
		return -1;
	if (event->type == SDL_EVENT_QUIT)
		return 1;
	return 0;
}

void app_frame_start(app_t *a)
{
	SDL_GL_MakeCurrent(a->video.window, a->video.context);
}

void app_frame_end(app_t *a)
{
	SDL_GL_SwapWindow(a->video.window);
}

/*
 * memory
 */

static const Uint32 membuf_tag = 0xBEEF;

typedef struct membuf {
	Uint32 tag;
	size_t size;
	void *ptr;
} membuf_t;

void *app_mem_alloc(app_t *a, size_t sz)
{
	if (!a || !sz)
		return NULL;

	membuf_t *mb = SDL_calloc(1, sizeof(membuf_t) + sz);
	if (!mb)
		return NULL;

	mb->tag = membuf_tag;
	mb->size = sz;
	mb->ptr = (Uint8 *)mb + sizeof(membuf_t);

	/* talk */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Allocated pointer %p (%zu bytes)\n", mb->ptr, mb->size);
	a->mem.num_allocations += 1;
	a->mem.bytes_allocated += sz;

	/* return usable ptr */
	return mb->ptr;
}

void app_mem_free(app_t *a, void *mem)
{
	if (!a || !mem)
		return;

	/* check for tag */
	membuf_t *mb = (membuf_t *)((Uint8 *)mem - sizeof(membuf_t));
	if (mb->tag != membuf_tag)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Memory tag 0x%04X does not match the expected tag 0x%04X!\n", mb->tag, membuf_tag);
		return;
	}

	/* talk */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Freeing pointer %p (%zu bytes)\n", mb->ptr, mb->size);
	a->mem.num_allocations -= 1;
	a->mem.bytes_allocated -= mb->size;

	/* clear structure */
	SDL_memset(mb, 0, sizeof(membuf_t));

	/* free structure */
	SDL_free(mb);
}

/*
 * strings
 */

const char *app_string_create(app_t *a, SDL_bool temp, const char *s, ...)
{
	char *dst;
	size_t max;
	va_list ap;

	if (temp)
	{
		if (a->strings.pos_tempstore >= a->strings.len_tempstore)
			a->strings.pos_tempstore = 0;

		dst = a->strings.tempstore + a->strings.pos_tempstore;
		max = a->strings.len_tempstore - a->strings.pos_tempstore;
	}
	else
	{
		if (a->strings.pos_store >= a->strings.len_store)
		{
			SDL_Log("error: out of space for static string storage");
			return NULL;
		}

		dst = a->strings.store + a->strings.pos_store;
		max = a->strings.len_store - a->strings.pos_store;
	}

	va_start(ap, s);
	vsnprintf(dst, max, s, ap);
	va_end(ap);

	if (temp)
		a->strings.pos_tempstore += SDL_strlen(dst) + 1;
	else
		a->strings.pos_store += SDL_strlen(dst) + 1;

	return (const char *)dst;
}

/*
 * entities
 */

entity_t *app_entity_spawn(app_t *a)
{
	/* hopefully guard against OOM */
	if (!a->world.entities)
		return NULL;

	/* find an inactive one */
	for (int i = 0; i < a->world.num_entities; i++)
	{
		if (a->world.entities[i].active == SDL_FALSE)
		{
			/* initialize entity */
			SDL_memset(&a->world.entities[i], 0, sizeof(entity_t));
			a->world.entities[i].active = SDL_TRUE;
			a->world.entities[i].id = i;
			return &a->world.entities[i];
		}
	}

	/* arbitrary maximum entity count */
	if (a->world.num_entities * 2 > a->world.max_entities)
		return NULL;

	/* allocate more entities if none were free */
	a->world.entities = SDL_realloc(a->world.entities, a->world.num_entities * sizeof(entity_t) * 2);

	/* setup new entities */
	for (int i = a->world.num_entities; i < a->world.num_entities * 2; i++)
	{
		a->world.entities[i].active = SDL_FALSE;
		a->world.entities[i].id = i;
	}

	a->world.num_entities *= 2;

	/* search again */
	return app_entity_spawn(a);
}

void app_entity_despawn(app_t *a, entity_t *entity)
{
	if (entity->despawn)
		entity->despawn(a, entity);

	entity->active = SDL_FALSE;
}
