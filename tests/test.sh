#!/bin/sh

################################################################################

tmpl=$1

################################################################################

repeat() {
	local times=$1
	local func=$2
	local i=0	

	while [ $i -lt $times ]; do
		eval $func
		i=$(( $i + 1 ))
	done
}

cutter() {
	printf '%s' '--------8<'
}

separator() {
	repeat 8 cutter
	printf '\n'
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
		show INPUT $tmpl
		show OUTPUT $tmpl.out
		show ERR $tmpl.err
		show EXP $tmpl.exp
	} >$tmpl.log
}

################################################################################

../tmpl/tmpl.exe <$tmpl >$tmpl.out 2>$tmpl.err
res=$?

if [ -r $tmpl.exp ]; then
	if [ $res -eq 0 ] && cmp $tmpl.out $tmpl.exp >/dev/null 2>&1; then
		printf '%s => OK!!!\n' $tmpl
	else
		printf '%s => NG!!!\n' $tmpl
		dump $tmpl
	fi
else
	dump $tmpl
fi

################################################################################
