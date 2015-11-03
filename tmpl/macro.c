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

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"
#include "macro.h"
#include "ss.h"
#include "sym.h"
#include "dbg.h"
#include "misc.h"

struct local {
	SLIST_ENTRY(local) entry;
	const char *var;
	const char *val;
};
SLIST_HEAD(locallist, local);
struct locallist locals = SLIST_HEAD_INITIALIZER(locals);

struct frame *f, *top, *bot;
struct macro_ops *ops;

void
initmacro(struct macro_ops *o)
{
	static char chars[SS_NCHARS];
	static char *strs[SS_NSTRS];
	static struct frame frames[MACRO_DEPTH];

	initsym();
	ss_init(chars, chars + nitems(chars), strs, strs + nitems(strs));
	bot = frames;
	f = top = bot + nitems(frames);
	ops = o;
}

void
finimacro(void)
{
	assert(f == top);
	ss_fini();
}

void
push(int cmd)
{
	if (f == bot)
		(*ops->error)("stack too deep!!!\n");
	f--;
	if (ss_push())
		(*ops->error)("cannot push string!!!\n");
	f->sym = NULL;
	f->cmd = cmd;
}

static const char *
pop(void)
{
	const char *sym;

	if (f->sym != NULL)
		sym = f->sym;
	else {
		if ((sym = ss_pop()) == (void *)-1)
			(*ops->error)("cannot pop string!!!\n");
	}
	if (f == top)
		(*ops->error)("cannot pop stack!!!\n");
	f++;
	return sym;
}

void
save(char c)
{
	if (f == top)
		(*ops->write)(c);
	else if (ss_put(c))
		(*ops->error)("cannot push char!!!\n");
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
	return f->cmd;
}

void
new(void)
{
	char *s;

	if ((s = ss_pop()) == (void *)-1)
		(*ops->error)("cannot pop string!!!\n");
	f->sym = newsym(s);
}

static const char **
getvars(int n)
{
	f += n;
	return (const char **)ss_getvars(n);
}

static const char *
putvars(int n)
{
	return ss_putvars(n);
}

void
define(int end)
{
	const char *var = NULL, *val;
	int cmd = -1;

	while (cmd != end) {
		val = (var == NULL) ? pop() : var;
		cmd = f->cmd;
		var = pop();
		setsym(newsym(var), newsym(val));
	}
}

static const char *
lookup(const char *var)
{
	struct local *l;

	SLIST_FOREACH(l, &locals, entry) {
		if (strcmp(var, l->var) == 0)
			break;
	}
	return (l != NULL) ? l->val : getsym(newsym(var));
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
	const char **v;
	const char *val;

	v = getvars(1);
	if ((val = lookup(v[0])) == NULL)
		unexpand(v[0]);
	else
		savestr(val);
	ss_put('\0');
	savestr(putvars(1));
}

static void
localiter(const char *var, const char *val, const char *pat)
{
	struct local l = { .var = var, .val = val };

	SLIST_INSERT_HEAD(&locals, &l, entry);
	(*ops->scan)(pat);
	SLIST_REMOVE_HEAD(&locals, entry);
}

void
local(void)
{
	const char **v;

	v = getvars(2);
	localiter(v[0], v[1], NULL);
	(void)putvars(2);
}

static void
templateiter(const char *var, const char *val, const char *pat)
{
	while ((val = lookup(val)) != NULL)
		localiter(var, val, pat);
}

void
template(void)
{
	const char **v;

	v = getvars(3);
	templateiter(v[0], v[1], v[2]);
	ss_put('\0');
	savestr(putvars(3));
}

static void
splititer(const char *var, const char *sep, const char *val, const char *pat)
{
	char *p, *q;

	p = overwrite(val);
	while (1) {
		if (p == pat)
			break;
		q = strchr(p, sep[0]);
		if (q == NULL)
			break;
		*q = '\0';
		localiter(var, p, pat);
		p = q + 1;
	}
}

void
split(void)
{
	const char **v;

	v = getvars(4);
	splititer(v[0], v[1], v[2], v[3]);
	ss_put('\0');
	savestr(putvars(4));
}
