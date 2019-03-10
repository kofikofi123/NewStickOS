#include "NSO8259A.h"
#include "NSOCoreUtils.h"

u8 kernel_init8259A(){
	return 0;
}

void kernel_deinit8259A(){
	kernel_outb(0xA1, 0xFF);
	kernel_outb(0x21, 0xFF);
}