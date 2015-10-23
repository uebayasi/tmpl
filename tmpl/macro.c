/*
 * Copyright (c) 2015 Masao Uebayashi <uebayasi@tombiinc.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"
#include "sym.h"

#if 1
#define	DBG(...)
#else
#define	DBG(...)	fprintf(stderr, __VA_ARGS__)
#endif
#define ERR(...) do { \
	fprintf(stderr, __VA_ARGS__); \
	exit(1); \
} while (0)

#define fp	(&(stack.frames[stack.depth]))
#define	fb	(&(fp->strbuf))
#define	sb	(&(strbuf))

struct frame frames[MACRO_DEPTH];
struct stack stack;
char chars[STRBUF_MAX];
struct strbuf strbuf;
struct macro_scan_ops scan_ops;

void
initmacro(struct macro_scan_ops *ops)
{
	initsym();
	stack.frames = &frames[0];
	stack.depth = MACRO_DEPTH;
	sb->head = &chars[0];
	sb->tail = &chars[0];
	scan_ops = *ops;
}

int
ispushed(void)
{
	return (stack.depth < MACRO_DEPTH);
}

void
push(int op)
{
	/* get new frame */
	if (stack.depth == 0)
		ERR("stack too deep!!!\n");
	stack.depth--;
	DBG("-%d\n", op);

	/* init new frame */
	fb->head = fb->tail = sb->tail;
	fp->op = op;
}

int
pop(const char **rsym)
{
	const char *sym;
	int op;

	/* alloc current string as symbol */
	if (fb->tail - fb->head == MACRO_MAX)
		ERR("macro name too long!!!\n");
	sym = fb->head;

	/* fini current frame */
	sb->tail = fb->head;
	op = fp->op;
	fb->head = fb->tail = NULL;
	fp->op = 0;
	if (stack.depth == MACRO_DEPTH)
		ERR("cannot pop stack!!!\n");

	/* put current frame */
	stack.depth++;
	DBG("+%d\n", op);
	
	*rsym = sym;
	return op;
}

void
save(char c)
{
	if (!ispushed()) {
		fputc(c, stdout);
		DBG("{%c}\n", c);
	} else {
		if (sb->tail - sb->head == STRBUF_MAX)
			ERR("buffer overflow!!!\n");
		*sb->tail++ = c;
		fb->tail++;
		DBG("[%c]\n", c);
	}
}

int
end(void)
{
	save('\0');
	return fp->op;
}

int
define(const char *var, const char **rvar)
{
	const char *val;
	int op;

	if (var == NULL)
		(void)pop(&val);
	else
		val = var;
	op = pop(&var);
	setsym(newsym(var), newsym(val));
	DBG("('%s'<='%s')\n", var, val);
	*rvar = var;
	return op;
}

void
expand(void)
{
	const char *var, *val;
	char c;

	(void)pop(&var);
	var = newsym(var);
	val = getsym(var);
	if (val == NULL) {
		save('~');
		save('{');
		while ((c = *var++) != '\0')
			save(c);
		save('~');
		save('}');
	} else {
		DBG("('%s'=>'%s')\n", var, val);
		while ((c = *val++) != '\0')
			save(c);
	}
}

void
template(void)
{
	const char *var;
	const char *val;
	const char *pat;
	char *str;
	void *state;

	(void)pop(&pat);
	(void)pop(&val);
	(void)pop(&var);
	val = newsym(val);
	var = newsym(var);
	str = strdup(pat);
	DBG("('%s'@'%s'@'%s')\n", var, val, str);

	/* suspend current lex state */
	state = (*scan_ops.suspend)();

	/* process template */
	while ((val = getsym(val)) != NULL) {
		setsym(var, val);
		DBG("('%s'<='%s')\n", var, val);
		(*scan_ops.proc)(str);
	}
	delsym(var);
	free(str);

	/* resume previous lex state */
	(*scan_ops.resume)(state);
}
