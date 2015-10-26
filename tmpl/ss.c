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
	ss_push();
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

void
ss_keep(char *s)
{
	cur = s;
	ss_push();
}

void
ss_unkeep(char *s)
{
	cur = s;
}

#ifdef DEBUG
void
ss_dump(void)
{
	char **xss;

	DUMPBUF(head, cur);
	for (xss = sse; ss < xss; xss--) {
		fputc('$', stderr);
	}
	fputc('\n', stderr);
}
#endif
