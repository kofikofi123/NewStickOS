#include "NSOPCI.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"

void kernel_pciFindDevice(u32 vendorID, u32 deviceID, u8* busNumber, u8* deviceNumber){

}


u8 kernel_pciReadConfig8(u8 busNumber, u8 deviceNumber, u8 function, u8 reg, u8 offset){
	return kernel_pciReadConfig32(busNumber, deviceNumber, function, reg) >> (offset << 3);
}

u16 kernel_pciReadConfig16(u8 busNumber, u8 deviceNumber, u8 function, u8 reg, u8 offset){
	return kernel_pciReadConfig32(busNumber, deviceNumber, function, reg) >> (offset << 4);
}

u32 kernel_pciReadConfig32(u8 busNumber, u8 deviceNumber, u8 function, u8 reg){
	u32 addr = (0x80000000) | (busNumber << 16) | (deviceNumber << 11) | (function << 8) | (reg << 2);

	kernel_outdw(0xCF8, addr);

	return kernel_indw(0xCFC);
}

void kernel_pciDebug(u8 bus, u8 device, u8 function){
	u32 l0 = kernel_pciReadConfig32(bus, device, function, 0);
	kernel_printfBOCHS("{\n\tVendorID: %x\n\tDeviceID: %x\n\t", l0 & 0xFFFF, (l0 >> 16) & 0xFFFF);

	for (u8 i = 0; i < 6; i++){
		kernel_printfBOCHS("BAR%d: %x\n\t", i, kernel_pciReadConfig32(bus, device, function, 4 + i));
	}
	u32 sl0 = kernel_pciReadConfig32(bus, device, function, 11);
	kernel_printfBOCHS("Subsystem ID: %x\n\tSubsystem Vendor ID: %x\n", sl0 >> 16, sl0 & 0xFFFF);
	kernel_printStringBOCHS("}\n");
}