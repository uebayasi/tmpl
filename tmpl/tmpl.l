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

static void just(void);
static void macro_start(void);
static void define_delim(void);
static void template_delim(void);
static void macro_end(void);

enum op {
	MACRO = 1,
	DEFINE,
	TMPLVAR,
	TMPLVAL,
};
%}

tilda	[~]
open	[{]
close	[}]
equal	[=]
at	[@]

%s	TILDA

%%

<INITIAL>{tilda} {
	BEGIN(TILDA);
}
<INITIAL>. {
	just();
}
<INITIAL>[\r\n] {
	just();
}

<TILDA>{open} {
	macro_start();
	BEGIN(0);
}
<TILDA>{equal} {
	define_delim();
	BEGIN(0);
}
<TILDA>{at} {
	template_delim();
	BEGIN(0);
}
<TILDA>{close} {
	macro_end();
	BEGIN(0);
}
<TILDA>{tilda} {
	just();
	BEGIN(0);
}
<TILDA>[.\r\n] {
	/* discard */
	BEGIN(0);
}

%%

#include "macro.h"

static void scan_write(char);
static void scan_read(const char *);
static void *scan_suspend(void);
static void scan_resume(void *);
static void scan_error(const char *, ...);

int
main(int argc, char *argv[])
{
	static struct macro_scan_ops ops = {
		.write = &scan_write,
		.read = &scan_read,
		.suspend = &scan_suspend,
		.resume = &scan_resume,
		.error = &scan_error,
	};
	initmacro(&ops);
	yylex();
	finimacro();
	return 0;
}

static void
just(void)
{
	save(yytext[0]);
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
	case TMPLVAR:
	case TMPLVAL:
	default:
		yy_fatal_error("~= outside macro define!!!");
	}
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
		yy_fatal_error("~@ outside macro template!!!");
	}
}

static void
macro_end(void)
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
		yy_fatal_error("out-of-context macro end!!!");
		break;
	}
}

static void
scan_write(char c)
{
	fputc(c, stdout);
}

static void
scan_read(const char *string)
{
	BEGIN(0);
	yy_scan_string(string);
	yylex();
}

static void *
scan_suspend(void)
{
	return YY_CURRENT_BUFFER;
}

static void
scan_resume(void *state)
{
	yy_switch_to_buffer(state);
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
