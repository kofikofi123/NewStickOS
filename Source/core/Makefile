CC := i686-elf-gcc
CFLAGS := -masm=intel -std=c99 -I../../Include/

CSOURCES := $(wildcard *.c)
ASOURCES := $(wildcard *.asm)
OBJECTS := $(CSOURCES:.c=.o)
OBJECTS := $(OBJECTS) $(ASOURCES:.asm=.o)

compile: $(OBJECTS)
	@echo "[Finished compiling core files]"
	

.PHONY: all
all: $(OBJECTS)

%.o: %.asm ; @nasm -f elf32 $< -o $@
%.o: %.c ; @$(CC) $(CFLAGS) -o $@ -c $<