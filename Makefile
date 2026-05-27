CC=gcc
CFLAGS=-std=c11 -Wall -Wextra
LDFLAGS=-lcomctl32 -lshell32

all: text-editor.exe

text-editor.exe: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f text-editor.exe
