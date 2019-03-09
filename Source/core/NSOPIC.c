#include "NSOPIC.h"
#include "NSOAPIC.h"

kernel_InterruptFunc _kernel_irqHandler[256-32] = {0};

static u8 _kernel_picDevice = 0;

void kernel_initIRQ(){
	if (kernel_hasAPIC()){
		//kernel_printfBOCHS("Initalizing ACPI");
		kernel_initAPIC();
	}else{
		_kernel_picDevice = 1;
		//kernel_printfBOCHS("Initalizing 8259A");
		//kernel_init8259A();
	}
}

