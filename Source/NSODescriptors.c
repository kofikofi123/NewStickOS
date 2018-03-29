#include "NSODescriptors.h"
#include "NSOAssistedInterrupt.h"

struct __attribute__((aligned(8))) InterruptDescriptor IDT[256];
struct PeusdoDescriptor m_pseudo;

void kernel_initExceptions(void){
    kernel_setIDTEntry(&IDT[0], (u32)&(int_divide), 0x08, 0, 0x8F);
    kernel_setIDTEntry(&IDT[1], (u32)&(int_debug), 0x08, 0, 0x8F);
    kernel_setIDTEntry(&IDT[2], (u32)&(int_nmi), 0x08, 0, 0x8F);
    kernel_setIDTEntry(&IDT[3], (u32)&(int_breakpoint), 0x08, 0, 0x8F);
    kernel_setIDTEntry(&IDT[4], (u32)&(int_overflow), 0x08, 0, 0x8F);
    kernel_setIDTEntry(&IDT[8], (u32)&(int_doubleFault), 0x08, 0, 0x8F);
    kernel_setIDTEntry(&IDT[14],(u32)&(int_pageFault), 0x08, 0, 0x8F);
    
    m_pseudo.limit = sizeof(IDT) - 1;
    m_pseudo.base = (u32)(&IDT[0]);
    
    kernel_loadIDT((void*)(&m_pseudo));
}

void kernel_setIDTEntry(struct InterruptDescriptor* idt_struct, u32 offset, u16 selector, u8 type0, u8 type1){
    idt_struct->Offset0 = (offset & 0xFFFF);
    idt_struct->Selector = selector;
    idt_struct->Type0 = type0;
    idt_struct->Type1 = type1;
    idt_struct->Offset1 = ((offset >> 16) & 0xFFFF);
};

/*

There are some aspects that the entertainment industries can change.
Many things were debunked, like violent movies create violent humans, but it does seem like it does desensitize maybe a very small minority of people. There is one main thing entertainment industries can change, and that is the moral values of the people.
Way back in 19th century, a play was written and published 
*/
//5.10.1