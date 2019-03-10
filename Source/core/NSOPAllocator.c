#include "DataType.h"
#include "NSOPAllocator.h"
#include "NSOCoreUtils.h"
#include "NSOBiosMaps.h"
#include "NSOBochs.h"

/*
TODO: add restrictions on pages that are marked as unusable by ACPI memory map
*/

u32 _kernel_page_cache[KERNEL_PAGE_CACHE_LIMIT] = {0};
u8 _kernel_page_cache_p = 0, _kernel_page_cache_l = 0;
u8 *_kernel_pBase = NULL, *_kernel_pEnd = NULL;
u32 maximumPageN = 0;
void* _kernel_end = NULL;
extern u32 kernel_end;

static u32 _kernel_getNextFree();
static u8 _kernel_refillPageCache();


void kernel_initPageAllocator(){
	u32 true_end = ((((u32)(&kernel_end)) + 0x1000) & ~(0xFFF));
	_kernel_end = (void*)true_end;

	maximumPageN = ((kernel_getLargestAddr()-true_end)/4096) + 1;
	u32 sizor = maximumPageN * sizeof(u32);
	u32 i = 0;
	
	for (i = 0; i < kernel_getEntryLength(); i++){
		struct kernel_BMM* bmm = kernel_getEntry(i);
		u32 base = (u32)bmm->base;
		u32 end = base + ((u32)bmm->len - 1);

		if (end > true_end && (end - true_end) >= sizor){
			if (base > true_end)
				_kernel_pBase = (u8*)base;
			else
				_kernel_pBase = (u8*)true_end;
			break;
		}
	}
	_kernel_pEnd = (_kernel_pBase + (sizor - 1));

	u32 sp = (true_end / 0x1000);

	//kernel_memset(_kernel_pBase, 0, (u32)(_kernel_pEnd-_kernel_pBase));
	
	for (i = 0; i <= sp; i++){
		*(_kernel_pBase + i) = 0xFF;
	}

	for (; i < maximumPageN; i++){
		*(_kernel_pBase + i) = 0;
	}
	

	_kernel_refillPageCache();
	
	return;
}

void* kernel_allocatePage(){
	void* final_page = NULL;

	/*
	u32 next_page = _kernel_getNextFree();

	if (next_page > 0){
		next_page--;
		*(_kernel_pBase + next_page) = 0xFF;
 		final_page = (void*)(next_page * 0x1000);
	}*/

	if (_kernel_page_cache_p >= _kernel_page_cache_l){

		if (!_kernel_refillPageCache())
			return final_page;

	}

	final_page = (void*)_kernel_page_cache[_kernel_page_cache_p++];

	return final_page;
}
/*
void kernel_markPage(u32 pageAddr){
	pageAddr = pageAddr & ~(0xFFF);
	*(_kernel_pBase + (pageAddr / 0x1000)) = 0xFF;	
}*/

static u8 _kernel_refillPageCache(){
	u32 check = 0;
	u8 i = 0;
	for (; i < KERNEL_PAGE_CACHE_LIMIT; i++){
		check = _kernel_getNextFree();
		if (check == 0) break;
		check--;
		*(_kernel_pBase + check) = 0xF0;
		//kernel_printfBOCHS("%x\n", check * 0x1000);////////////////////////////////////////////////
		_kernel_page_cache[i] = (check) * 0x1000;
	}
	if (i == 0) return 0;
	_kernel_page_cache_l = i + 1;
	_kernel_page_cache_p = 0;

	return 1;
}

void kernel_freePage(void* addr){
	//todo: I guess I should add a check
	u32 addr_u = (u32)addr & ~(0xFFF);

	u32 pageN = addr_u / 0x1000;

	*(_kernel_pBase + pageN) = 0;
}

void kernel_markPage(u32 pageAddr){
	pageAddr = pageAddr & ~(0xFFF);
	*(_kernel_pBase + (pageAddr / 0x1000)) = 0xFF;	
}

void kernel_markPageAddr(void* addr){
	kernel_markPage(((u32)addr & ~(0xFFF))/0x1000);
}

u32 kernel_findPages(u32 pages){
	u32 temp = 0;

	for (u32 i = 0; i < maximumPageN; i++){
		for (u32 j = i; j < i + pages; j++){
			temp = *(_kernel_pBase + j);
			//kernel_printfBOCHS("i: %x (%x)\n", j, temp);
			if (temp != 0)
				break;
		}
		if (temp == 0) return i;
	}
	return 0xFFFFFFFF;
}


static u32 _kernel_getNextFree(){
	u8* temp = _kernel_pBase;
	for (u32 i = 0; i < maximumPageN; i++){
		if (*temp == 0)
			return (temp-_kernel_pBase) + 1;
		temp++;	
	}

	return 0;
}