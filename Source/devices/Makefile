CC := i686-elf-gcc
CFLAGS := -Wall -std=c99 -I../../Include/

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

compile: $(OBJECTS)
	@echo "[Finished compiling device files]"
	

.PHONY: all
all: $(OBJECTS)

%.o: %.c ; @$(CC) $(CFLAGS) -o $@ -c $<
