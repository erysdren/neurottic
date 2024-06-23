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

/*
 *
 * types & defines
 *
 */

#define CON_FONTWIDTH (8)
#define CON_FONTHEIGHT (8)
#define CON_NUMLINES (RENDER_HEIGHT / CON_FONTHEIGHT)
#define CON_LINESIZE (80)
#define CON_BUFSIZE (4096)
#define CON_PREFIXSIZE (2)

typedef struct cmd {
	const char *name;
	int (*func)(int argc, char **argv);
} cmd_t;

/*
 *
 * globals
 *
 */

static SDL_bool started = SDL_FALSE;
static char textbuf[CON_BUFSIZE] = {};
static char *textbuf_ptr = NULL;
static char *lines[CON_NUMLINES] = {};
static int num_lines = 0;
static char input[CON_LINESIZE] = {};
static int input_len = 0;
static int input_cursor = 0;

/*
 *
 * commands
 *
 */

static int CMD_Quit(int argc, char **argv)
{
	Quit();
	exit(0);
	return 0;
}

static cmd_t commands[] = {
	{"quit", CMD_Quit},
	{"exit", CMD_Quit}
};

/*
 *
 * private
 *
 */

static char **tokenize(const char *s, int *num_args)
{
	static char *argv[32];
	int argc = 0;
	char *ptr, *end;

	ptr = (char *)s;
	for(;;)
	{
		while(*ptr && isspace(*ptr))
			ptr++;

		if(!*ptr)
			break;

		end = ptr + 1;

		while(*end && !isspace(*end))
			end++;

		if (argc < 32 - 1)
			argv[argc++] = ptr;

		if (!*end)
			break;

		*end = 0;
		ptr = end + 1;
	}

	argv[argc] = 0;
	*num_args = argc;
	return argv;
}

static void push_line(char *ptr)
{
	/* add line */
	lines[num_lines++] = ptr;

	/* do memmove if we've hit the screen edge */
	if (num_lines >= CON_NUMLINES - 1)
	{
		SDL_memmove(&lines[0], &lines[1], (CON_NUMLINES - 1) * sizeof(char *));
		num_lines = CON_NUMLINES - 2;
	}
}

/*
 *
 * public
 *
 */

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

void Console_Print(const char *s)
{
	int i;
	int len_src = SDL_strlen(s);

	if (!started)
		return;

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

	if (!started)
		return;

	/* do vargs */
	va_start(args, fmt);
	SDL_vsnprintf(line, sizeof(line), fmt, args);
	va_end(args);

	/* push up console buffer with the text */
	Console_Print(line);
}

void Console_Evaluate(const char *s)
{
	int argc;
	char **argv = tokenize(s, &argc);

	if (!argv || !argc)
		return;

	for (int i = 0; i < ASIZE(commands); i++)
	{
		if (SDL_strcasecmp(argv[0], commands[i].name) == 0)
			commands[i].func(argc, argv);
	}

	Log("Unknown command or cvar entered");
}

void Console_HandleInput(int c)
{
	if (!started)
		return;

	switch (c)
	{
		/* newlines */
		case '\n':
		case '\r':
			input[input_len] = '\0';
			Log(input);
			Console_Evaluate(&input[1]);
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

char *Console_GetInputLine(void)
{
	return input;
}
