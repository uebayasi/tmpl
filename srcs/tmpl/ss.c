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
#include "ss.h"
#include "dbg.h"

static char *head, *tail, *cur;
static char **sss, **sse, **ss;

void
ss_init(char *p, char *q, char **s, char **e)
{
	head = cur = p;
	tail = q;
	sss = s;
	sse = e;
	ss = sss;
}

void
ss_fini(void)
{
	assert(cur == head);
	assert(ss == sss);
}

int
ss_put(char c)
{
	if (cur == tail)
		return 1;
	else {
		*cur++ = c;
		return 0;
	}
}

int
ss_push(void)
{
	if (ss == sse)
		return 1;
	else {
		*ss++ = cur;
		return 0;
	}
}

char *
ss_pop(void)
{
	if (ss == sss)
		return (void *)-1;
	else {
		cur = *--ss;
		return cur;
	}
}

char **
ss_getvars(int n)
{
	char **v;

	v = ss - n;
	if (ss_push())
		return (void *)-1;
	else
		return v;
}

char *
ss_putvars(int n)
{
	char *s;

	if ((s = ss_pop()) == (void *)-1)
		return (void *)-1;
	else {
		ss -= n;
		cur = *ss;
		return s;
	}
}
