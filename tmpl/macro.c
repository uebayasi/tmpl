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

//#define	DEBUG

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

struct frame frames[MACRO_DEPTH];
struct frame *fp, *top, *bottom;

struct macro_scan_ops scan_ops;

#ifdef DEBUG
const char cs[20] = {
	[0] = '0',
	[10] = 'n',
};
#define	vc(c)	(cs[(int)c])
#endif

void
initmacro(struct macro_scan_ops *ops)
{
	static char chars[STRBUF_MAX];

	initsym();
	ss_alloc(chars, STRBUF_MAX);
	bottom = &frames[0];
	fp = top = &frames[MACRO_DEPTH - 1];
	scan_ops = *ops;
}

int
ispushed(void)
{
	return (fp < top);
}

void
push(int op)
{
	if (fp == bottom)
		ERR("stack too deep!!!\n");
	DBG("-%d\n", op);
	fp--;

	ss_push(&fp->buf);
	fp->sym = NULL;
	fp->op = op;
}

void
pushstr(int op)
{
	push(op);
	fp->sym = (void *)-1;
}

int
pop(const char **rsym)
{
	const char *sym;
	int op;

	if (fp->sym != NULL && fp->sym != (void *)-1)
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

static void
dup(const char *s)
{
	ss_dup(&fp->buf, s);
}

void
save(char c)
{
	char l;

	if (!ispushed()) {
		fputc(c, stdout);
		l = '{';
	} else {
		ss_put(&fp->buf, c);
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
	if (fp->sym != (void *)-1) {
		fp->sym = newsym(ss_pop(&fp->buf));
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
	dup(pat);
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
