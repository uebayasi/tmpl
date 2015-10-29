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

%{
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void just(yyscan_t);
static void macro_start(void);
static void define_delim(void);
static int local_delim(void);
static int local_start(void);
static void template_delim(void);
static int macro_end(void *);
static void scan_error(const char *, ...);

enum op {
	MACRO = 1,
	DEFINE,
	LOCAL,
	TMPLVAR,
	TMPLVAL,
};
%}

tilda	[~]
open	[{]
close	[}]
equal	[=]
bar	[|]
at	[@]

%state	TILDA

%option	reentrant

%%

<INITIAL>{tilda} {
	BEGIN(TILDA);
}
<INITIAL>. {
	just(yyscanner);
}
<INITIAL>[\r\n] {
	just(yyscanner);
}

<TILDA>{open} {
	BEGIN(0);
	macro_start();
}
<TILDA>{equal} {
	BEGIN(0);
	define_delim();
}
<TILDA>{bar} {
	BEGIN(0);
	if (local_delim())
		local_start();
	else
		; /* continue */
}
<TILDA>{at} {
	BEGIN(0);
	template_delim();
}
<TILDA>{close} {
	BEGIN(0);
	if (macro_end(yyscanner))
		return 0; /* local_end() */
	else
		; /* continue */
}
<TILDA>{tilda} {
	BEGIN(0);
	just(yyscanner);
}
<TILDA>[.\r\n] {
	/* discard */
	BEGIN(0);
}

%%

#include "macro.h"

static void scan_write(char);
static void scan_read(void *, const char *);
static void *scan_suspend(void);
static void scan_resume(void *);
static void scan_scan(void);
static void scan_error(const char *, ...);

yyscan_t main_scan;
int main_scan_depth;

int
main(int argc, char *argv[])
{
	static struct macro_ops ops = {
		.write = &scan_write,
		.read = &scan_read,
		.suspend = &scan_suspend,
		.resume = &scan_resume,
		.scan = &scan_scan,
		.error = &scan_error,
	};

	initmacro(&ops);
	yylex_init(&main_scan);
	yylex(main_scan);
	yylex_destroy(main_scan);
	finimacro();
	return 0;
}

static void
just(yyscan_t scan)
{
	save(yyget_text(scan)[0]);
}

static void
macro_start(void)
{
	push(MACRO);
}

static void
define_delim(void)
{
	switch (delim()) {
	case MACRO:
	case DEFINE:
		new();
		push(DEFINE);
		break;
	default:
		scan_error("~= outside macro define!!!");
	}
}

static int
local_delim(void)
{
	switch (delim()) {
	case MACRO:
		push(LOCAL);
		break;
	case LOCAL:
		return 1;
		break;
	default:
		scan_error("~| outside macro define!!!");
	}
	return 0;
}

static int
local_start(void)
{
	local();
}

static void
template_delim(void)
{
	switch (delim()) {
	case MACRO:
		new();
		push(TMPLVAR);
		break;
	case TMPLVAR:
		new();
		push(TMPLVAL);
		break;
	case DEFINE:
	default:
		scan_error("~@ outside macro template!!!");
	}
}

static int
macro_end(void *scan)
{
	switch (delim()) {
	case MACRO:
		new();
		expand();
		break;
	case DEFINE:
		new();
		define(MACRO);
		break;
	case TMPLVAL:
		template();
		break;
	default:
		if (main_scan_depth != 0) {
			return 1; /* local */
		} else
			scan_error("out-of-context macro end!!!");
		break;
	}
	return 0;
}

static void
scan_write(char c)
{
	fputc(c, stdout);
}

static void
scan_read(void *state, const char *string)
{
	yyscan_t scan;
	YY_BUFFER_STATE buf;

	yylex_init(&scan);
	buf = yy_scan_string(string, scan);
	yylex(scan);
	yy_delete_buffer(buf, scan);
	yylex_destroy(scan);
}

static void
scan_scan(void)
{
	main_scan_depth++;
	yylex(main_scan);
	main_scan_depth--;
}

static void *
scan_suspend(void)
{
	return NULL;
}

static void
scan_resume(void *state)
{
}

static void
scan_error(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	exit(1);
}
