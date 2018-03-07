#ifndef _NSO_INTERRUPT_H
#define _NSO_INTERRUPT_H

#include "DataType.h"
//eax, ecx edx, ebx, esp, ebp, esi, edi
struct InterruptArguments {
    u32 edi, esi, ebp, esp;
    u32 ebx, edx, ecx, eax;
    
    u32 vector_number;
    u32 cs, eip, error_code;
    u32 eflags;
};

typedef void(*InterruptServiceRoutine)(struct InterruptArguments*);

extern void interrupt_handler(struct InterruptArguments);

void isr_0(struct InterruptArguments*);
void isr_1(struct InterruptArguments*);
void isr_2(struct InterruptArguments*);
void isr_3(struct InterruptArguments*);
void isr_4(struct InterruptArguments*);

#endif