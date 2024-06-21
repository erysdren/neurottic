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

#define LUMP_CACHE 0

/*
 *
 * types
 *
 */

static const char iwad_magic[4] = "IWAD";
static const char pwad_magic[4] = "PWAD";

typedef struct wad {
	char *filename;
	char magic[4];
	Sint32 num_lumps;
	Sint32 ofs_lumps;
	struct wad_lump {
		Sint32 ofs;
		Sint32 len;
		char name[8];
	} *lumps;
	void *data;
} wad_t;

#if LUMP_CACHE
/* cached lump */
typedef struct cached_lump {
	char name[8];
	Sint32 size;
	struct {
		Sint32 which;
		Sint32 index;
	} wad;
	struct {
		Sint32 which;
		char *path;
	} path;
} cached_lump_t;
#endif

/*
 *
 * globals
 *
 */

/* is lump manager running? */
static SDL_bool started = SDL_FALSE;

/* wad file handles */
static int num_wads = 0;
static wad_t **wads = NULL;

/* paths strings */
static int num_paths = 0;
static char **paths = NULL;

#if LUMP_CACHE
/* cached lump indexes */
static int num_cached_lumps = 0;
static cached_lump_t *cached_lumps = NULL;
#endif

/*
 *
 * private functions
 *
 */

#if LUMP_CACHE
static int LM_CacheLumpsFromWAD(int w)
{
	if (w < 0 || w >= num_wads)
		return -1;

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "LM: Caching %d lumps from \"%s\"", wads[w]->num_lumps, wads[w]->filename);

	/* increase cache size */
	cached_lumps = SDL_realloc(cached_lumps, num_cached_lumps + wads[w]->num_lumps * sizeof(cached_lump_t));

	/* do caching */
	for (int i = num_cached_lumps; i < num_cached_lumps + wads[w]->num_lumps; i++)
	{
		/* setup generic fields */
		SDL_strlcpy(cached_lumps[i].name, wads[w]->lumps[i - num_cached_lumps].name, 8);
		cached_lumps[i].size = wads[w]->lumps[i - num_cached_lumps].len;

		/* setup wad fields */
		cached_lumps[i].wad.which = w;
		cached_lumps[i].wad.index = i - num_cached_lumps;

		/* setup path fields */
		cached_lumps[i].path.which = -1;
		cached_lumps[i].path.path = NULL;
	}

	/* increase total cached lumps */
	num_cached_lumps += wads[w]->num_lumps;

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "LM: WAD cache size: %d", num_cached_lumps);

	return wads[w]->num_lumps;
}
#endif

/*
 *
 * public functions
 *
 */

/* startup lump manager */
int LM_Init(void)
{
	if (started)
		return -1;

	started = SDL_TRUE;

	return 0;
}

/* shutdown lump manager */
void LM_Quit(void)
{
	if (wads)
	{
		for (int i = 0; i < num_wads; i++)
		{
			if (wads[i])
			{
				if (wads[i]->lumps)
					SDL_free(wads[i]->lumps);

				if (wads[i]->filename)
					SDL_free(wads[i]->filename);

				if (wads[i]->data)
					SDL_free(wads[i]->data);

				SDL_free(wads[i]);
			}
		}

		SDL_free(wads);
	}

	if (paths)
	{
		for (int i = 0; i < num_paths; i++)
		{
			if (paths[i])
				SDL_free(paths[i]);
		}

		SDL_free(paths);
	}

#if LUMP_CACHE
	if (cached_lumps)
	{
		for (int i = 0; i < num_cached_lumps; i++)
		{
			if (cached_lumps[i].path.path)
				SDL_free(cached_lumps[i].path.path);
		}

		SDL_free(cached_lumps);
	}
#endif

	num_wads = 0;
	wads = NULL;

	num_paths = 0;
	paths = NULL;

#if LUMP_CACHE
	num_cached_lumps = 0;
	cached_lumps = NULL;
#endif

	started = SDL_FALSE;
}

/* add wad to lump manager */
int LM_AddWAD(const char *filename)
{
	SDL_IOStream *io = NULL;
	wad_t *wad = NULL;
	void *data = NULL;
	size_t size = 0;
	int ret = -1;

	/* stoopid */
	if (!filename)
		goto fail;

	/* load file */
	data = SDL_LoadFile(filename, &size);
	if (!data)
		goto fail;

	/* create io */
	io = SDL_IOFromConstMem(data, size);
	if (!io)
		goto fail;

	/* allocate wad */
	wad = SDL_calloc(1, sizeof(wad_t));
	if (!wad)
		goto fail;

	/* check magic */
	SDL_ReadIO(io, wad->magic, 4);
	if (SDL_memcmp(wad->magic, iwad_magic, 4) != 0 && SDL_memcmp(wad->magic, pwad_magic, 4) != 0)
		goto fail;

	/* read data */
	SDL_ReadS32LE(io, &wad->num_lumps);
	SDL_ReadS32LE(io, &wad->ofs_lumps);

	/* allocate lumps */
	wad->lumps = SDL_calloc(wad->num_lumps, sizeof(struct wad_lump));
	if (!wad->lumps)
		goto fail;

	/* read lumps */
	SDL_SeekIO(io, wad->ofs_lumps, SDL_IO_SEEK_SET);
	SDL_ReadIO(io, wad->lumps, wad->num_lumps * sizeof(struct wad_lump));

	/* assign other data */
	wad->filename = SDL_strdup(filename);
	wad->data = data;

#if LUMP_CACHE
	/* cache wads */
	if (!LM_CacheLumpsFromWAD(wad))
		goto fail;
#endif

	/* add to wads list */
	num_wads++;
	wads = SDL_realloc(wads, sizeof(wad_t *) * num_wads);
	wads[num_wads - 1] = wad;

	/* done */
	ret = 0;
	goto done;

fail:
	if (wad)
	{
		if (wad->lumps)
			SDL_free(wad->lumps);
		SDL_free(wad);
	}
	if (data)
		SDL_free(data);
done:
	if (io)
		SDL_CloseIO(io);
	return ret;
}

/* add filesystem path to lump manager */
int LM_AddPath(const char *path)
{
	SDL_PathInfo info;

	/* stoopid */
	if (!path)
		return -1;

	/* check if path exists */
	if (SDL_GetPathInfo(path, &info) != 0)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
		return -1;
	}

	/* check if its a directory */
	if (info.type != SDL_PATHTYPE_DIRECTORY)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "\"%s\" is not a directory", path);
		return -1;
	}

	/* add to paths list */
	num_paths++;
	paths = SDL_realloc(paths, sizeof(char *) * num_paths);
	paths[num_paths - 1] = SDL_strdup(path);

	return 0;
}

/* load entire lump into buffer. this data should be freed with SDL_free */
void *LM_LoadLump(const char *name, size_t *sz)
{
	int i, x;

	/* search paths */
	i = num_paths;
	while (--i >= 0)
	{
		static char path[256];
		SDL_snprintf(path, sizeof(path), "%s/%s", paths[i], name);

		void *data = NULL;
		if ((data = SDL_LoadFile(path, sz)) != NULL)
			return data;
	}

	/* search wads */
	i = num_wads;
	while (--i >= 0)
	{
		for (x = 0; x < wads[i]->num_lumps; x++)
		{
			/* null lumps can't be loaded */
			if (!wads[i]->lumps[x].ofs || !wads[i]->lumps[x].len)
				continue;

			/* simple name search */
			if (SDL_strncasecmp(name, wads[i]->lumps[x].name, 8) == 0)
			{
				void *ptr = (Uint8 *)wads[i]->data + (size_t)wads[i]->lumps[x].ofs;
				SDL_IOStream *io = SDL_IOFromConstMem(ptr, (size_t)wads[i]->lumps[x].len);
				return SDL_LoadFile_IO(io, sz, SDL_TRUE);
			}
		}
	}

	return NULL;
}

/* open lump for reading with SDL_IOStream */
SDL_IOStream *LM_OpenLumpIO(const char *name)
{
	int i, x;

	/* search paths */
	i = num_paths;
	while (--i >= 0)
	{
		static char path[256];
		SDL_snprintf(path, sizeof(path), "%s/%s", paths[i], name);

		SDL_IOStream *io = NULL;
		if ((io = SDL_IOFromFile(path, "rb")) != NULL)
			return io;
	}

	/* search wads */
	i = num_wads;
	while (--i >= 0)
	{
		for (x = 0; x < wads[i]->num_lumps; x++)
		{
			/* null lumps can't be opened */
			if (!wads[i]->lumps[x].ofs || !wads[i]->lumps[x].len)
				continue;

			/* simple name search */
			if (SDL_strncasecmp(name, wads[i]->lumps[x].name, 8) == 0)
			{
				void *ptr = (Uint8 *)wads[i]->data + (size_t)wads[i]->lumps[x].ofs;
				return SDL_IOFromConstMem(ptr, (size_t)wads[i]->lumps[x].len);
			}
		}
	}

	return NULL;
}

/* get lump index by name */
Sint32 LM_GetLumpIndex(const char *name)
{
	int i, x;

	/* search wads */
	i = num_wads;
	while (--i >= 0)
	{
		for (x = 0; x < wads[i]->num_lumps; x++)
		{
			/* simple name search */
			if (SDL_strncasecmp(name, wads[i]->lumps[x].name, 8) == 0)
				return x;
		}
	}

	/* search paths */
	i = num_paths;
	while (--i >= 0)
	{
		static char path[256];
		SDL_snprintf(path, sizeof(path), "%s/%s", paths[i], name);

		/* exists on disk, but not in a wad */
		SDL_IOStream *io;
		if ((io = SDL_IOFromFile(path, "rb")) != NULL)
		{
			SDL_CloseIO(io);
			return -2;
		}
	}

	return -1;
}
