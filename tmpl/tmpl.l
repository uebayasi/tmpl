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
static void local_start(void);
static void template_delim(void);
static void split_delim(void);
static int macro_end(void *);

enum cmd {
	MACRO = 1,
	DEFINE,
	LOCAL,
	TMPLVAR,
	TMPLVAL,
	SPLITSEP,
	SPLITVAL,
	SPLITPAT,
};
%}

tilda	[~]
open	[{]
close	[}]
equal	[=]
bar	[|]
at	[@]
slash	[/]

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
	yytext[0] = '\0';
	just(yyscanner);
	define_delim();
}
<TILDA>{bar} {
	BEGIN(0);
	yytext[0] = '\0';
	just(yyscanner);
	if (local_delim())
		local_start();
	else
		; /* continue */
}
<TILDA>{at} {
	BEGIN(0);
	yytext[0] = '\0';
	just(yyscanner);
	template_delim();
}
<TILDA>{slash} {
	BEGIN(0);
	yytext[0] = '\0';
	just(yyscanner);
	split_delim();
}
<TILDA>{close} {
	BEGIN(0);
	yytext[0] = '\0';
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
static void scan_scan(const char *);
static void scan_error(const char *, ...) __attribute__((__noreturn__));
static int scan_depth(void);

yyscan_t main_scan;
static int main_scan_depth;

int
main(int argc, char *argv[])
{
	static struct macro_ops ops = {
		.write = &scan_write,
		.scan = &scan_scan,
		.error = &scan_error,
	};

	(void)yyunput;
	(void)input;

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
		/* NOTREACHED */
	default:
		scan_error("~| outside macro define!!!");
	}
	return 0;
}

static void
local_start(void)
{
	local();
}

static void
template_delim(void)
{
	switch (delim()) {
	case MACRO:
		push(TMPLVAR);
		break;
	case TMPLVAR:
		push(TMPLVAL);
		break;
	default:
		scan_error("~@ outside macro template!!!");
	}
}

static void
split_delim(void)
{
	switch (delim()) {
	case MACRO:
		push(SPLITSEP);
		break;
	case SPLITSEP:
		push(SPLITVAL);
		break;
	case SPLITVAL:
		push(SPLITPAT);
		break;
	default:
		scan_error("~/ outside macro define!!!");
	}
}

static int
macro_end(void *scan)
{
	if (scan_depth() == 0)
		just(scan); /* \0 */
	else
		; /* nothing */
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
	case SPLITPAT:
		split();
		break;
	default:
		if (scan_depth() == 0)
			scan_error("out-of-context macro end!!!");
		else
			return 1; /* local */
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
scan_string(const char *string)
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
scan_rescan(void)
{
	main_scan_depth++;
	yylex(main_scan);
	main_scan_depth--;
}

static void
scan_scan(const char *string)
{
	if (string != NULL)
		scan_string(string);
	else
		scan_rescan();
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

static int
scan_depth(void)
{
	return main_scan_depth;
}
