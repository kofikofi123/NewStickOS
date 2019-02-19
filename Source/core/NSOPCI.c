#include "NSOPCI.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"

void kernel_pciFindDevice(u32 vendorID, u32 deviceID, u8* busNumber, u8* deviceNumber){

}


u8 kernel_pciReadConfig8(u8 busNumber, u8 deviceNumber, u8 reg, u8 offset){
	return kernel_pciReadConfig32(busNumber, deviceNumber, reg) >> (offset << 3);
}

u16 kernel_pciReadConfig16(u8 busNumber, u8 deviceNumber, u8 reg, u8 offset){
	return kernel_pciReadConfig32(busNumber, deviceNumber, reg) >> (offset << 4);
}

u32 kernel_pciReadConfig32(u8 busNumber, u8 deviceNumber, u8 reg){
	u32 addr = (0x80000000) | (busNumber << 16) | (deviceNumber << 11) | (reg << 2);

	kernel_outdw(0xCF8, addr);

	return kernel_indw(0xCFC);
}

void kernel_pciDebug(u8 bus, u8 device){
	u32 l0 = kernel_pciReadConfig32(device, bus, 0);
	u8 ht = kernel_pciReadConfig8(device, bus, 0x0C, 2);
	kernel_printfBOCHS("{\n\tVendorID: %x\n\tDeviceID: %x\n\tHeader: %x\n}", l0 & 0xFFFF, (l0 >> 16) & 0xFFFF, ht);
}