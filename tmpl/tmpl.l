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
#include <stdio.h>

static void just(void);
static void macro_start(void);
static void define_delim(void);
static void template_delim(void);
static void macro_close(void);
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
	macro_close();
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

static void macro_proc(const char *);
static void *macro_suspend(void);
static void macro_resume(void *);

int
main(int argc, char *argv[])
{
	struct macro_scan_ops ops = {
		.proc = &macro_proc,
		.suspend = &macro_suspend,
		.resume = &macro_resume,
	};
	initmacro(&ops);
	yylex();
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
	switch (end()) {
	case MACRO:
	case DEFINE:
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
	switch (end()) {
	case MACRO:
		push(TMPLVAR);
		break;
	case TMPLVAR:
		pushstr(TMPLVAL);
		break;
	case DEFINE:
	default:
		yy_fatal_error("~@ outside macro template!!!");
	}
}

static void
macro_close(void)
{
	if (!ispushed()) {
		yy_fatal_error("macro close without macro start!!!");
	} else {
		macro_end();
	}
}

static void
macro_end(void)
{
	const char *var;

	switch (end()) {
	case MACRO:
		expand();
		break;
	case DEFINE:
		var = NULL;
		while (define(var, &var) != MACRO)
			continue;
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
macro_proc(const char *string)
{
	BEGIN(0);
	yy_scan_string(string);
	yylex();
}

static void *
macro_suspend(void)
{
	return YY_CURRENT_BUFFER;
}

static void
macro_resume(void *state)
{
	yy_switch_to_buffer(state);
}
