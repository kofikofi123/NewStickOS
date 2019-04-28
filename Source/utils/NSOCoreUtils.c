#include "NSOCoreUtils.h"
#include "NSOBochs.h"


static u32 _kernel_maxCPUID = 0, _kernel_maxCPUIDEX = 0;

void kernel_out8(u16 port, u8 data){
	__asm__ volatile ("out dx, al"
			:
			: "d" (port), "a" (data));
}

void kernel_out16(u16 port, u16 data){
	__asm__ volatile ("out dx, ax"
			:
			: "d" (port), "a" (data));
}

void kernel_out32(u16 port, u32 data){
	__asm__ volatile ("out dx, eax"
			:
			: "d" (port), "a" (data));
}

u8 kernel_in8(u16 port){
	u8 data = 0;
	__asm__ volatile ("in al, dx"
			: "=a" (data)
			: "d" (port));
	return data;
}

u16 kernel_in16(u16 port){
	u16 data = 0;
	__asm__ volatile ("in ax, dx"
			: "=a" (data)
			: "d" (port));
	return data;
}

u32 kernel_in32(u16 port){
	u32 data = 0;
	__asm__ volatile ("in eax, dx"
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
	__asm__ volatile ("pushfd\n\tpop eax"
			: "=a" (eflag));
	return eflag;
}

void kernel_setEFLAGS(u32 eflag){
	__asm__ volatile ("push eax\n\tpopfd"
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

	__asm__ volatile("cpuid"
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
	char* b = a + (siz);

	while (a != b)
		*a++ = val;
}

void kernel_memcpy(void* memA, const void* memB, u32 size){
	char* a = (char*)memA;
	char* b = (char*)memB;
	char* c = a + size;

	while (a != c)
		*a++ = *b++;
}

s8 kernel_memcmp(const void* memA, const void* memB, u32 size){
	char* a = (char*)memA, *b = (char*)memB, *c = a + size;

	char tA = 0;
	while (a != c){
		tA = (*a++ - *b++);

		if (tA != 0)
			return tA;
	}
	return 0;
}

void kernel_wrmsr(u32 c, u64 da){
	__asm__ volatile ("wrmsr"
					  :
					  : "c" (c), "d" ((u32)(da >> 32)), "a" ((u32)(da & 0xFFFFFFFF)));
}

u64 kernel_rdmsr(u32 c){
	u32 hi = 0, lo = 0;
	__asm__ volatile ("rdmsr"
					  : "=d" (hi), "=a"(lo)
					  : "c" (c));
	return ((u64)hi << 32) | (lo);
}

u8 kernel_calculateChecksum(void* buffer, u32 size){
	u8 checksum = 0;

	char* a = (char*)buffer;
	char* c = a + size;

	while (a != c)
		checksum += *a++;

	return checksum;
}