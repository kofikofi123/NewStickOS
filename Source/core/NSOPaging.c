#include "NSOPaging.h"
#include "NSOCoreUtils.h"
#include "NSOPAllocator.h"
#include "NSOBochs.h"

extern u32 kernel_end;
static u32* _kernel_local_pageDirectory;


static u32* _kernel_getPageTable(u32);
static void _kernel_loadPageDirectory(u16, u32, u8);
static u8 _kernel_mapPageTable(u32);

void kernel_initPaging(){
	_kernel_local_pageDirectory = kernel_allocatePage();

	if (_kernel_local_pageDirectory == NULL)
		kernel_panic("Unable to allocate page for page directory!");

	for (u16 i = 0; i < 1024; i++)
		_kernel_local_pageDirectory[i] = 0x02;

	void* pt = kernel_allocatePage();
	if (pt == NULL)
		kernel_panic("Unable to allocate page for page table!");

	u32 pt32 = (u32)pt;
	u32 pd32 = (u32)_kernel_local_pageDirectory;

	_kernel_loadPageDirectory(0, pt32, 0x02);
	kernel_mapAddress(pd32, pd32, 0x02);
	kernel_mapAddress(pt32, pt32, 0x02);

	kernel_loadPageDirectory(_kernel_local_pageDirectory);
}

u8 kernel_mapAddress(u32 virtAddr, u32 physAddr, u8 flags){
	u32* pt_addr = _kernel_getPageTable(virtAddr);

	if (pt_addr == NULL) {
		u32* pt_addr = kernel_allocatePage();
		if (pt_addr == NULL)
			return 0;/*
		if (!_kernel_mapPageTable((u32)pt_addr)){
			kernel_freePage(pt_addr);
			return 0;
		}*/
		u32 pt32 = (u32)pt_addr;
		_kernel_loadPageDirectory(pt32 >> 22, pt32, 0x02);
	}

	u32* pt = &pt_addr[(virtAddr >> 12) & 0x3FF];
	*pt = (physAddr & ~(0xFFF)) | flags | 0x01;
	return 1;
}

void kernel_unmapAddress(u32 virtAddr){
	u32* pt_addr = _kernel_getPageTable(virtAddr);

	if (pt_addr == NULL) return;

	u32* pt = &pt_addr[(virtAddr >> 12) & 0x3FF];
	*pt = 0;
}

//probably can only map 4 mib at a time
u8 kernel_mapIdentity(u32 from, u32 to, u8 flags){
	from = (from & ~(0xFFF));
	to = ((to + 0x1000) & ~(0xFFF));

	u32* pt_addr = _kernel_getPageTable(from);

	if (pt_addr == NULL) {
		u32* pt_addr = kernel_allocatePage();
		if (pt_addr == NULL)
			return 0;/*
		if (!_kernel_mapPageTable((u32)pt_addr)){
			kernel_freePage(pt_addr);
			return 0;
		}*/
		u32 pt32 = (u32)pt_addr;
		_kernel_loadPageDirectory(pt32 >> 22, pt32, 0x02);
	}

	u32* pt = &pt_addr[(from >> 12) & 0x3FF];
	u32 temp = from;
	u32 pages = (to - from) >> 12;
	while (pages-- > 0){
		*pt = temp | flags | 0x01;
		pt++;
		temp += 0x1000;
	}
	return 1;
}

static u32* _kernel_getPageTable(u32 virtAddr){
	u32 pId = (virtAddr >> 22);
	u32 pd = _kernel_local_pageDirectory[pId];

	u32* pt_addr = NULL;

	if ((pd & 0x100) == 0x100)
		pt_addr = (u32*)((pd & ~(0xFFF)));
	
	return pt_addr;
}

static void _kernel_loadPageDirectory(u16 pId, u32 addr, u8 flags){
	_kernel_local_pageDirectory[pId] = addr | flags | 0x100 | 0x01;
}

void* kernel_vAllocatePage(u32 addr, u32 pages, u8 flags){

	u32 tempA = addr, tempB = tempA;
	u32 i = 0;
	void* tempPage = NULL;
	for (; i < pages; i++){
		tempPage = kernel_allocatePage();

		if (tempPage == NULL)
			break;

		kernel_mapAddress(tempA, (u32)tempPage, flags);
		tempA += 0x1000;
	}

	if (((pages - 1) - i)) return (void*)addr;

	kernel_vFreePage(tempB, i + 1);

	return NULL;
}

void kernel_vFreePage(u32 addr, u32 pages){
	for (u32 i = 0; i < pages; i++){
		kernel_unmapAddress(addr);
		kernel_freePage((void*)addr);
		addr += 0x1000;
	}
}
/*

static u8 _kernel_mapPageTable(u32 ptAddr){
	u32* pt_addr = _kernel_getPageTable(ptAddr);

	if (pt_addr == NULL){

	}
}*/