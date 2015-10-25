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
ss_alloc(char *p, char *q, char **xsss, char **xsse)
{
	head = cur = p;
	tail = q;
	sss = xsss;
	sse = ss = xsse;
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
ss_push(void)
{
	*--ss = cur;
}

char *
ss_pop(void)
{
	cur = *ss++;
	return cur;
}

char *
ss_keep(char *s)
{
	char *k;

	k = cur;
	cur = s;
	ss_push();
	return k;
}

void
ss_unkeep(char *k)
{
	cur = k;
}

void
ss_flush(void (*f)(const char *))
{
#ifdef DEBUG
	fprintf(stderr, "!!!!!!!!\n");
#endif
	if (ss != sse) {
		char *s = ss_pop();
		(*f)(s);
	}
}

#ifdef DEBUG
void
ss_dump(void)
{
	char **xss;

	DUMPBUF(head, cur);
	for (xss = ss; xss < sse; xss++)
		fprintf(stderr, "||||%s||||\n", *xss);
}
#endif
