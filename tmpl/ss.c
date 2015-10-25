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

static char *head, *tail, *cur;
static char **sss, **sse, **ss;

void
ss_alloc(char *p, char *q, char **xsss, char **xsse)
{
	head = cur = p;
	tail = q;
	sss = ss = xsss;
	sse = xsse;
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

void
ss_push(char **rs)
{
	*ss++ = cur;
}

char *
ss_pop(char **rs)
{
	cur = *--ss;
	return *ss;
}

void
ss_keep(char **rs, const char *s)
{
	while (*s++ != '\0')
		continue;
	cur = s;
	ss_push(rs);
}
