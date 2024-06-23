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
#ifndef _LOGGING_H_
#define _LOGGING_H_
#ifdef __cplusplus
extern "C" {
#endif

/* start logging to specified filename, or NULL */
int Logging_Start(const char *filename, SDL_bool append);

/* shutdown logging */
void Logging_Quit(void);

/* log standard message */
#define Log(...) ({SDL_Log(__VA_ARGS__); 0;})

/* log verbose message */
#define LogVerbose(...) (SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__), 0)

/* log warning message */
#define LogWarning(...) (SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__), 0)

/* log critical message */
#define LogCritical(...) (SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__), 0)

/* log error message */
#define LogError(...) (SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__), SDL_SetError(__VA_ARGS__))

#ifdef __cplusplus
}
#endif
#endif /* _LOGGING_H_ */
