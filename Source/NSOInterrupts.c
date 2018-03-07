#include "NSOInterrupts.h"
#include "NSOUtils.h"



InterruptServiceRoutine kernel_interrupt_routines[256] = {isr_0, isr_1, isr_2, isr_3, isr_4};

void interrupt_handler(struct InterruptArguments arguments){
    InterruptServiceRoutine handler = kernel_interrupt_routines[arguments.vector_number];
    if (IS_NULL(handler)){
        kernel_cli();
        kernel_halt();
    }else{
        handler(&arguments);
    }
}

//interrupts for now are just a way for me to see if I did anything wrong, or need to check something
void isr_0(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}

void isr_1(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}

void isr_2(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}

void isr_3(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}

void isr_4(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}

void isr_5(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}

void isr_6(struct InterruptArguments* pArguments){
    kernel_cli();
    kernel_halt();
}