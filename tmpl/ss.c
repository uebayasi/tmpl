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

#include "ss.h"
#include "dbg.h"

static char *head, *tail, *cur;
static char **sss, **sse, **ss;

void
ss_alloc(char *p, char *q, char **s, char **e)
{
	head = cur = p;
	tail = q;
	sss = s;
	sse = ss = e;
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
	if (ss == sss)
		return 1;
	else {
		*--ss = cur;
		return 0;
	}
}

char *
ss_pop(void)
{
	if (ss == sse)
		return (void *)-1;
	else {
		cur = *ss++;
		return cur;
	}
}

int
ss_keep(char *s)
{
	cur = s;
	ss_push();
	while (*s++ != '\0')
		continue;
	cur = s;
	ss_push();
	return 0;
}

char *
ss_unkeep(void)
{
	char *s;

	s = ss_pop();
	(void)ss_pop();
	return s;
}
