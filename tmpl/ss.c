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
#include "ss.h"

#define ERR(...) do { \
	fprintf(stderr, __VA_ARGS__); \
	exit(1); \
} while (0)

static struct {
	struct strbuf all;
	struct strbuf cur;
} ss;

void
ss_alloc(const char *head, const char *tail)
{
	ss.cur.head = ss.cur.tail = ss.all.head = head;
	ss.all.tail = tail;
}

const char *
ss_pop(struct strbuf *b)
{
	ss.cur.tail = b->tail = b->head;
	return b->head;
}

void
ss_put(struct strbuf *b, char c)
{
	if (ss.cur.head == ss.all.tail)
		ERR("buffer overflow!!!\n");
	*ss.cur.tail++ = c;
	b->tail++;
}

void
ss_push(struct strbuf *b)
{
	b->head = b->tail = ss.cur.tail;
}

void
ss_dup(struct strbuf *b, const char *s)
{
	while (*s++ != '\0')
		continue;
	ss.cur.tail = s;
	ss_push(b);
}
