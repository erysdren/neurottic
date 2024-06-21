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
#ifndef _LM_H_
#define _LM_H_
#ifdef __cplusplus
extern "C" {
#endif

/*
 * neurottic lump manager
 *
 * paths are always searched before wads, allowing for easy user replacements.
 * lumps have no file extension, and thus they are not tested when loading from
 * disk instead of wad.
 *
 * wads are searched in reverse order from when they were added, so later wads
 * will overwrite earlier wads. the same is true for paths.
 *
 * NOTE: thread safety has not been tested, but since all buffers and handles
 * are strictly read-only, it may be okay.
 */

#include <SDL3/SDL.h>

/* startup lump manager */
int LM_Init(void);

/* shutdown lump manager
 *
 * closes all currently open wads and deletes all path references. it is safe
 * to call LM_Init() again after this.
 *
 * NOTE: all SDL_IOStream handles returned from LM_OpenLumpIO() will become
 * invalid after this! however, data from LM_LoadLump() will be unaffected.
 */
void LM_Quit(void);

/* add wad file by name to lump manager
 *
 * NOTE: uses case-insensitive searching, so DARKWAR.WAD and darkwar.wad will
 * be equivalent.
 */
int LM_AddWAD(const char *filename);

/* add filesystem path to lump manager
 *
 * NOTE: uses case-insensitive searching, so ./lumps/ and ./LUMPS/ will
 * be equivalent.
 */
int LM_AddPath(const char *path);

/* load entire lump into buffer
 *
 * NOTE: should be freed with SDL_free after use.
 */
void *LM_LoadLump(const char *name, size_t *sz);

/* open lump for reading with SDL_IOStream
 *
 * NOTE: should always be closed with SDL_CloseIO after use.
 */
SDL_IOStream *LM_OpenLumpIO(const char *name);

/* get lump index by name
 *
 * returns -1 if the lump doesn't exist, and -2 if the lump only exists on
 * disk and not in a wad file
 */
Sint32 LM_GetLumpIndex(const char *name);

#ifdef __cplusplus
}
#endif
#endif /* _LM_H_ */
