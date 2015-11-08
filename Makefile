# tmpl

# common

CFLAGS?=	-Wall -Werror -g -O0
CPPFLAGS?=	-DDEBUG
CC?=		cc ${CPPFLAGS} ${CFLAGS}
LEX?=		flex

all: ./srcs/tmpl/tmpl.exe test

# tmpl

tmpl_exe_OBJS= \
./srcs/tmpl/tmpl.o \
./srcs/tmpl/macro.o \
./srcs/tmpl/ss.o \
./srcs/tmpl/sym.o \

./srcs/tmpl/tmpl.exe: ${tmpl_exe_OBJS} ./srcs/symtab/symtab.o
	${CC} -o ./srcs/tmpl/tmpl.exe ${tmpl_exe_OBJS} ./srcs/symtab/symtab.o -ll

./srcs/tmpl/tmpl.c: ./srcs/tmpl/tmpl.l
	${LEX} -o./srcs/tmpl/tmpl.c ./srcs/tmpl/tmpl.l

# tests

tmpl_tests= \
test0 \
test1 \
test2 \
test3 \
test4 \
test5 \
test6 \
test7 \
test8 \
test9 \
test10 \
test11 \
test12 \
test13 \
test14 \
test15 \
test16 \
test17 \
test18 \

.for t in ${tmpl_tests}
test: ./tests/${t}.tmpl.out
${t}: ./tests/${t}.tmpl.out
.if exists(./tests/${t}.tmpl)
./tests/${t}.tmpl.out: ./tests/${t}.tmpl
.else
./tests/${t}.tmpl.out: ./tests/${t}.sh
.endif
./tests/${t}.tmpl.out: ./tests/test.sh ./srcs/tmpl/tmpl.exe
	@cd ./tests && ./test.sh ${t}
.endfor

# symtab

symtab_o_OBJS= \
./srcs/symtab/hashtab.o \
./srcs/symtab/intern.o \

./srcs/symtab/symtab.o: ${symtab_o_OBJS}
	ld -r -o ./srcs/symtab/symtab.o ${symtab_o_OBJS}

# clean

clean:
	rm -f ./srcs/tmpl/tmpl.c */*.o */*.exe ./tests/*.out ./tests/*.err

# rules

.for o in ${tmpl_exe_OBJS} ${symtab_o_OBJS}
${o}: ${o:R}.c
	${CC} -o ${o} -c ${o:R}.c
.endfor
