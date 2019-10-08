#include "NSOATA.h"
#include "NSOPCI.h"
#include "NSOAllocator.h"
#include "NSOCoreUtils.h"
#include "NSOInterrupts.h"
#include "NSOIOAPIC.h"

struct kernel_ataDevice _kernel_ataDevices = {.next = NULL};

static struct kernel_ideChannel* _kernel_createNewIDEChannel(struct kernel_pciDevice*);
static void _kernel_probeIdeChannel(struct kernel_ideChannel*);

static KERNEL_DEFINE_INTERRUPT(ATA){
	kernel_printfBOCHS("ATA device finished something\n");
}

void kernel_detectATADevices(){
	struct kernel_pciDevice* currentController = NULL;

	currentController = kernel_pciSearchDeviceByClass(0x01, 0x01, currentController);

	while (currentController != NULL){
		struct kernel_ideChannel* ideChannel = _kernel_createNewIDEChannel(currentController);

		if (!ideChannel)
			kernel_panic("Out of memory");
		


		_kernel_probeIdeChannel(ideChannel);

		currentController = kernel_pciSearchDeviceByClass(0x01, 0x01, currentController);
	}	
}

static struct kernel_ideChannel* _kernel_createNewIDEChannel(struct kernel_pciDevice* device){
	struct kernel_ideChannel* channel = kernel_malloc(sizeof(struct kernel_ideChannel), 4);
	struct kernel_IOAPIC* ioapic = kernel_getIOAPIC(0);

	struct kernel_RedirectionIRQ ataIRQ;
	ataIRQ.deliveryMode = 0;
	ataIRQ.destinationMode = 0;
	ataIRQ.intpol = 0;
	ataIRQ.triggerMode = 0;
	ataIRQ.interruptMask = 0;
	ataIRQ.destinationField = 0;

	if (channel){
		u8 progIF = kernel_pciReadConfig8(device->busNumber, 
										  device->deviceNumber, 
										  device->functionNumber, 
										  2, 
										  1);

		channel->pciDevice = device;
		channel->progIF = progIF;

		u32* ioBases = channel->ioBases;

		if ((progIF & 0x01) == 1){
			ioBases[KERNEL_IDECHANNEL_PRIMARY_COMMAND] = kernel_pciReadConfig32(device->busNumber,
																				device->deviceNumber,
																				device->functionNumber,
																				4);
			ioBases[KERNEL_IDECHANNEL_PRIMARY_CONTROL] = kernel_pciReadConfig32(device->busNumber,
																				device->deviceNumber,
																				device->functionNumber,
																				5);
		}else{
			ioBases[KERNEL_IDECHANNEL_PRIMARY_COMMAND] = 0x1F0;
			ioBases[KERNEL_IDECHANNEL_PRIMARY_CONTROL] = 0x3F6;


			u32 irq = kernel_getIrqMapping(14);

			kernel_printfBOCHS("irq: %d\n", irq);

			ataIRQ.interruptVector = 0x2E;

			kernel_writeRedirectionTable(ioapic, &ataIRQ, irq);
			kernel_requestIRQ(0x2E, "ATA - Master", kernel_ATA, 0);
		}

		if ((progIF & 0x04) == 0x04){
			ioBases[KERNEL_IDECHANNEL_SECONDARY_COMMAND] = kernel_pciReadConfig32(device->busNumber,
																				device->deviceNumber,
																				device->functionNumber,
																				6);
			ioBases[KERNEL_IDECHANNEL_SECONDARY_CONTROL] = kernel_pciReadConfig32(device->busNumber,
																				device->deviceNumber,
																				device->functionNumber,
																				7);
		}else{
			ioBases[KERNEL_IDECHANNEL_SECONDARY_COMMAND] = 0x170;
			ioBases[KERNEL_IDECHANNEL_SECONDARY_CONTROL] = 0x376;

			u32 irq = kernel_getIrqMapping(15);

			ataIRQ.interruptVector = 0x2F;
			
			kernel_writeRedirectionTable(ioapic, &ataIRQ, irq);
			kernel_requestIRQ(0x2F, "ATA - Slave", kernel_ATA, 0);
		}
	}

	kernel_pciWriteConfig16(device->busNumber,
						   device->deviceNumber,
						   device->functionNumber,
						   1,
						   0,
						   0b1000000001);

	return channel;
}

static void _kernel_probeIdeChannel(struct kernel_ideChannel* ideChannel){
	u32 primCommandIO = ideChannel->ioBases[KERNEL_IDECHANNEL_PRIMARY_COMMAND];
	u32 primControlIO = ideChannel->ioBases[KERNEL_IDECHANNEL_PRIMARY_CONTROL];

	kernel_out8(primCommandIO + 6, 0b10100000);
	kernel_out8(primCommandIO + 7, 0x90);

	kernel_hlt();
}