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

#include <stddef.h>
#include <string.h>
#include "queue.h"
#include "macro.h"
#include "ss.h"
#include "sym.h"
#include "dbg.h"

#ifndef nitems
#define	nitems(_a)	(sizeof((_a)) / sizeof((_a)[0]))
#endif

struct local;
struct local {
	SLIST_ENTRY(local) entry;
	const char *var;
	const char *val;
};
SLIST_HEAD(locallist, local);
struct locallist locals = SLIST_HEAD_INITIALIZER(locals);

struct frame *f, *top, *bot;
struct macro_ops *scan;

void
initmacro(struct macro_ops *o)
{
	static char chars[SS_NCHARS];
	static char *strs[SS_NSTRS];
	static struct frame frames[MACRO_DEPTH];

	initsym();
	ss_alloc(chars, chars + nitems(chars), strs, strs + nitems(strs));
	bot = frames;
	f = top = bot + nitems(frames);
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
		(*scan->error)("stack too deep!!!\n");
	f--;
	if (ss_push())
		(*scan->error)("cannot push string!!!\n");
	f->sym = NULL;
	f->op = op;
}

static const char *
pop(void)
{
	const char *sym;

	if (f->sym != NULL)
		sym = f->sym;
	else {
		sym = ss_pop();
		if (sym == (void *)-1)
			(*scan->error)("cannot pop string!!!\n");
	}
	if (f == top)
		(*scan->error)("cannot pop stack!!!\n");
	f++;
	return sym;
}

void
save(char c)
{
	if (f == top)
		(*scan->write)(c);
	else if (ss_put(c))
		(*scan->error)("cannot push char!!!\n");
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
	char *s;

	if ((s = ss_pop()) == (void *)-1)
		(*scan->error)("cannot pop string!!!\n");
	f->sym = newsym(s);
}

static void
keep(char *s)
{
	if (ss_keep(s))
		(*scan->error)("cannot push char!!!\n");
	f--;
}

static char *
unkeep(void)
{
	f++;
	return ss_unkeep();
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

static const char *
lookuplocal(const char *var)
{
	struct local *l;

	SLIST_FOREACH(l, &locals, entry) {
		if (strcmp(var, l->var) == 0)
			break;
	}
	return (l != NULL) ? l->val : NULL;
}

static const char *
lookup(const char *var)
{
	const char *val;

	if ((val = lookuplocal(var)) != NULL)
		return val;
	else {
		var = newsym(var);
		return getsym(var);
	}
}

void
expand(void)
{
	const char *var, *val;

	var = pop();
	val = lookup(var);
	if (val == NULL)
		unexpand(var);
	else
		savestr(val);
}

static void
localiterscan(const char *var, const char *val)
{
	struct local l;

	l.var = var;
	l.val = val;
	SLIST_INSERT_HEAD(&locals, &l, entry);
	(*scan->scan)();
	SLIST_REMOVE_HEAD(&locals, entry);
}

static void
localiterread(const char *var, const char *val, const char *pat)
{
	struct local l;

	l.var = var;
	l.val = val;
	SLIST_INSERT_HEAD(&locals, &l, entry);
	(*scan->read)(pat);
	SLIST_REMOVE_HEAD(&locals, entry);
}

void
local(void)
{
	const char *var, *val;
	char *s;

	val = pop();
	var = pop();
	keep(var);
	keep(val);
	ss_push();
	f--;
	localiterscan(var, val);
	f++;
	s = ss_pop();
	(void)unkeep();
	(void)unkeep();
	savestr(s);
}

static void
templateiter(const char *var, const char *val, const char *pat)
{
	while ((val = getsym(val)) != NULL)
		localiterread(var, val, pat);
}

void
template(void)
{
	const char *var, *val, *pat;

	pat = pop();
	val = pop();
	var = pop();
	keep(pat);
	templateiter(var, val, pat);
	save('\0');
	savestr(unkeep());
}

static void
splititer(const char *var, char sep, const char *val, const char *pat)
{
	char *p, *q;

	p = val;
	while (1) {
		if (p == pat)
			break;
		q = strchr(p, sep);
		if (q == NULL)
			break;
		*q = '\0';
		localiterread(var, p, pat);
		p = q + 1;
	}
}

void
split(void)
{
	const char *var, *sep, *val, *pat;
	char *s;

	pat = pop();
	val = pop();
	sep = pop();
	var = pop();
	keep(var);
	keep(sep);
	keep(val);
	keep(pat);
	ss_push();
	f--;
	splititer(var, sep[0], val, pat);
	f++;
	s = ss_pop();
	(void)unkeep();
	(void)unkeep();
	(void)unkeep();
	(void)unkeep();
	savestr(s);
}
