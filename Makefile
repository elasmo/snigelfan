# Makefile for snigelfan
#
# -DVERBOSE to compile in verbose output in snigelfan
# -DCOLOR to compile in support for colors
# -ggdb3 for debugging purposes

make:
	gcc -o snigelfan snigelfan.c -lncurses -Wall -DCOLOR
clean:
	rm -f snigelfan
	rm -f *.core
install:
	cp snigelfan /usr/local/bin/
