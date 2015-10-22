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
#include "tmpl.h"

static void just(void);
static void macro_start(void);
static void define_start(void);
static void repeat_delim(void);
static void macro_close(void);
static void macro_end(void);

enum op {
	MACRO = 1,
	DEFINE,
	REPEAT,
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
	define_start();
	BEGIN(0);
}
<TILDA>{at} {
	repeat_delim();
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
	struct macro_ops ops = {
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
define_start(void)
{
	int op;

	op = end();
	(void)op; /* op == MACRO */
	push(DEFINE);
}

static void
repeat_delim(void)
{
	int op;

	op = end();
	switch (op) {
	case MACRO:
	case REPEAT:
		push(REPEAT);
		break;
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
	int op;

	op = end();
	switch (op) {
	case MACRO:
		op = expand();
		break;
	case DEFINE:
		op = define(NULL);
		break;
	case REPEAT:
		op = template();
		break;
	default:
		break;
	}
}

static void
macro_proc(const char *string)
{
	yy_scan_string(string);
	yylex();
}

static void *
macro_suspend(void)
{
	BEGIN(0);
	return yy_current_buffer;
}

static void
macro_resume(void *state)
{
	yy_switch_to_buffer(state);
}
