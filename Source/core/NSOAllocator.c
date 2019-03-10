#include "NSOPaging.h"
#include "NSOBiosMaps.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"

struct _kernel_AllocNode {
	u32 size;
	struct _kernel_AllocNode* next;
	struct _kernel_AllocNode* prev;
};


struct _kernel_AllocNode head;
struct _kernel_AllocNode* end;

static u8 _kernel_extendHEAP(u32);
static struct _kernel_AllocNode* _kernel_allocatorFindSpace(u32, u8, u32*, u32*);

void kernel_initAllocation(){
	struct _kernel_AllocNode* temp = NULL;
	temp = end = (struct _kernel_AllocNode*)0xC0000000;
	if (!_kernel_extendHEAP(4))
		kernel_panic("Cannot setup kernel heap");

	head.size = (u32)end - 0xC0000000;
	head.next = temp;
	head.prev = NULL;

	temp->size = head.size;
	temp->next = end;
	temp->prev = &head;

}


void* kernel_malloc(u32 size, u8 alignment){
	u32 paddingSize = 0;
	u32 residueBytes = 0;

	struct _kernel_AllocNode* node = _kernel_allocatorFindSpace(size, alignment, &paddingSize, &residueBytes);

	if (node == NULL) return NULL;

	struct _kernel_AllocNode* oldPrev = node->prev, *oldNext = node->next;
	//u32 oldSize = node->size;

	u32 total_size = paddingSize + size + residueBytes;

	u32* sizor = (void*)node + residueBytes;

	*sizor++ = 0x80000000 | total_size;

	void* final = (void*)(sizor++);

	struct _kernel_AllocNode* node2 = (void*)node + total_size;

	if (node2 < oldNext){
		node2->size = (end - node2);
		node2->next = oldNext;
		node2->prev = oldPrev;
	}

	if (oldNext != end)	
		oldNext->prev = node2;
	oldPrev->next = node2;

	return final;
}

void kernel_free(void* ptr){
	if (ptr == NULL) return;

	if ((u32)ptr < 0xC0000000) return;

	u32* temp = ((u32*)ptr) - 1;

	u32 size = *temp;

	if ((size & 0x80000000) != 0x80000000) return;

	size = size & ~(0x80000000);

	kernel_printfBOCHS("SIze: %x\n", size);
}

static struct _kernel_AllocNode* _kernel_allocatorFindSpace(u32 allocSize, u8 alignment, u32* paddingSize, u32* residueBytes){
	struct _kernel_AllocNode* node = head.next;

	u32 addr = 0, addr2 = 0;
	u32 tal = 0, lat = 0;
	while (node != end){
		addr = ((u32)node + 4);

		if ((addr % alignment) != 0){
			addr2 = (addr + alignment) & ~(alignment - 1);
			tal = (addr2 - addr);
			addr = addr2;
		}

		if ((addr % 4) != 0){
			addr2 = (addr + 4) & ~(3);
			lat = (addr2 - addr);
		}


		if ((node->size + tal) >= allocSize){
			*residueBytes = tal;
			*paddingSize = lat;
			return node;
		}
		node = node->next;
	}
	return NULL;
}

static u8 _kernel_extendHEAP(u32 pages){
	u32 temp = ((u32)end);

	u32* tempA = kernel_vAllocatePage(temp, pages, 0x02);

	if (tempA == NULL) return 0;

	end = (void*)tempA + (pages << 12);

	head.size = ((u32)end - 0xC0000000);

	return 1;
}