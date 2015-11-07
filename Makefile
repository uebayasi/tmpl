# tmpl

# common

CFLAGS=		-Wall -Werror -g -O0
#CPPFLAGS=	-DDEBUG
CC=		cc ${CPPFLAGS} ${CFLAGS}
LEX=		flex

all: ./tmpl/tmpl.exe test

# tmpl

tmpl_exe_OBJS= \
./tmpl/tmpl.o \
./tmpl/macro.o \
./tmpl/ss.o \
./tmpl/sym.o \

./tmpl/tmpl.exe: ${tmpl_exe_OBJS} ./symtab/symtab.o
	${CC} -o ./tmpl/tmpl.exe ${tmpl_exe_OBJS} ./symtab/symtab.o -ll

./tmpl/tmpl.c: ./tmpl/tmpl.l
	${LEX} -o./tmpl/tmpl.c ./tmpl/tmpl.l

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

.for t in ${tmpl_tests}
test: ./tests/${t}.tmpl.out
${t}: ./tests/${t}.tmpl.out
.if exists(./tests/${t}.tmpl)
./tests/${t}.tmpl.out: ./tests/${t}.tmpl
.else
./tests/${t}.tmpl.out: ./tests/${t}.sh
.endif
./tests/${t}.tmpl.out: ./tests/test.sh ./tmpl/tmpl.exe
	@cd ./tests && ./test.sh ${t}
.endfor

# symtab

symtab_o_OBJS= \
./symtab/hashtab.o \
./symtab/intern.o \

./symtab/symtab.o: ${symtab_o_OBJS}
	ld -r -o ./symtab/symtab.o ${symtab_o_OBJS}

# clean

clean:
	rm -f ./tmpl/tmpl.c */*.o */*.exe ./tests/*.out ./tests/*.err

# rules

.for o in ${tmpl_exe_OBJS} ${symtab_o_OBJS}
${o}: ${o:R}.c
	${CC} -o ${o} -c ${o:R}.c
.endfor
