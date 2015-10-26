# tmpl

# common

CFLAGS=		-Wall -g -O0
#CPPFLAGS=	-DDEBUG
CC=		cc ${CPPFLAGS} ${CFLAGS}

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
	flex -o./tmpl/tmpl.c ./tmpl/tmpl.l

test:
	@cd ./tests && ./test.sh test0.tmpl
	@cd ./tests && ./test.sh test1.tmpl
	@cd ./tests && ./test.sh test2.tmpl
	@cd ./tests && ./test.sh test3.tmpl
	@cd ./tests && ./test.sh test4.tmpl
	@cd ./tests && ./test.sh test5.tmpl
	@cd ./tests && ./test.sh test6.tmpl
	@cd ./tests && ./test.sh test7.tmpl
	@cd ./tests && ./test.sh test8.tmpl
	@cd ./tests && ./test.sh test9.tmpl
	@cd ./tests && ./test.sh test10.tmpl
	@cd ./tests && ./test.sh test11.tmpl
	@cd ./tests && ./test.sh test12.tmpl

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
