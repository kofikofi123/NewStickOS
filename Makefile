CC := i686-nos-gcc
LD := i686-nos-ld
NASM := nasm
GII := genisoimage
CFLAGS := -Wall -masm=intel -ffreestanding -m32 --std=c99 -IInclude -IInclude/acpica
BUILD := ./Build
BOCHS := bochs
GIT := git

DIRS := ./Source/core ./Source/devices ./Source/utils \
		./Source/acpica ./Source/acpica/debugger ./Source/acpica/disassembler  \
		./Source/acpica/dispatcher ./Source/acpica/events ./Source/acpica/executer  \
		./Source/acpica/hardware ./Source/acpica/namespace ./Source/acpica/parser  \
		./Source/acpica/resources ./Source/acpica/tables ./Source/acpica/utilities

BOOTDIR := ./Source/boot

VPATH = ./Source/boot:./Source/core:./Source/devices:./Source/utils: \
		./Source/acpica:./Source/acpica/debugger:./Source/acpica/disassembler: \
		./Source/acpica/dispatcher:./Source/acpica/events./Source/acpica/executer: \
		./Source/acpica/hardware:./Source/acpica/namespace:./Source/acpica/parser: \
		./Source/acpica/resources:./Source/acpica/tables:./Source/acpica/utilities \

CSOURCES :=  $(wildcard $(addsuffix /*.c,$(DIRS)))
ASOURCES := $(wildcard $(addsuffix /*.asm,$(DIRS)))
BSOURCES := $(wildcard $(addsuffix /*.asm,$(BOOTDIR)))
OBJECTS := $(CSOURCES:.c=.o)
OBJECTS += $(ASOURCES:.asm=.o)
EXTRA_OBJECTS := $(OBJECTS) $(BSOURCES:.asm=.bin)

build: all
	@$(LD) -T KernelLinkerScript.ld $(OBJECTS)
	@$(sudo) cp $(BOOTDIR)/*.bin ./DiskContent
	@$(GII) -V 'MYOS' -no-emul-boot -boot-load-size 4 -b Bootloader.bin -o StickOS.iso DiskContent

build-debug: all
	@$(LD) -T KernelLinkerScriptDebug.ld $(OBJECTS)

git-save: $(CSOURCES) $(ASOURCES) $(BSOURCES)
	@$(GIT) add .
	@$(GIT) commit -m "oke"

bochs-run: build clean
	@$(BOCHS) -f bochs_config.txt -q

.PHONY: all clean

all: $(EXTRA_OBJECTS)
clean: 
	@rm -rf $(EXTRA_OBJECTS)

%.o: %.c ; @$(CC) $(CFLAGS) -o $@ -c $<
%.o: %.asm ; @$(NASM) -f elf32 $< -o $@
%.bin: %.asm ; @$(NASM) -f bin $< -o $@