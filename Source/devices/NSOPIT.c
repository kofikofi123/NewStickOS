#include "NSOCoreUtils.h"
#include "NSOInterruptCTRL.h"
#include "NSOIOAPIC.h"
#include "NSOBochs.h"
#include "NSOPCI.h"

//static u32 _kernel_pitFreq = 0;
static u32 _kernel_pitTickRAW = 0;
static u32 _kernel_pitTick = 0;

static KERNEL_DEFINE_INTERRUPT(PIT){
	_kernel_pitTickRAW++;

	if ((_kernel_pitTickRAW % 1193) == 0){
		_kernel_pitTick++;
		kernel_printfBOCHS("Time generator %d\n", _kernel_pitTick);
	}
}

void kernel_initPIT(){
	///////////////////////////////////////////
	u32 irq = kernel_getIrqMapping(0);

	struct kernel_RedirectionIRQ pitIRQ;
	struct kernel_IOAPIC* ioapic = kernel_getIOAPIC(0);

	pitIRQ.interruptVector = 0x20;
	pitIRQ.deliveryMode = 0;
	pitIRQ.destinationMode = 0;
	pitIRQ.intpol = 0;
	pitIRQ.triggerMode = 0;
	pitIRQ.interruptMask = 0;
	pitIRQ.destinationField = 0;

	kernel_writeRedirectionTable(ioapic, &pitIRQ, irq);
	kernel_requestIRQ(0x20, "ISA PIT", kernel_PIT, 0);

}

void kernel_setTimer(u16 count){

	kernel_out8(0x43, (0x03 << 4) | (3 << 1));
	kernel_out8(0x40, count & 0xFF);
	kernel_out8(0x40, count >> 8);
}

