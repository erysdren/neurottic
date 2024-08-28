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
#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef enum FS_Path {
	FS_PATH_BASE,
	FS_PATH_PREF
} FS_Path;

int FS_Init(void);
void FS_Quit(void);

/** opens a file for reading, writing or appending
 *
 * NOTE: only files in the FS_PATH_PREF path can be written or appended to.
 *
 * @param name path to file name
 * @param mode mode string
 * @param path base path to use
 * @returns opened IOStream, or NULL on error
 */
SDL_IOStream *FS_OpenFile(const char *name, const char *mode, FS_Path path);

#ifdef __cplusplus
}
#endif
#endif /* _FILESYSTEM_H_ */
