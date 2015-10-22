# tmpl

CC=	cc -Wall -g -O0

all: ./tmpl/tmpl.exe test

./tmpl/tmpl.exe: ./tmpl/tmpl.o ./tmpl/macro.o ./tmpl/sym.o ./symtab/symtab.o
	${CC} -o ./tmpl/tmpl.exe ./tmpl/tmpl.o ./tmpl/macro.o ./tmpl/sym.o ./symtab/symtab.o -ll
./tmpl/tmpl.o: ./tmpl/tmpl.c
	${CC} -o ./tmpl/tmpl.o -c ./tmpl/tmpl.c
./tmpl/tmpl.c: ./tmpl/tmpl.l
	flex -o./tmpl/tmpl.c ./tmpl/tmpl.l

./tmpl/sym.o: ./tmpl/sym.c
	${CC} -o ./tmpl/sym.o -c ./tmpl/sym.c
./tmpl/macro.o: ./tmpl/macro.c
	${CC} -o ./tmpl/macro.o -c ./tmpl/macro.c

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

./symtab/symtab.o: ./symtab/hashtab.o ./symtab/intern.o
	ld -r -o ./symtab/symtab.o ./symtab/hashtab.o ./symtab/intern.o
./symtab/hashtab.o: ./symtab/hashtab.c
	${CC} -o ./symtab/hashtab.o -c ./symtab/hashtab.c
./symtab/intern.o: ./symtab/intern.c
	${CC} -o ./symtab/intern.o -c ./symtab/intern.c

# clean

clean:
	rm -f ./tmpl/tmpl.c */*.o */*.exe ./tests/*.out ./tests/*.err
