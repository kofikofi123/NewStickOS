

compileSources:
	@make -C Source --no-print-directory
	@genisoimage -V 'MYOS' -no-emul-boot -boot-load-size 4 -b Bootloader.bin -o StickOS.iso DiskContent
	@echo "[Finished]"

.PHONY: clean
clean:
	@rm -f DiskContent/*.bin
	@rm -f Build/*.o
	@rm -f StickOS.iso