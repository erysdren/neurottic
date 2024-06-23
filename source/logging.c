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

#include <stdio.h>

static SDL_IOStream *log_file = NULL;

static void log_func(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
	Uint64 when;
	double seconds;
	static const char *priorities[SDL_NUM_LOG_PRIORITIES] = {
		[SDL_LOG_PRIORITY_VERBOSE] = "VERBOSE: ",
		[SDL_LOG_PRIORITY_DEBUG] = "DEBUG: ",
		[SDL_LOG_PRIORITY_INFO] = "INFO: ",
		[SDL_LOG_PRIORITY_WARN] = "WARNING: ",
		[SDL_LOG_PRIORITY_ERROR] = "ERROR: ",
		[SDL_LOG_PRIORITY_CRITICAL] = "CRITICAL: "
	};

	if (priority < SDL_GetLogPriority(category))
		return;

	when = SDL_GetTicks();
	seconds = (double)when / 1000.0f;

	fprintf(stdout, "[%09.4f] %s%s\n", seconds, priorities[priority], message);

	Console_Print(message);

	if (userdata)
		SDL_IOprintf((SDL_IOStream *)userdata, "[%09.4f] %s%s\n", seconds, priorities[priority], message);
}

int Logging_Init(const char *filename, SDL_bool append)
{
	if (filename)
	{
		if (log_file)
		{
			LogWarning("Logging_Init(): Closing existing log file");
			SDL_CloseIO(log_file);
		}

		log_file = SDL_IOFromFile(filename, append ? "ab" : "wb");
		if (!log_file)
			LogWarning("Logging_Init(): Unable to open log file \"%s\" for writing", filename);
	}

	SDL_SetLogOutputFunction(log_func, (void *)log_file);
	if (log_file)
		Log("Started logging to \"%s\"", filename);

	return 0;
}

void Logging_Quit(void)
{
	SDL_SetLogOutputFunction(NULL, NULL);
	if (log_file) SDL_CloseIO(log_file);
	log_file = NULL;
}
