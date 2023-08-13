# LIBDIR = /usr/local/lib
# CFLAGS = -Wall -O2 -ansi -pedantic -Wno-unused-result
# CC = gcc
###########################################################################

compiler: compiler.o stack.o
	gcc stack.o compiler.o -o compiler  

stack.o: stack.c stack.h item.h
	# gcc -Wall -O2 -ansi -pedantic -Wno-unused-result -c stack.c
	gcc -O2 -c stack.c

compiler.o: compiler.c stack.h item.h
	# gcc -Wall -O2 -ansi -pedantic -Wno-unused-result -c compiler.c
	gcc -O2 -c compiler.c

