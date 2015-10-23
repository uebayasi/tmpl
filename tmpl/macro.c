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

#if 0
#define	DBG(...)	do {} while (0)
#define	DUMPBUF()	do {} while (0)
#else
#define	DBG(...)	do { \
	int d = stack.depth; \
	while (d++ < MACRO_DEPTH) fputc('\t', stderr); \
	fprintf(stderr, __VA_ARGS__); \
} while (0)
#define	DUMPBUF()	do { \
	char *str; \
	fprintf(stderr, "===|"); \
	for (str = strbuf.head; str != strbuf.tail; str++) { \
		char c = *str; \
		char l = '['; \
		if (c < 0x20) \
			fprintf(stderr, "%c\\%c%c", l, (cs[(int)c] == 0) ? '?' : cs[(int)c], l + 2); \
		else \
			fprintf(stderr, "%c %c%c", l, c, l + 2); \
	} \
	fprintf(stderr, "|===\n"); \
} while (0)
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

const char cs[20] = {
	[0] = '0',
	[10] = 'n',
};

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
	DBG("-%d\n", op);
	stack.depth--;

	/* init new frame */
	fb->head = fb->tail = sb->tail;
	fp->sym = NULL;
	fp->op = op;
}

int
pop(const char **rsym)
{
	const char *sym;
	int op;

	if (fp->sym != NULL)
		sym = fp->sym;
	else
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
	char l;

	if (!ispushed()) {
		fputc(c, stdout);
		l = '{';
	} else {
		if (sb->tail - sb->head == STRBUF_MAX)
			ERR("buffer overflow!!!\n");
		*sb->tail++ = c;
		fb->tail++;
		l = '[';
	}
	if (c < 0x20)
		DBG("%c\\%c%c\n", l, (cs[(int)c] == 0) ? '?' : cs[(int)c], l + 2);
	else
		DBG("%c %c%c\n", l, c, l + 2);
}

static void
savestr(const char *s)
{
	int c;

	while ((c = *s++) != '\0')
		save(c);
}

int
end(void)
{
	save('\0');
	fp->sym = newsym(fb->head);
	sb->tail = fb->tail = fb->head;
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
	const char *var, *val, *l, *r, *str;

	(void)pop(&var);
	var = newsym(var);
	val = getsym(var);
	if (val == NULL) {
		DBG("('%s'=='%s')\n", var, var);
		l = "~{";
		r = "~}";
		str = var;
	} else {
		DBG("('%s'=>'%s')\n", var, val);
		l = "";
		r = "";
		str = val;
	}
	savestr(l);
	savestr(str);
	savestr(r);
}

void
template(void)
{
	const char *var;
	const char *val;
	const char *pat;
	char *str;
	void *state;

	DUMPBUF();

	(void)pop(&pat);
	(void)pop(&val);
	(void)pop(&var);
	str = strdup(pat);
	DBG("('%s'@'%s'@'%s')\n", var, val, str);
	push(0);

	/* suspend current lex state */
	state = (*scan_ops.suspend)();

	/* process template */
	while ((val = getsym(val)) != NULL) {
		setsym(var, val);
		DBG("('%s':='%s')\n", var, val);
		(*scan_ops.proc)(str);
	}
	delsym(var);
	free(str);

	(void)pop(&pat);
	savestr(pat);

	/* resume previous lex state */
	(*scan_ops.resume)(state);

	DUMPBUF();
}
