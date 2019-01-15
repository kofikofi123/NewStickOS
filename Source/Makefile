LD := i686-elf-ld
OBJCOPY := i686-elf-objcopy

compile_source:
	@make -C boot
	@mv -f ./boot/*.bin ../DiskContent
	@make -C core 
	@mv -f ./core/*.o ../Build
	@make -C devices
	@#mv -f ./devices/*.o ../Build
	@make -C utils
	@mv -f ./utils/*.o ../Build

	@$(LD) -T KernelLinkerScript.ld ../Build/*.o
	@mv Stick.bin ../DiskContent
	@echo "[Finished compiling sources]"