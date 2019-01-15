CC := i686-elf-gcc
CFLAGS := -Wall -std=c99 -masm=intel -I../../Include/

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

compile: $(OBJECTS)
	@echo "[Finished compiling util files]"
	

.PHONY: all
all: $(OBJECTS)

%.o: %.c ; @$(CC) $(CFLAGS) -o $@ -c $<
