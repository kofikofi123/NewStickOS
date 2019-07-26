#ifndef KERNEL_MP_H
#define KERNEL_MP_H

#include "DataType.h"

struct kernel_TSS {
	u32 LINK;
	u32 ESP0;
	u32 SS0;
	u32 ESP1;
	u32 SS1;
	u32 ESP2;
	u32 SS2;
	u32 CR3;
	u32 EIP;
	u32 EFLAGS;
	u32 EAX;
	u32 ECX;
	u32 EDX;
	u32 EBX;
	u32 ESP;
	u32 EBP;
	u32 ESI;
	u32 EDI;
	u32 ES;
	u32 CS;
	u32 SS;
	u32 DS;
	u32 FS;
	u32 GS;
	u32 LDTR;
	u32 IOPB;
};

struct kernel_ProcessorInfo {
	u8 apicID;
	u8 acpiID;
	u8 isUsable;

//

	struct kernel_TSS* processorTSS;
	
	struct kernel_ProcessorInfo* next;
};

void kernel_newProcessor(u8, u8, u8);

struct kernel_ProcessorInfo* kernel_getProc(u8);
struct kernel_ProcessorInfo* kernel_getProcByAPIC(u8);
struct kernel_ProcessorInfo* kernel_getProcByACPI(u8);

void kernel_setupTSS(struct kernel_ProcessorInfo*);

u8 kernel_getNumOfProc();

#endif