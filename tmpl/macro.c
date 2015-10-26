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

struct frame *f, *top, *bot;
struct macro_scan_ops *scan;

void
initmacro(struct macro_scan_ops *o)
{
	static char chars[STRBUF_MAX];
	static char *strs[MACRO_DEPTH];
	static struct frame frames[MACRO_DEPTH];

	initsym();
	ss_alloc(chars, chars + STRBUF_MAX, strs, strs + MACRO_DEPTH);
	bot = &frames[0];
	f = top = bot + MACRO_DEPTH;
	scan = o;
}

void
finimacro(void)
{
}

void
push(int op)
{
	if (f == bot)
		ERR("stack too deep!!!\n");
	f--;
	ss_push();
	f->sym = NULL;
	f->op = op;
}

static const char *
pop(void)
{
	const char *sym;

	if (f->sym != NULL)
		sym = f->sym;
	else
		sym = ss_pop();
	if (f == top)
		ERR("cannot pop stack!!!\n");
	f++;
	return sym;
}

void
save(char c)
{
	if (f == top)
		(*scan->write)(c);
	else if (ss_put(c))
		ERR("cannot push char!!!\n");
	DUMPCHAR((f == top) ? '{' : '[', c);
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
	return f->op;
}

void
new(void)
{
	f->sym = newsym(ss_pop());
}

static void
keep(char *s)
{
	ss_keep(s);
	while (*s++ != '\0')
		continue;
	ss_keep(s);
	f--;
}

static void
unkeep(void)
{
	char *s;

	f++;
	s = ss_pop();
	ss_unkeep(ss_pop());
	savestr(s);
}

void
define(int end)
{
	const char *var = NULL, *val;
	int op = -1;

	while (op != end) {
		if (var == NULL)
			val = pop();
		else
			val = var;
		op = f->op;
		var = pop();
		setsym(newsym(var), newsym(val));
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

	var = pop();
	var = newsym(var);
	val = getsym(var);
	if (val == NULL)
		unexpand(var);
	else
		savestr(val);
}

void
template(void)
{
	const char *var, *val, *pat;
	void *state;

	pat = pop();
	val = pop();
	var = pop();
	keep(pat);
	state = (*scan->suspend)();
	while ((val = getsym(val)) != NULL) {
		setsym(var, val);
		(*scan->read)(pat);
	}
	delsym(var);
	(*scan->resume)(state);
	save('\0');
	unkeep();
}
