#include "NSOACPI.h"
#include "NSOPaging.h"
#include "NSOAllocator.h"
#include "NSOLocks.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOPCI.h"


ACPI_STATUS AcpiOsInitialize(){
	kernel_printStringBOCHS("Initalizing ACPICA\n");
	return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(){
	kernel_printStringBOCHS("Terminating ACPICA\n");
	return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(){
	ACPI_PHYSICAL_ADDRESS addr = 0;
	AcpiFindRootPointer(&addr);
	//kernel_printfBOCHS("okr: %x\n", addr);
	return addr;
}

ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES* predefinedObject, ACPI_STRING* NewValue){
	*NewValue = NULL;
	return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER* existingTable, ACPI_TABLE_HEADER** NewTable){
	*NewTable = NULL;
	return AE_OK;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER* existingTable, ACPI_PHYSICAL_ADDRESS* newAddress, UINT32* newTableLength){
	*newAddress = 0;
	*newTableLength = 0;
	return AE_OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/*ACPI_STATUS AcpiOsCreateCache(char* cacheName, UINT16 objectSize, UINT16 maxDepth, ACPI_CACHE_T **returnCache){
	return AE_OK;
}

ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T* cache){
	return AE_OK;
}

ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T* cache){
	return AE_OK;
}

void* AcpiOsAquireObject(ACPI_CACHE_T *cache){
	return NULL;
}

ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T* Cache, void* object){
	return AE_OK;
}*/

void* AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysAddr, ACPI_SIZE Length){
	ACPI_SIZE len = ((Length + 0x1000) & ~(0xFFF));

	if (!kernel_mapIdentity((u32)PhysAddr, (u32)(PhysAddr + len), 0x02))
		return NULL;

	return ACPI_TO_POINTER(PhysAddr);
}

void AcpiOsUnmapMemory(void* vaddr, ACPI_SIZE Length){
	ACPI_SIZE len = ((Length + 0x1000) & ~(0xFFF));
	kernel_unmapIdentity((u32)vaddr, (u32)(vaddr + len));
}

ACPI_STATUS AcpiOsGetPhysicalAddress(void* logicalAddress, ACPI_PHYSICAL_ADDRESS* physicalAddress){
	if (logicalAddress == NULL || physicalAddress == NULL)
		return AE_BAD_PARAMETER;

	void* temp = kernel_getPhysicalAddress((u32)logicalAddress);

	ACPI_STATUS ret = AE_OK;
	if (temp == NULL){
		*physicalAddress = 0;
		ret = AE_ERROR;
	}else
		*physicalAddress = (u32)physicalAddress;

	return ret;
}

void* AcpiOsAllocate(ACPI_SIZE size){
	//kernel_printfBOCHS("Allocating: %x bytes\n", size);
	return kernel_malloc(size, 1);
}

void AcpiOsFree(void* memory){
	return kernel_free(memory);
}

BOOLEAN AcpiOsReadable(void* Memory, ACPI_SIZE Length){
	return TRUE;
}

BOOLEAN AcpiOsWriteable(void* Memory, ACPI_SIZE Length){
	return TRUE;
}

///////////////////////////////////////////////////////////////////////

ACPI_THREAD_ID AcpiOsGetThreadId(){
	return 0;
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE type, ACPI_OSD_EXEC_CALLBACK function, void* context){
	return AE_OK;
}

void AcpiOsSleep(UINT64 milliseconds){}

void AcpiOsStall(UINT32 microseconds){}

void AcpiOsWaitEventsComplete(){}

////////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX* outHandle){
	if (outHandle == NULL) return AE_BAD_PARAMETER;
	void* handle = kernel_malloc(sizeof(u32), 4);

	if (handle == NULL)
		return AE_NO_MEMORY;

	*outHandle = handle;
	return AE_OK;
}

void AcpiOsDeleteMutex(ACPI_MUTEX Handle){
	kernel_free(Handle);
}

ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX handle, UINT16 timeout){
	if (handle == NULL) 
		return AE_BAD_PARAMETER;

	kernel_acquireMutex(handle);
	//for now
	return AE_OK;
}

void AcpiOsReleaseMutex(ACPI_MUTEX handle){
	kernel_releaseLock(handle);
}

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 maxUnits, UINT32 initalUnits, ACPI_SEMAPHORE* outHandle){
	return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE handle){
	return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE handle, UINT32 units, UINT16 timeout){
	return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE handle, UINT32 units){
	return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK* outHandle){
	if (outHandle == NULL) return AE_BAD_PARAMETER;

	ACPI_SPINLOCK lock = kernel_createLock();

	if (lock == NULL)
		return AE_NO_MEMORY;

	*outHandle = lock;
	return AE_OK;
}

void AcpiOsDeleteLock(ACPI_HANDLE handle){
	kernel_free(handle);
}


ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle){
	ACPI_CPU_FLAGS flag = kernel_getEFLAGS();

	kernel_acquireSpinlock(Handle);

	return flag;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK handle, ACPI_CPU_FLAGS flags){
	kernel_releaseLock(handle);

	kernel_setEFLAGS(flags);
}

///////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 interruptLevel, ACPI_OSD_HANDLER handler, void* context){
	kernel_printfBOCHS("OKR 1\n");
	__asm__("xchg bx, bx");
	return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 interruptLevel, ACPI_OSD_HANDLER handler){
	kernel_printfBOCHS("OKR 2\n");
	__asm__("xchg bx, bx");
	return AE_OK;
}

///////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS address, UINT64* value, UINT32 width){
	kernel_printfBOCHS("Testing (read): %x\n", address);
	switch (width){
		case 8:
			*(u8*)value = *(u8*)address;
			break;
		case 16: 
			*(u16*)value = *(u16*)address;
			break;
		case 32:
			*(u32*)value = *(u32*)address;
			break;
		case 64:
			*(u64*)value = *(u64*)address; //possible ono
			break;
	}
	return AE_OK;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS address, UINT64 value, UINT32 width){
	kernel_printfBOCHS("Testing (write): %x\n", address);
	switch (width){
		case 8:
			*(u8*)address = (u8)value;
			break;
		case 16: 
			*(u16*)address = (u16)value;
			break;
		case 32:
			*(u32*)address = (u32)value;
			break;
		case 64:
			*(u64*)address = (u64)value;
			break;
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsReadPort(ACPI_PHYSICAL_ADDRESS address, UINT32* value, UINT32 width){
	switch (width){
		case 8:
			*(u8*)value = kernel_in8(address);
			break;
		case 16:
			*(u16*)value = kernel_in16(address);
			break;
		case 32:
			*(u32*)value = kernel_in32(address);
			break;
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePort(ACPI_PHYSICAL_ADDRESS address, UINT32 value, UINT32 width){
	switch (width){
		case 8:
			kernel_out8(address, (u8)value);
			break;
		case 16:
			kernel_out16(address, (u16)value);
			break;
		case 32:
			kernel_out32(address, (u32)value);
			break;
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *pciID, UINT32 reg, UINT64 *value, UINT32 width){
	switch (width){
		case 8:
			*(u8*)value = kernel_pciReadConfig8(pciID->Bus, pciID->Device, pciID->Function, pciID->Segment, reg);
			break;
		case 16:
			*(u16*)value = kernel_pciReadConfig16(pciID->Bus, pciID->Device, pciID->Function, pciID->Segment, reg);
			break;
		case 32:
			*(u32*)value = kernel_pciReadConfig32(pciID->Bus, pciID->Device, pciID->Function, reg);
			break;
	}

	return AE_OK;
}


ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *pciID, UINT32 reg, UINT64 value, UINT32 width){
	kernel_printfBOCHS("OKR 3\n");
	__asm__("xchg bx, bx");
	return AE_OK;
}

/////////////////////////////////////////////////////////////////////////////

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char* format, ...){
	kernel_printStringBOCHS(format); //for now
}


void AcpiOsVprintf(const char* format, va_list list){
	kernel_printfBOCHS("OK: \"%s\"", format);
}

UINT64 AcpiOsGetTimer(){
	return 0;
}

ACPI_STATUS AcpiOsSignal(UINT32 func, void* info){
	return AE_OK;
}

/////////////////////////////////////////////////////////////////////

ACPI_STATUS AcpiOsEnterSleep(UINT8 sleep, UINT32 rega, UINT32 regb){
	return AE_OK;
}

ACPI_STATUS AcpiOsWaitCommandReady(){
	return AE_OK;
}

ACPI_STATUS AcpiOsNotifyCommandComplete(){
	return AE_OK;
}

ACPI_STATUS AcpiOsInitializeDebugger(){
	return AE_OK;
}

void AcpiOsTerminateDebugger(){}