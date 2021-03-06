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

#ifndef _MACRO_H_
#define _MACRO_H_

#include "macro_config.h"

struct frame {
	const char *sym;
	int cmd;
};

struct macro_ops {
	void (*write)(char);
	void (*scan)(const char *);
	void (*error)(const char *, ...) __attribute__((__noreturn__));
};

void initmacro(const struct macro_ops *);
void finimacro(void);
void push(int);
void save(char c);
int delim(void);
void new(void);
void define(int);
void expand(void);
void local(void);
void template(void);
void split(void);
void ifdef(void);

#endif /* _MACRO_H_ */
