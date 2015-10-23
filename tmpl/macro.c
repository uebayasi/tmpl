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
#include "macro.h"
#include "sym.h"

#if 0
#define	DBG(...)	do {} while (0)
#define	DBGINDENT()	do {} while (0)
#define	DUMPCHAR()	do {} while (0)
#define	DUMPBUF()	do {} while (0)
#else
#define	DEBUG
#define	DBGINDENT()	do { \
	int d = stack.depth; \
	while (d++ < MACRO_DEPTH) fputc('\t', stderr); \
} while (0)
#define	DBG(...)	do { \
	DBGINDENT(); \
	fprintf(stderr, __VA_ARGS__); \
} while (0)
#define	PC(c)		((vc(c) == 0) ? '?' : vc(c))
#define	DUMPCHAR(l, c, n)	do { \
	if ((c) < 0x20) \
		fprintf(stderr, "%c%c%c%c%s", (l), '\\', PC(c), (l) + 2, (n)); \
	else \
		fprintf(stderr, "%c%c%c%c%s", (l), ' ', c, (l) + 2, (n)); \
} while (0)
#define	DUMPBUF()	do { \
	char *str; \
	fprintf(stderr, "===|"); \
	for (str = strbuf.head; str != strbuf.tail; str++) { \
		char c = *str; \
		DUMPCHAR('[', c, ""); \
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
#define	vc(c)	(cs[(int)c])

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

void
pushstr(int op)
{
	push(op);
	fp->sym = -1;
}

int
pop(const char **rsym)
{
	const char *sym;
	int op;

	if (fp->sym != NULL && fp->sym != -1)
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

static void
dupstr(const char *s)
{
	while (*s++ != '\0')
		continue;
	fb->head = fb->tail = sb->tail = s;
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
	DBGINDENT();
	DUMPCHAR(l, c, "\n");
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
	if (fp->sym != -1) {
		fp->sym = newsym(fb->head);
		sb->tail = fb->tail = fb->head;
	}
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

static void
unexpand(const char *s)
{
	savestr("~{");
	savestr(s);
	savestr("~}");
}

void
expand(void)
{
	const char *var, *val;

	(void)pop(&var);
	var = newsym(var);
	val = getsym(var);
	if (val == NULL) {
		DBG("('%s'=='%s')\n", var, var);
		unexpand(var);
	} else {
		DBG("('%s'=>'%s')\n", var, val);
		savestr(val);
	}
}

void
template(void)
{
	const char *var, *val, *pat;
	void *state;

	(void)pop(&pat);
	(void)pop(&val);
	(void)pop(&var);
	dupstr(pat);
	push(0);
	DBG("('%s'@'%s'@'%s')\n", var, val, pat);

	state = (*scan_ops.suspend)();
	while ((val = getsym(val)) != NULL) {
		setsym(var, val);
		DBG("('%s':='%s')\n", var, val);
		(*scan_ops.proc)(pat);
	}
	delsym(var);
	(*scan_ops.resume)(state);

	(void)pop(&pat);
	savestr(pat);
}
