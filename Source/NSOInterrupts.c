#include "NSOInterrupts.h"
#include "NSOUtils.h"



InterruptServiceRoutine exeception_isrs[256] = {isr_0, isr_1, isr_2, isr_3, isr_4};

void interrupt_handler(struct InterruptArguments arguments){
    
    kernel_halt();
    
    InterruptServiceRoutine handler = exeception_isrs[arguments.vector_number];
    if (IS_NULL(handler)){
        kernel_cli();
        kernel_halt();
    }else{
        handler(&arguments);
    }
}


void isr_0(struct InterruptArguments* pArguments){
    kernel_cli();
    __asm__("mov eax, 0");
    kernel_halt();
}

void isr_1(struct InterruptArguments* pArguments){
    kernel_cli();
    __asm__("mov eax, 1");
    kernel_halt();
}

void isr_2(struct InterruptArguments* pArguments){
    kernel_cli();
    __asm__("mov eax, 2");
    kernel_halt();
}

void isr_3(struct InterruptArguments* pArguments){
    kernel_cli();
    __asm__("mov eax, 3");
    kernel_halt();
}

void isr_4(struct InterruptArguments* pArguments){
    kernel_cli();
    __asm__("mov eax, 4");
    kernel_halt();
}


