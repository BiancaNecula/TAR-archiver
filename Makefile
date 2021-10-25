

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99


build: archiver.c
	$(CC) $(CFLAGS) archiver.c -o archiver -lm

pack:
	zip -FSr 315CA_BiancaNecula_Tema3.zip README Makefile *.c *.h

clean:
	rm -f archiver
	rm -f *.tar

