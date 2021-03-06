#include "NSOPaging.h"
#include "NSOCoreUtils.h"
#include "NSOPAllocator.h"
#include "NSOBochs.h"

extern u32 kernel_end;
u32* kernel_pageDirectory;


//static u32* _kernel_getPageDir(u32);
static u32* _kernel_getPageDirR(u32);

static u8 _kernel_mapAddressR(u32, u32, u8);
static u8 _kernel_mapAddressN(u32, u32, u8);

static void _kernel_loadPageDirectory(u16, u32, u8);

static u8 _kernel_isPagingEnabled();

void kernel_initPaging(){
	kernel_pageDirectory = kernel_allocatePage();

	if (kernel_pageDirectory == NULL)
		kernel_panic("Unable to allocate page for page directory!");

	for (u16 i = 0; i < 1024; i++)
		kernel_pageDirectory[i] = 0x02;

	void* pt = kernel_allocatePage();
	if (pt == NULL)
		kernel_panic("Unable to allocate page for page table!");

	u32 pt32 = (u32)pt;
	u32 pd32 = (u32)kernel_pageDirectory;

	_kernel_loadPageDirectory(0, pt32, 0x02);
	_kernel_loadPageDirectory(1023, pd32, 0x02);

	kernel_mapAddress(pd32, pd32, 0x02);
	kernel_mapAddress(pt32, pt32, 0x02);

	kernel_loadPageDirectory(kernel_pageDirectory);
}

u32* kernel_createVirtualAddressSpace(){
	u32* pageDir = kernel_allocatePage();


	if (!pageDir){
		kernel_freePage(pageDir);
		return NULL;
	}


	return pageDir;
}

void kernel_copyPageDirectories(u32* new, u32* old, u32 start, u32 pages){
	u32 pageDir = start >> 22;
	u32 pageTbl = (start >> 12) & 0x3FF;

}

u8 kernel_mapAddress(u32 virtAddr, u32 physAddr, u8 flags){
	if (_kernel_isPagingEnabled())
		return _kernel_mapAddressR(virtAddr, physAddr, flags);
	else
		return _kernel_mapAddressN(virtAddr, physAddr, flags);
}

static u8 _kernel_mapAddressN(u32 virtAddr, u32 physAddr, u8 flags){
	//kernel_printfBOCHS("Normal kernel mapping\n");
	u32 pd = *_kernel_getPageDirR(virtAddr);
	u32* pt = (u32*)(pd & ~(0xFFF));
	pt[(virtAddr >> 12) & 0x3FF] = physAddr | flags | 0x01; 
	return 1;
}

static u8 _kernel_mapAddressR(u32 virtAddr, u32 physAddr, u8 flags){
	u32 pId = (virtAddr >> 22);
	u32 ptId = ((virtAddr >> 12) & 0x3FF);
	u32* pd_p = (u32*)(0xFFFFF000 + ((pId) << 2));
	u32 pd = *pd_p;
	u32* v_new_table = (u32*)(0xFFC00000 + (pId << 12));

	if (!(pd & 1)){
		u32* new_table = kernel_allocatePage();
		if (new_table == NULL) return 0;
		*pd_p = (u32)new_table | 0x01;
	}else{
		u32 t = v_new_table[ptId];
		if ((t & 1) && ((t & ~(0xFFF)) != physAddr)) return 0;
	}

	v_new_table[ptId] = physAddr | flags | 0x01;
	return 1;
}

static void _kernel_unmapAddressR(u32 virtAddr){
	u32 pId = (virtAddr >> 22);
	u32 ptId = ((virtAddr >> 12) & 0x3FF);
	u32* pd_p = (u32*)(0xFFFFF000 + ((pId) << 2));
	u32 pd = *pd_p;
	u32* v_new_table = (u32*)(0xFFC00000 + ((pId << 12)));

	if ((pd & 1) == 1)
		v_new_table[ptId] = 0;
}

static void _kernel_unmapAddressN(u32 virtAddr){
	u32 pd = *_kernel_getPageDirR(virtAddr);
	u32* pt = (u32*)(pd & ~(0xFFF));
	pt[(virtAddr >> 12) & 0x3FF] = 0; 
}

void kernel_unmapAddress(u32 virtAddr){
	//todo, too lazy
	if (_kernel_isPagingEnabled())
		return _kernel_unmapAddressR(virtAddr);
	else
		return _kernel_unmapAddressN(virtAddr);
}

void kernel_unmapIdentity(u32 from, u32 to){
	from = from & ~(0xFFF);
	to = (to + 0x1000) & ~(0xFFF);
	void(*unmappingFunction)(u32) = _kernel_unmapAddressN;
	if (_kernel_isPagingEnabled()){
		unmappingFunction = _kernel_unmapAddressR;
	}

	while (from <= to){
		unmappingFunction(from);
		from += 0x1000;
	}	
}

//for now, I guess I will just take the performance hit
u8 kernel_mapIdentity(u32 from, u32 to, u8 flags){
	from = from & ~(0xFFF);
	to = (to) & ~(0xFFF);
	u8(*mappingFunction)(u32, u32, u8) = _kernel_mapAddressN;
	if (_kernel_isPagingEnabled())
		mappingFunction = _kernel_mapAddressR;

	u32 ofrom = from;
	u8 result = 1;
	while (from <= to){
		if (!mappingFunction(from, from, flags)){
			kernel_unmapIdentity(ofrom, from);
			result = 0;
			break;
		}
		from = from + 0x1000;

	}

	return result;

}

void* kernel_getPhysicalAddress(u32 vAddr){ //todo, make better
	u32 pId = (vAddr >> 22);
	u32 ptId = (vAddr >> 12) & 0x3FF;

	u32 pd = *(u32*)(0xFFFFF000 + (pId << 2));
	if (!(pd & 1)) return NULL;

	u32 pt = *((u32*)(0xFFC00000 + (pId << 12)) + ptId);

	if (!(pt & 1)) return NULL;

	return (void*)((pt & ~(0xFFF)) + (vAddr & 0xFFF));
}

//probably could integrate this somewhere else
/*static u32* _kernel_getPageDir(u32 virtAddr){
	u32 pId = (virtAddr >> 22);

	u32 vaddr = 0xFFC00000 + (pId << 12);

	return (u32*)vaddr;
}*/


static inline u32* _kernel_getPageDirR(u32 virtAddr){
	return &kernel_pageDirectory[(virtAddr >> 22)];
}

static inline void _kernel_loadPageDirectory(u16 pId, u32 addr, u8 flags){
	kernel_pageDirectory[pId] = addr | flags | 0x01;
}

void* kernel_vAllocatePage(u32 addr, u32 pages, u8 flags){

	u32 tempA = addr, tempB = tempA;
	u32 i = 0;
	void* tempPage = NULL;
	for (; i < pages; i++){
		tempPage = kernel_allocatePage();
		//kernel_printfBOCHS("tempPage = %x\n", (u32)tempPage);
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

static u8 _kernel_isPagingEnabled(){
	u32 a = 0;
	__asm__ volatile("mov eax, cr0"
			: "=a" (a));
	return (a >> 31);
}
/*

static u8 _kernel_mapPageTable(u32 ptAddr){
	u32* pt_addr = _kernel_getPageTable(ptAddr);

	if (pt_addr == NULL){

	}
}*/