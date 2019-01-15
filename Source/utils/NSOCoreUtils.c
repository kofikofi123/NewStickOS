#include "NSOCoreUtils.h"

/*To comment*/

void kernel_outb(u16 port, u8 data){
	__asm__("out dx, al"
			:
			: "d" (port), "a" (data));
}

void kernel_outw(u16 port, u16 data){
	__asm__("out dx, ax"
			:
			: "d" (port), "a" (data));
}

void kernel_outdw(u16 port, u32 data){
	__asm__("out dx, eax"
			:
			: "d" (port), "a" (data));
}

u8 kernel_inb(u16 port){
	u8 data = 0;
	__asm__("in al, dx"
			: "=a" (data)
			: "d" (port));
	return data;
}

u16 kernel_inw(u16 port){
	u16 data = 0;
	__asm__("in ax, dx"
			: "=a" (data)
			: "d" (port));
	return data;
}

u32 kernel_indw(u16 port){
	u32 data = 0;
	__asm__("in eax, dx"
			: "=a" (data)
			: "d" (port));
	return data;
}