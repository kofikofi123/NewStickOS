SOURCES := $(wildcard *.asm)
OBJECTS := $(SOURCES:.asm=.bin)


compile: $(OBJECTS)
	@echo "[Finished compiling boot sources]"

.PHONY: all
all: $(OBJECTS)

%.bin: %.asm ; @nasm -f bin $< -o $@
