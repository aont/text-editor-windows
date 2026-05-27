CC=gcc
CFLAGS=-std=c11 -Wall -Wextra
LDFLAGS=-lcomctl32 -lcomdlg32 -luxtheme -ldwmapi -lgdi32

all: editor.exe

editor.exe: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f editor.exe
