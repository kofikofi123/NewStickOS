#include "NSOACPI.h"
#include "NSOPaging.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOBiosMaps.h"
#include "NSOStringUtils.h"

struct kernel_ACPINamespace kernel_rootNamespace;
static void* _kernel_rsdt = NULL;

static void* _kernel_scanRSD(u32, u32);
static u8 _kernel_checksumACPI(void*, u32);
static void* _kernel_findACPITable32(const char*);
static void* _kernel_findACPITable64(const char*);
static void _kernel_mapACPIRegion();

u8 kernel_initACPI(){
	//search EBDA
	u32 ebdaBase = (u32)(*(u16*)(0x40E)) << 4;
	u32 ebdaEnd = ebdaBase + 0x1000;
	
	struct kernel_RSDP* rsdp = NULL;
	if ((rsdp = _kernel_scanRSD(ebdaBase, ebdaEnd)) == NULL){
		if ((rsdp = _kernel_scanRSD(0xE0000, 0xFFFFF)) == NULL)
			return 0;
	}

	void* tempAddr = NULL;

	if (!_kernel_checksumACPI(rsdp, 20))
		return 0;

	if (rsdp->revision >= 2){ 
		if (!_kernel_checksumACPI(rsdp, rsdp->length))
			return 0;
		tempAddr = (void*)((u32)rsdp->xsdtAddress);
	}else
		tempAddr = (void*)rsdp->rsdtAddress;

	_kernel_mapACPIRegion();
	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)tempAddr;

	if (!_kernel_checksumACPI(header, header->length))
		return 0;

	_kernel_rsdt = tempAddr;

	return 1;
}

void* kernel_findACPITable(const char* str){
	if (_kernel_rsdt == NULL) return NULL;

	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)_kernel_rsdt;

	char determinant = header->signature[0];

	if (determinant == 'X')
		return _kernel_findACPITable64(str);
	else
		return _kernel_findACPITable32(str);

}


static void* _kernel_findACPITable32(const char* str){
	struct kernel_ACPIHeader* header = (struct kernel_ACPIHeader*)_kernel_rsdt;


	u32 entries = (header->length - sizeof(struct kernel_ACPIHeader))/4;

	u32* tempAddrs = (u32*)(header + 1);

	struct kernel_ACPIHeader* tempHeader = NULL;

	while (entries-- > 0){
		tempHeader = (struct kernel_ACPIHeader*)(*tempAddrs++);

		if (kernel_stringCompareRAW(str, tempHeader->signature, 4)){
			if (_kernel_checksumACPI(tempHeader, tempHeader->length))
				return tempHeader;
			else
				return NULL;
		}
	}

	return NULL;
}

static void* _kernel_findACPITable64(const char* str){
	return NULL;
}


static u8 _kernel_checksumACPI(void* addr, u32 size){
	return kernel_calculateChecksum(addr, size) == 0;
}

static void* _kernel_scanRSD(u32 base, u32 endR){
	base = base & ~(0xFFF);
	endR = endR & ~(0xFFF);
	if (!kernel_mapIdentity(base, endR, 0x02))
		return NULL;

	const char* rsdsig = "RSD PTR ";

	void *cur = (void*)base;
	void *end = (void*)endR;


	while (cur != end){
		if (kernel_memcmp(cur, rsdsig, 8) == 0){
			return cur;
		}
		cur = cur + 0x10;
	}

	kernel_unmapIdentity(base, endR);
	return NULL;
}

static void _kernel_mapACPIRegion(){
	struct kernel_BMM* temp = NULL;
	for (u8 i = 0; i < kernel_getMemMapEntryLength(); i++){
		temp = kernel_getMemMapEntry(i);
		if (temp->type == 3){
			u32 base = temp->base;
			kernel_mapIdentity(base, base + (temp->len - 1), 0x02);
		}
	}
}