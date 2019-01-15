LD := i686-elf-ld
OBJCOPY := i686-elf-objcopy

compile_source:
	@make -C boot
	@mv -f ./boot/*.bin ../DiskContent
	@make -C core 
	@mv -f ./core/*.o ../Build
	@$(LD) -T KernelLinkerScript.ld ../Build/*.o
	@mv Stick.bin ../DiskContent
	@echo "[Finished compiling sources]"