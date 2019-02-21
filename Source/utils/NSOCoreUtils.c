#include "NSOCoreUtils.h"
#include "NSOBochs.h"


static u32 _kernel_maxCPUID = 0, _kernel_maxCPUIDEX = 0;

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

inline void kernel_sti(){
	__asm__("sti");
}

inline void kernel_cli(){
	__asm__("cli");
}

inline void kernel_hlt(){
	__asm__("hlt");
}

u32 kernel_getEFLAGS(){
	u32 eflag = 0;
	__asm__("pushfd\n\tpop eax"
			: "=a" (eflag));
	return eflag;
}

void kernel_setEFLAGS(u32 eflag){
	__asm__("push eax\n\tpopfd"
			:
			: "a" (eflag));
}

void kernel_panic(const char* panicMessage){
	kernel_printfBOCHS("\nPANIC: %s\n", panicMessage);
	kernel_cli();
	while (1)
		kernel_hlt();
}

u8 kernel_isCpuidSupported(){
	u32 o = kernel_getEFLAGS(), n = 0;
	kernel_setEFLAGS(o ^ 0x200000);
	n = kernel_getEFLAGS();
	return (o >> 21) ^ (n >> 21);
}

void kernel_cpuid(u32* eax, u32* ebx, u32* ecx, u32* edx){
	__asm__("cpuid"
			: "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
			: "a" (*eax));
}

u8 kernel_checkStandardCpuid(u32 eax){
	return eax <= _kernel_maxCPUID;
}

u8 kernel_checkExtendedCpuid(u32 eax){
	return eax <= _kernel_maxCPUIDEX;
}

void kernel_getMaxCpuid(){
	u32 a = 0, b = 0, c = 0, d = 0;
	kernel_cpuid(&a, &b, &c, &d);
	_kernel_maxCPUID = a;
	a = 0x80000000;
	kernel_cpuid(&a, &b, &c, &d);
}

void kernel_memset(void* mem, u8 val, u32 siz){
	char* a = (char*)mem;
	char* b = a + (siz - 1);

	while (a != b)
		*a++ = val;
}