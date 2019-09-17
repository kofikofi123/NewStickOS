#include "NSOPCI.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"
#include "NSOAllocator.h"

//Osdev impl vvvvvvvvvvvvvvv

struct kernel_pciBus kernel_hostBus = {
	.busNumber = 0,
	.next = NULL,
	.devices = NULL
};

static void _kernel_enumerateBus(u8);
static u8 _kernel_enumerateDevice(u8, u8);
static void _kernel_enumerateFunction(u8, u8, u8);


static struct kernel_pciBus* _kernel_pciGetBus(u8);
static void _kernel_pciAppendDeviceToBus(struct kernel_pciBus*, struct kernel_pciDevice*);
static void _kernel_pciAppendBuses(struct kernel_pciBus*, struct kernel_pciBus*);
static struct kernel_pciDevice* _kernel_pciCreateDevice(u16, u16, u8, u8, u8);
static struct kernel_pciDevice* _kernel_pciSearchDevice(struct kernel_pciBus*, u16, u16);
static struct kernel_pciBus* _kernel_pciCreateBus(u8);

//////////////////////////////////////////////////////////////////////////////////////////////////////
u8 kernel_pciReadConfig8(u8 busNumber, u8 deviceNumber, u8 function, u8 reg, u8 offset){
	return (kernel_pciReadConfig32(busNumber, deviceNumber, function, reg) >> (offset << 3)) & 0xFF;
}

u16 kernel_pciReadConfig16(u8 busNumber, u8 deviceNumber, u8 function, u8 reg, u8 offset){
	return (kernel_pciReadConfig32(busNumber, deviceNumber, function, reg) >> (offset << 4)) & 0xFFFF;
}

u32 kernel_pciReadConfig32(u8 busNumber, u8 deviceNumber, u8 function, u8 reg){
	u32 addr = (0x80000000) | (busNumber << 16) | (deviceNumber << 11) | (function << 8) | (reg << 2);

	kernel_out32(0xCF8, addr);

	return kernel_in32(0xCFC);
}

void kernel_enumeratePCI(){//todo (other stuff)
	_kernel_enumerateBus(0);
}

struct kernel_pciDevice* kernel_pciSearchDevice(u16 vendorID, u16 deviceID){
	struct kernel_pciBus* bus = &kernel_hostBus;
	struct kernel_pciDevice* device = NULL;

	while (bus != NULL){
		device = _kernel_pciSearchDevice(bus, vendorID, deviceID);

		if (device != NULL)
			break;
	}

	return device;
}

struct kernel_pciDevice* kernel_pciIterSearchDevice(u16 vendorID, struct kernel_pciDevice* old){
	//struct kernel_pciBus* bus = &kernel_hostBus;
	return NULL;
}

static struct kernel_pciDevice* _kernel_pciSearchDevice(struct kernel_pciBus* bus, u16 vendorID, u16 deviceID){
	struct kernel_pciDevice* mainDevice = bus->devices;

	while (mainDevice != NULL){
		if (mainDevice->vendorID == vendorID && mainDevice->deviceID == deviceID)
			break;
		mainDevice = mainDevice->next;
	}

	return mainDevice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void kernel_pciWriteConfig32(u8 busNumber, u8 deviceNumber, u8 function, u8 reg, u32 val){
	u32 addr = (0x80000000) | (busNumber << 16) | (deviceNumber << 11) | (function << 8) | (reg << 2);

	kernel_out32(0xCF8, addr);
	kernel_out32(0xCFC, val);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void _kernel_enumerateBus(u8 bus){
	for (u8 i = 0; i < 32; i++){
		if (!(_kernel_enumerateDevice(bus, i)))
			break;
	}
}

static u8 _kernel_enumerateDevice(u8 bus, u8 device){
	u16 vendorID = kernel_pciReadConfig16(bus, device, 0, 0, 0);

	u8 header = kernel_pciReadConfig8(bus, device, 0, 3, 2);


	u8 isMultiFunc = (header >> 7);

	if (vendorID == 0xFFFF) return 0;

	_kernel_enumerateFunction(bus, device, 0);

	if (isMultiFunc){
		for (u8 i = 1; i < 8; i++){
			vendorID = kernel_pciReadConfig16(bus, device, i, 0, 0);

			if (vendorID == 0xFFFF) break;

			_kernel_enumerateFunction(bus, device, i);
		}
	}

	return 1;
}

static void _kernel_enumerateFunction(u8 bus, u8 device, u8 function){
	u32 temp = kernel_pciReadConfig32(bus, device, function, 0);
	u8 header = kernel_pciReadConfig8(bus, device, function, 3, 2);
	struct kernel_pciBus* mainBus = _kernel_pciGetBus(bus);
	u16 vendorID = (temp & 0xFFFF), deviceID = (temp >> 16);
	u8 deviceType = header & 0x7F;

	if (deviceType == 0){
		//normal device
		struct kernel_pciDevice* deviceNode = _kernel_pciCreateDevice(vendorID, deviceID, bus, device, function);
		

		_kernel_pciAppendDeviceToBus(mainBus, deviceNode);
	}else if (deviceType == 1){
		//pci-to-pci bridge
		u8 secondaryBus = kernel_pciReadConfig8(bus, device, function, 6, 1);

		struct kernel_pciBus* newBus = _kernel_pciCreateBus(secondaryBus);

		_kernel_pciAppendBuses(mainBus, newBus);

		_kernel_enumerateBus(secondaryBus);
	}
}

static void _kernel_pciAppendDeviceToBus(struct kernel_pciBus* bus, struct kernel_pciDevice* device){
	if (bus->devices != NULL){
		struct kernel_pciDevice* lastDevice = bus->devices;

		while (lastDevice->next != NULL)
			lastDevice = lastDevice->next;

		lastDevice->next = device;
	}else
		bus->devices = device;
}

static void _kernel_pciAppendBuses(struct kernel_pciBus* mainBus, struct kernel_pciBus* bus){
	struct kernel_pciBus* lastBus = mainBus;


	while (lastBus->next != NULL)
		lastBus = lastBus->next;

	lastBus->next = bus;
}

static struct kernel_pciBus* _kernel_pciGetBus(u8 busNumber){
	struct kernel_pciBus* bus = &kernel_hostBus;


	while (bus != NULL){
		if (bus->busNumber == busNumber)
			break;
		bus = bus->next;
	}

	return bus;
}

static struct kernel_pciDevice* _kernel_pciCreateDevice(u16 vendorID, u16 deviceID, u8 busNumber, u8 deviceNumber, u8 functionNumber){
	struct kernel_pciDevice* device = kernel_malloc(sizeof(struct kernel_pciDevice), 2);

	if (device != NULL){
		device->vendorID = vendorID;
		device->deviceID = deviceID;
		device->busNumber = busNumber;
		device->deviceNumber = deviceNumber;
		device->functionNumber = functionNumber;

		device->next = NULL;
	}

	return device;
}

static struct kernel_pciBus* _kernel_pciCreateBus(u8 busNumber){
	struct kernel_pciBus* bus = kernel_malloc(sizeof(struct kernel_pciBus), 2);

	if (bus != NULL){
		bus->busNumber = busNumber;
		bus->next = NULL;
		bus->devices = NULL;
	}

	return bus;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void kernel_pciDebug(u8 bus, u8 device, u8 function){
	u32 l0 = kernel_pciReadConfig32(bus, device, function, 0);
	kernel_printfBOCHS("{\n\tVendorID: %x\n\tDeviceID: %x\n\t", l0 & 0xFFFF, (l0 >> 16) & 0xFFFF);

	for (u8 i = 0; i < 6; i++){
		kernel_printfBOCHS("BAR%d: %x\n\t", i, kernel_pciReadConfig32(bus, device, function, 4 + i));
	}
	u32 sl0 = kernel_pciReadConfig32(bus, device, function, 11);
	kernel_printfBOCHS("Subsystem ID: %x\n\tSubsystem Vendor ID: %x\n", sl0 >> 16, sl0 & 0xFFFF);
	kernel_printfBOCHS("}\n");
}