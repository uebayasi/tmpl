# tmpl

CC=	cc -Wall -g -O0

all: ./tmpl/tmpl.exe test

# tmpl

tmpl_exe_OBJS= \
./tmpl/tmpl.o \
./tmpl/macro.o \
./tmpl/ss.o \
./tmpl/sym.o \
./symtab/symtab.o \

./tmpl/tmpl.exe: ${tmpl_exe_OBJS}
	${CC} -o ./tmpl/tmpl.exe ${tmpl_exe_OBJS} -ll

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

.SUFFIXES: .c .o
	${CC} -o $@ -c $<
.SUFFIXES: .l .c
	flex -o$@ $<
