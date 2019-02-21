#include "NSOPaging.h"
#include "NSOCoreUtils.h"
#include "NSOPAllocator.h"
#include "NSOBochs.h"

extern u32 kernel_end;
static u32* _kernel_local_pageDirectory;

static u8 _kernel_checkPagingBits(u32, u8);
static u32* _kernel_getPageTable(u32);

void kernel_initPaging(){
	_kernel_local_pageDirectory = kernel_allocatePage();
	if (_kernel_local_pageDirectory == NULL)
		kernel_panic("What?");

	for (u16 i = 0; i < 1024; i++){
		_kernel_local_pageDirectory[i] = 0x02;
	}

	kernel_loadPageTable(0);
	kernel_loadPageDirectory(_kernel_local_pageDirectory);
}

void kernel_identityMap(u32 start, u32 end){
	end = (end + 0x1000) & ~(0xFFF);
}

u8 kernel_mapIdentity(u32 addr, u16 pages){
	addr = addr & ~(0xFFF);

	u32* pt_addr = _kernel_getPageTable(addr);
	if (pt_addr == NULL) return 0;
	u32* pt = &pt_addr[(addr >> 12) & 0x3FF];
	while (pages-- > 0){
		*pt = addr | 0x03;
		pt++;
		addr += 0x1000;
	}
	return 1;
}

u8 kernel_mapAddress(u32 physAddr, u32 virtAddr){
	u32* pt_addr = _kernel_getPageTable(virtAddr);
	if (pt_addr == NULL) return 0;
	u32* pt = &pt_addr[(virtAddr >> 12) & 0x3FF];
	*pt = physAddr | 0x03;
	return 1;
}

u32* kernel_loadPageTable(u16 pId){
	u32* pt = kernel_allocatePage();
	if (pt == NULL) return NULL;
	kernel_memset(pt, 0, 4096);


	_kernel_local_pageDirectory[pId] = (u32)pt | 0x100 | 0x01;
	return pt;
}

static inline u8 _kernel_checkPagingBits(u32 ps, u8 bit){
	return (ps >> bit) & 0x01;
}

static u32* _kernel_getPageTable(u32 virtAddr){
	u32 pId = (virtAddr >> 22);
	u32 pd = _kernel_local_pageDirectory[pId];

	u32* pt_addr = NULL;

	if (!_kernel_checkPagingBits(pd, 8)){
		pt_addr = kernel_loadPageTable(pId);
	}else
		pt_addr = (u32*)((pd & ~(0xFFF)));
	
	return pt_addr;
}

/*u8 kernel_reservePage(void*);
u8 kernel_reservePage2(void*, void*);
u8 kernel_releasePage(void*);*/