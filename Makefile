CC ?= gcc
WINDRES ?= windres
TARGET = tabedit.exe
SRC = main.c
RES = resource.o

CFLAGS = -std=c11 -O2 -Wall -Wextra -municode -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601 -finput-charset=UTF-8
RCFLAGS = --codepage=65001
LDFLAGS = -mwindows -municode -lcomctl32 -lcomdlg32 -lshell32 -luser32 -lgdi32 -luxtheme -ladvapi32 -lkernel32

all: $(TARGET)

$(RES): resource.rc app.manifest
	$(WINDRES) $(RCFLAGS) -O coff -i resource.rc -o $(RES)

$(TARGET): $(SRC) $(RES)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(RES) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(RES)
