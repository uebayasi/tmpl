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
#include "ss.h"
#include "sym.h"
#include "dbg.h"

#define ERR(...) do { \
	fprintf(stderr, __VA_ARGS__); \
	exit(1); \
} while (0)

struct frame *fp, *top, *bot;
struct macro_scan_ops scan;

#ifdef DEBUG
const char cs[20] = {
	[0] = '0',
	[10] = 'n',
};
#define	vc(c)	(cs[(int)c])
#endif

void
initmacro(struct macro_scan_ops *o)
{
	static char chars[STRBUF_MAX];
	static struct frame frames[MACRO_DEPTH];

	initsym();
	ss_alloc(chars, chars + STRBUF_MAX - 1);
	bot = &frames[0];
	fp = top = bot + MACRO_DEPTH - 1;
	scan = *o;
}

void
push(int op)
{
	if (fp == bot)
		ERR("stack too deep!!!\n");
	DBG("-%d\n", op);
	fp--;
	ss_push(&fp->buf);
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
		sym = ss_pop(&fp->buf);
	op = fp->op;
	if (fp == top)
		ERR("cannot pop stack!!!\n");
	fp++;
	DBG("+%d\n", op);
	*rsym = sym;
	return op;
}

void
save(char c)
{
	if (fp == top)
		(*scan.one)(c);
	else {
		if (ss_put(c))
			ERR("cannot push char!!!\n");
	}
	DUMPCHAR((fp == top) ? '{' : '[', c);
}

static void
savestr(const char *s)
{
	int c;

	while ((c = *s++) != '\0')
		save(c);
}

int
delim(void)
{
	save('\0');
	return fp->op;
}

void
new(void)
{
	fp->sym = newsym(ss_pop(&fp->buf));
}

static void
keep(const char *s)
{
	ss_keep(&fp->buf, s);
	push(0);
}

static void
unkeep(void)
{
	const char *s;

	(void)pop(&s);
	savestr(s);
}

void
define(int end)
{
	const char *var = NULL, *val;
	int op = -1;

	while (op != end) {
		if (var == NULL)
			(void)pop(&val);
		else
			val = var;
		op = pop(&var);
		setsym(newsym(var), newsym(val));
		DBG("('%s'<='%s')\n", var, val);
	}
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
	keep(pat);
	DBG("('%s'@'%s'@'%s')\n", var, val, pat);
	state = (*scan.suspend)();
	while ((val = getsym(val)) != NULL) {
		setsym(var, val);
		DBG("('%s':='%s')\n", var, val);
		(*scan.proc)(pat);
	}
	delsym(var);
	(*scan.resume)(state);
	unkeep();
}
