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

#ifndef DEBUG

#define	DBG(...)	do {} while (0)
#define	DBGINDENT()	do {} while (0)
#define	DUMPCHAR(l, c)	do {} while (0)
#define	DUMPBUF()	do {} while (0)

#else /* !DEBUG */

#include <stdio.h>

#define	DBGINDENT()	do { \
	int d = top - f; \
	while (d-- > 0) fputc('\t', stderr); \
} while (0)

#define	DBG(...)	do { \
	DBGINDENT(); \
	fprintf(stderr, __VA_ARGS__); \
} while (0)

#define	VC(c)		((c) == 0 ? '0' : (c) == 10 ? 'n' : '?')

#define	DBGCHAR(l, c, n)	do { \
	char xa, xb; \
	if ((c) < 0x20) { \
		xa = '\\'; xb = VC(c); \
	} else { \
		xa = ' '; xb = (c); \
	} \
	fprintf(stderr, "%c%c%c%c%s", (l), xa, xb, (l) + 2, (n)); \
} while (0)

#define	DUMPCHAR(l, c)	do { \
	DBGINDENT(); \
	DBGCHAR(l, c, "\n"); \
} while (0)

#define	DUMPBUF(p, q)	do { \
	char *str; \
	fprintf(stderr, "===|"); \
	for (str = (p); str != (q); str++) { \
		char c = *str; \
		DBGCHAR('[', c, ""); \
	} \
	fprintf(stderr, "|===\n"); \
} while (0)

#endif /* !DEBUG */
