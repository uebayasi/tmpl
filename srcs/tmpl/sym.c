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

#include "../symtab/symtab.h"
#include "sym.h"

static struct hashtab *symtab = NULL;

void
initsym(void)
{
	initintern();
	symtab = ht_new();
}

const char *
newsym(const char *s)
{
	return intern(s);
}

const char *
getsym(const char *s)
{
	return ht_lookup2(symtab, s, NULL);
}

void
setsym(const char *s, const char *v)
{
	/* always overwrite (replace==1) */
	ht_insrep2(symtab, s, NULL, v, 1);
}

void
delsym(const char *s)
{
	ht_remove(symtab, s);
}
