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

#include <ctype.h>

#define CON_NUMLINES (64)
#define CON_LINESIZE (64)
#define CON_BUFSIZE (4096)
#define CON_PREFIXSIZE (2)

static SDL_bool started = SDL_FALSE;
static char textbuf[CON_BUFSIZE] = {};
static char *textbuf_ptr = NULL;
static char *lines[CON_NUMLINES] = {};
static int num_lines = 0;
static char input[CON_LINESIZE] = {};
static int input_len = 0;
static int input_cursor = 0;

int Console_Init(void)
{
	if (started)
		return LogError("Console_Init(): Already started");

	started = SDL_TRUE;

	/* set text buffer pointer */
	textbuf_ptr = textbuf;

	/* setup input buffer */
	Console_ClearInput();

	return Log("Console initialized");
}

void Console_Quit(void)
{
	started = SDL_FALSE;
	SDL_memset(textbuf, 0, sizeof(textbuf));
	textbuf_ptr = NULL;
	SDL_memset(lines, 0, sizeof(lines));
	num_lines = 0;
	SDL_memset(input, 0, sizeof(input));
	input_len = 0;
	input_cursor = 0;
}

static void push_line(char *ptr)
{
	lines[num_lines++] = ptr;

	/* copy second half to first half if we're about to overflow */
	if (num_lines >= CON_NUMLINES - 1)
	{
		void *first;
		void *second;
		int len;

		/* pointer to first half */
		first = lines;

		/* pointer to second half */
		second = &lines[(CON_NUMLINES / 2) - 1];

		/* copy and set size */
		len = sizeof(char *) * (CON_NUMLINES / 2);

		/* copy second half to first half */
		memcpy(first, second, len);

		/* zero out second half (minus one for input line) */
		second = &lines[CON_NUMLINES / 2];
		len = sizeof(char *) * ((CON_NUMLINES / 2) - 1);
		SDL_memset(second, 0, len);

		/* set num_lines */
		num_lines = CON_NUMLINES / 2;
	}
}

void Console_Print(const char *s)
{
	int i;
	int len_src = SDL_strlen(s);

	/* bounds checks */
	if (textbuf_ptr + len_src + 1 > textbuf + CON_BUFSIZE)
		textbuf_ptr = textbuf;

	/* add string to text buffer */
	SDL_snprintf(textbuf_ptr, CON_BUFSIZE, "%s", s);

	/* add pointer to lines buffer */
	push_line(textbuf_ptr);

	/* advance text buffer pointer */
	textbuf_ptr += len_src + 1;

	/* check for newlines and push line again */
	for (i = 0; i < len_src; i++)
	{
		if (textbuf_ptr[i] == '\n')
		{
			textbuf_ptr[i] = '\0';
			push_line(&textbuf_ptr[i + 1]);
		}
	}
}

void Console_Printf(const char *fmt, ...)
{
	static char line[CON_LINESIZE];
	va_list args;

	/* do vargs */
	va_start(args, fmt);
	SDL_vsnprintf(line, sizeof(line), fmt, args);
	va_end(args);

	/* push up console buffer with the text */
	Console_Print(line);
}

void Console_HandleInput(int c)
{
	switch (c)
	{
		/* newlines */
		case '\n':
		case '\r':
			input[input_len] = '\0';
			Console_Print(input);
			Console_ClearInput();
			break;

		/* backspace */
		case '\b':
			if (input_len > CON_PREFIXSIZE)
			{
				if (input_cursor == input_len)
				{
					/* push cursor back */
					input_len--;
					input_cursor--;

					/* remove character */
					input[input_len] = '\0';
				}
			}
			break;

		/* printable */
		default:
			if (c < 256 && isprint(c) && input_len < CON_LINESIZE - 1)
			{
				if (input_cursor == input_len)
				{
					/* add character */
					input[input_len] = c;

					/* push cursor forward */
					input_len++;
					input_cursor++;
				}
			}
	}
}

void Console_Clear(void)
{
	SDL_memset(textbuf, 0, sizeof(textbuf));
	textbuf_ptr = textbuf;
	SDL_memset(lines, 0, sizeof(lines));
	Console_ClearInput();
}

void Console_ClearInput(void)
{
	/* clear buffer */
	SDL_memset(input, 0, sizeof(input));
	input_len = input_cursor = CON_PREFIXSIZE;

	/* setup prefix */
	input[0] = '>';
	input[1] = ' ';
}

char **Console_GetLines(int *n)
{
	if (n) *n = num_lines;
	return lines;
}
