# tmpl

CC=	cc -Wall -g -O0

all: tmpl.exe test

tmpl.exe: tmpl.o macro.o sym.o symtab.o
	${CC} -o tmpl.exe tmpl.o macro.o sym.o symtab.o -ll
tmpl.o: tmpl.c
	${CC} -o tmpl.o -c tmpl.c
tmpl.c: tmpl.l
	flex -otmpl.c tmpl.l

sym.o: sym.c
	${CC} -o sym.o -c sym.c
macro.o: macro.c
	${CC} -o macro.o -c macro.c

test:
	@./test.sh test0.tmpl
	@./test.sh test1.tmpl
	@./test.sh test2.tmpl
	@./test.sh test3.tmpl
	@./test.sh test4.tmpl
	@./test.sh test5.tmpl
	@./test.sh test6.tmpl
	@./test.sh test7.tmpl
	@./test.sh test8.tmpl
	@./test.sh test9.tmpl
	@./test.sh test10.tmpl
	@./test.sh test11.tmpl
	@./test.sh test12.tmpl

# symtab

symtab.o: hashtab.o intern.o
	ld -r -o symtab.o hashtab.o intern.o
hashtab.o: hashtab.c
	${CC} -o hashtab.o -c hashtab.c
intern.o: intern.c
	${CC} -o intern.o -c intern.c

# clean

clean:
	rm -f tmpl.c *.o *.exe *.out *.err
