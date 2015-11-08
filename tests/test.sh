#!/bin/sh

################################################################################

test=$1
tmpl=${test}.tmpl
scrpt=${test}.sh
exe=../srcs/tmpl/tmpl.exe

################################################################################

separator() {
	printf '--------8<--------8<--------8<--------8<--------8<--------8<--------8<--------8<\n'
}

dump_normal() {
	cat
}

dump_hex() {
	od -c
}

show_normal() {
	local name=$1
	local file=$2

	printf '%s:\n' "$name"
	separator
	dump_normal <$file
	separator
	printf '\n'
}

show_hex() {
	local name=$1
	local file=$2

	printf '%s (hexdump):\n' "$name"
	separator
	dump_hex <$file
	separator
	printf '\n'
}

show() {
	local name=$1
	local file=$2

	show_normal "$name" "$file"
	show_hex "$name" "$file"
}

dump() {
	local tmpl=$1

	{
		if [ -e $tmpl ]; then
			show INPUT $tmpl
		else
			:
		fi
		show OUTPUT $tmpl.out
		show ERR $tmpl.err
		show EXP $tmpl.exp
	} >$tmpl.log
}

################################################################################

if [ -e $tmpl ]; then
	$exe <$tmpl
else
	sh $scrpt | $exe
fi >$tmpl.out 2>$tmpl.err
res=$?

if [ -r $tmpl.exp ]; then
	if [ $res -eq 0 ] && cmp $tmpl.out $tmpl.exp >/dev/null 2>&1; then
		printf '%s => OK!!!\n' $tmpl
	else
		printf '%s => NG!!!\n' $tmpl
	fi
fi
dump $tmpl

################################################################################
