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

static struct strbuf buf;
static struct strbuf cur;

void
ss_alloc(const char *head, const char *tail)
{
	cur.head = cur.tail = buf.head = head;
	buf.tail = tail;
}

const char *
ss_pop(struct strbuf *b)
{
	cur.tail = b->head;
	return b->head;
}

int
ss_put(struct strbuf *b, char c)
{
	if (cur.head == buf.tail)
		return 1;
	else {
		*cur.tail++ = c;
		return 0;
	}
}

void
ss_push(struct strbuf *b)
{
	b->head = cur.tail;
}

void
ss_keep(struct strbuf *b, const char *s)
{
	while (*s++ != '\0')
		continue;
	cur.tail = s;
	ss_push(b);
}
