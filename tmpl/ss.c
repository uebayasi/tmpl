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
#include "macro.h" /* XXX STRBUF_MAX */

static struct {
	struct strbuf all;
	struct strbuf cur;
} ss;
#define	sb	(&ss.cur)
#define	ab	(&ss.all)

void
ss_alloc(void)
{
	static char chars[STRBUF_MAX];

	sb->head = &chars[0];
	sb->tail = &chars[0];
	ab->head = &chars[0];
	ab->tail = &chars[STRBUF_MAX];
}

const char *
ss_get(struct strbuf *b)
{
	return b->head;
}

void
ss_put(struct strbuf *b, char c)
{
	*sb->tail++ = c;
	b->tail++;
}

void
ss_push(struct strbuf *b)
{
	b->head = b->tail = sb->tail;
}

void
ss_pop(struct strbuf *b)
{
	sb->tail = b->tail = b->head;
}

void
ss_dup(struct strbuf *b, const char *s)
{
	sb->tail = s;
	ss_push(b);
}

int
ss_is_limit(void)
{
	return (sb->head == ab->tail);
}
