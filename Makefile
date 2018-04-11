SRC := Source
INCL := Include
DCON := DiskContent
BUILD := Build
FLAGS := -m32 -masm=intel -Wall -std=c99 -I$(INCL)/
CC := i686-elf-gcc 
LD := i686-elf-ld

comp: 
	@$(CC) $(FLAGS) -c $(SRC)/*.c
	
	for f in $(SRC)/*.asm; \
	do \
		nasm -f elf32 $$f; \
	done; \
	
	@mv -t $(BUILD)/ *.o
	@mv -t $(BUILD)/ $(SRC)/*.o
	
link: comp
	@$(LD) -T KernelLinkerScript.ld $(BUILD)/*.o
	@mv *.bin $(DCON)
disk: link 
	@nasm -f bin Bootloader.asm -o $(DCON)/Bootloader.bin
	@nasm -f bin Setup.asm -o $(DCON)/Setup.bin
	@genisoimage -V 'MYOS' -no-emul-boot -boot-load-size 4 -b Bootloader.bin -o StickOS.iso $(DCON)
clean:
	@find ./ -name *.iso | xargs rm
	@find $(BUILD) -name *.o | xargs rm 
	@find $(DCON) -name *.bin | xargs rm 
	@find ./ -name *.o | xargs rm