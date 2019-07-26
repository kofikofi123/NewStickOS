#include "NSO8259A.h"
#include "NSOCoreUtils.h"

//most of the code here based directly from osdev 8259 pge


u8 kernel_init8259A(){
	u8 s1 = kernel_in8(0x21), s2 = kernel_in8(0xA1);
	kernel_out8(0x20, 0b00010101);
	kernel_out8(0xA0, 0b00010101);
	
	kernel_out8(0x21, 0x20);
	kernel_out8(0xA1, 0x28);

	kernel_out8(0x21, 4);
	kernel_out8(0xA1, 2);

	kernel_out8(0x21, 0b00001101);
	kernel_out8(0xA1, 0b00001101);

	kernel_out8(0x21, s1);
	kernel_out8(0xA1, s2);

	return 1;
}

void kernel_8259EOI(u8 irq){
	if (irq > 7)
		kernel_out8(0xA1, 0x20);
	kernel_out8(0x21, 0x20);
}

void kernel_mask8259INTS(u8 masterMask, u8 slaveMask){
	kernel_out8(0x21, masterMask);
	kernel_out8(0xA1, slaveMask);
}