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

/*
 *
 * types
 *
 */

/* mapset flags */
enum {
	MAPSET_SINGLEPLAYER = 1 << 0,
	MAPSET_COMMBAT = 1 << 1,
	MAPSET_LUDICROUS = 1 << 2,
	MAPSET_SHAREWARE = 1 << 3,
	MAPSET_RANDROTT = 1 << 4
};

/* map flags */
enum {
	MAP_OPEN_PUSHWALLS = 1 << 0
};

/* map structure */
typedef struct map {
	Uint32 used;
	Uint32 crc;
	Uint32 tag;
	Uint32 flags;
	Uint32 plane_offsets[3];
	Uint32 plane_sizes[3];
	char name[24];
} map_t;

/* magics */
static const Uint32 rtl_version = 0x0101;
static const Uint32 rxl_version = 0x0200;
static const Uint32 rle_tag_registered = 0x4344;
static const Uint32 rle_tag_shareware = 0x4D4B;
static const char rtl_magic[4] = "RTL\0";
static const char rtc_magic[4] = "RTC\0";
static const char rtr_magic[4] = "RTR\0";
static const char rxl_magic[4] = "RXL\0";
static const char rxc_magic[4] = "RXC\0";

/*
 *
 * globals
 *
 */

/* current mapset */
static SDL_IOStream *mapset_io = NULL;
static char *mapset_filename = NULL;
static Uint32 mapset_flags = 0;
static Uint32 mapset_version = 0;

/* current map */
static map_t maps[100] = {};
static int map_index = -1;
static Uint16 *map_planes[3] = {};

/*
 *
 * public functions
 *
 */

int MS_LoadMapSet(const char *filename)
{
	char magic[4];
	SDL_IOStream *io;
	Uint32 flags;
	Uint32 version;

	/* stoopid */
	if (!filename)
		return -1;

	/* open file */
	io = SDL_IOFromFile(filename, "rb");
	if (!io)
		return -1;

	/* test magic */
	SDL_ReadIO(io, magic, 4);
	if (SDL_memcmp(magic, rtl_magic, 4) == 0)
	{
		flags |= MAPSET_SINGLEPLAYER;
	}
	else if (SDL_memcmp(magic, rtc_magic, 4) == 0)
	{
		flags |= MAPSET_COMMBAT;
	}
	else if (SDL_memcmp(magic, rtr_magic, 4) == 0)
	{
		flags |= MAPSET_RANDROTT;
	}
	else if (SDL_memcmp(magic, rxl_magic, 4) == 0)
	{
		flags |= MAPSET_SINGLEPLAYER | MAPSET_LUDICROUS;
	}
	else if (SDL_memcmp(magic, rxc_magic, 4) == 0)
	{
		flags |= MAPSET_COMMBAT | MAPSET_LUDICROUS;
	}
	else
	{
		SDL_CloseIO(io);
		return -1;
	}

	/* test version */
	SDL_ReadU32LE(io, &version);
	if (version == rxl_version)
	{
		flags |= MAPSET_LUDICROUS;
	}
	else if (version != rtl_version)
	{
		SDL_CloseIO(io);
		return -1;
	}

	/* unload whatever is there */
	MS_UnloadMapSet();

	/* read maps */
	SDL_SeekIO(io, 8, SDL_IO_SEEK_SET);
	for (int i = 0; i < 100; i++)
	{
		SDL_ReadU32LE(io, &maps[i].used);
		SDL_ReadU32LE(io, &maps[i].crc);
		SDL_ReadU32LE(io, &maps[i].tag);
		SDL_ReadU32LE(io, &maps[i].flags);
		SDL_ReadU32LE(io, &maps[i].plane_offsets[0]);
		SDL_ReadU32LE(io, &maps[i].plane_offsets[1]);
		SDL_ReadU32LE(io, &maps[i].plane_offsets[2]);
		SDL_ReadU32LE(io, &maps[i].plane_sizes[0]);
		SDL_ReadU32LE(io, &maps[i].plane_sizes[1]);
		SDL_ReadU32LE(io, &maps[i].plane_sizes[2]);
		SDL_ReadIO(io, maps[i].name, 24);
	}

	/* save data */
	mapset_io = io;
	mapset_flags = flags;
	mapset_version = version;
	mapset_filename = SDL_strdup(filename);

	SDL_Log("Successfully loaded \"%s\"", mapset_filename);

	return 0;
}

void MS_UnloadMapSet(void)
{
	MS_UnloadMap();

	if (mapset_filename)
		SDL_free(mapset_filename);

	if (mapset_io)
		SDL_CloseIO(mapset_io);

	SDL_memset(maps, 0, sizeof(maps));

	mapset_io = NULL;
	mapset_filename = NULL;
}

int MS_LoadMap(int map)
{
	/* out of range */
	if (map < 0 || map >= 100)
		return -1;

	/* not used in this mapset */
	if (!maps[map].used)
		return -1;

	map_index = map;

	map_planes[0] = SDL_calloc(1, 128 * 128 * 2);
	map_planes[1] = SDL_calloc(1, 128 * 128 * 2);
	map_planes[2] = SDL_calloc(1, 128 * 128 * 2);

	/* decompress planes */
	for (int i = 0; i < 3; i++)
	{
		void *compressed = SDL_calloc(1, maps[map].plane_sizes[i]);

		SDL_SeekIO(mapset_io, maps[map].plane_offsets[i], SDL_IO_SEEK_SET);
		SDL_ReadIO(mapset_io, compressed, maps[map].plane_sizes[i]);

		rlew_uncompress(compressed, maps[map].plane_sizes[i], maps[map].tag, map_planes[i], 128 * 128 * 2);

		SDL_free(compressed);
	}

	return 0;
}

void MS_UnloadMap(void)
{
	if (map_planes[0]) SDL_free(map_planes[0]);
	if (map_planes[1]) SDL_free(map_planes[1]);
	if (map_planes[2]) SDL_free(map_planes[2]);

	map_index = -1;
	SDL_memset(map_planes, 0, sizeof(map_planes));
}

/* get pointer to decompressed plane from current map (valid range is 0-2) */
Uint16 *MS_GetCurrentMapPlane(int plane)
{
	if (plane < 0 || plane >= 3)
		return NULL;
	return map_planes[plane];
}
