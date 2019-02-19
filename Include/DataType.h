#ifndef _DATATYPE_H
#define _DATATYPE_H

#define NULL (void*)0

typedef unsigned char u8;
typedef signed char s8;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned int u32;
typedef signed int s32;

typedef unsigned long long int u64;
typedef signed long long int s64;

struct kernel_DescriptorPointers {
	u16 size;
	u32 location;
}__attribute__((packed));

struct kernel_IRegs {
	u32 ss, ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 error_code;
}__attribute__((packed));


#endif 