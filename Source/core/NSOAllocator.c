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
static struct _kernel_AllocNode* _kernel_allocatorFindSpace(u32, u8, u8*, u8*);
static struct _kernel_AllocNode* _kernel_findNearestNode(struct _kernel_AllocNode*);
//static struct _kernel_AllocNode* _kernel_combineNodes(struct _kernel_AllocNode*);

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
	u8 paddingSize = 0;
	u8 residueBytes = 0;

	struct _kernel_AllocNode* node = _kernel_allocatorFindSpace(size, alignment, &paddingSize, &residueBytes);

	if (node == NULL) return NULL;

	struct _kernel_AllocNode* oldPrev = node->prev, *oldNext = node->next;
	//u32 oldSize = node->size;

	u32 total_size = paddingSize + size + residueBytes + 6;


	u32* sizor = (u32*)node;

	*sizor++ = 0x80000000 | size;

	u16* paddingB = (void*)sizor + residueBytes;

	*paddingB++ = ((u16)residueBytes << 8) | paddingSize;

	void* final = (void*)paddingB;

	struct _kernel_AllocNode* node2 = (void*)node + total_size;


	if (node2 < oldNext){
		node2->next = oldNext;
		node2->prev = oldPrev;
	}

	if (oldNext != end)	
		oldNext->prev = node2;
	node2->size = ((u32)end - (u32)node2);
	oldPrev->next = node2;

	return final;
}

void kernel_free(void* ptr){
	if (ptr == NULL) return;

	if ((u32)ptr < 0xC0000000) return;

	u16* vpr = ((u16*)ptr) - 1;

	u16 pr = *vpr;

	u8 residueBytes = (pr >> 8);
	u8 paddingBytes = (pr & 0xFF);

	u32* header = (void*)vpr - residueBytes - 4;
	u32 allocSize = *header;

	if ((allocSize & 0x80000000) != 0x80000000) {
		return;
	}

	allocSize = allocSize & ~(0x80000000);

	u32 totalSize = allocSize + residueBytes + paddingBytes + 6;

	struct _kernel_AllocNode* newNode = (struct _kernel_AllocNode*)header, *prevNode = NULL, *nextNode = NULL;

	newNode->size = totalSize;

	prevNode = _kernel_findNearestNode(newNode);

	nextNode = prevNode->next;
	
	newNode->prev = prevNode;
	newNode->next = nextNode;

	prevNode->next = newNode;

	if (nextNode != NULL && nextNode != end)
		nextNode->prev = newNode;
}

void kernel_debugAllocator(){
	struct _kernel_AllocNode* curNode = head.next, *nextNode = NULL;
	kernel_printStringBOCHS("\nStart\n");
	while (curNode != NULL && curNode != end){
		nextNode = curNode->next;
		kernel_printfBOCHS("{\n\tAddr: %x\n\tSize: %x\n\tNext: %x\n}\n", (u32)curNode, curNode->size, (u32)nextNode);
		curNode = nextNode;
	}
	kernel_printStringBOCHS("End\n");
}

static struct _kernel_AllocNode* _kernel_allocatorFindSpace(u32 allocSize, u8 alignment, u8* paddingSize, u8* residueBytes){
	struct _kernel_AllocNode* node = head.next;

	u32 cAddr, addr = 0, addr2 = 0;
	u8 tal = 0, lat = 0;
	//u32 ftal = 0;
	u32 sano = sizeof(struct _kernel_AllocNode);
	while (node != end){
		cAddr = ((u32)node);
		addr = (cAddr + 6);

		if ((addr % alignment) != 0){
			addr2 = (addr + alignment) & ~(alignment - 1);
			tal = (addr2 - addr);
			addr = addr2;
		}

		addr += allocSize;

		u32 temp = (addr - cAddr);
		if (temp < sano)
			lat = (sano - temp);

		if ((addr % 4) != 0){
			addr2 = (addr + 4) & ~(3);
			lat += (addr2 - addr);
		}
		if ((node->size) >= (allocSize + tal + lat + 6)){
			*residueBytes = tal;
			*paddingSize = lat;
			return node;
		}
		node = node->next;
	}
	return NULL;
}

static struct _kernel_AllocNode* _kernel_findNearestNode(struct _kernel_AllocNode* node){
	struct _kernel_AllocNode* tempA = head.next, *tempB = &head;

	while (tempA != NULL){
		if (tempA > node)
			break;
		tempB = tempA;
		tempA = tempA->next;
	}

	return tempB;
}

static u8 _kernel_extendHEAP(u32 pages){
	u32 temp = ((u32)end);

	u32* tempA = kernel_vAllocatePage(temp, pages, 0x02);

	if (tempA == NULL) return 0;

	end = (void*)tempA + (pages << 12);

	head.size = ((u32)end - 0xC0000000);

	return 1;
}

/*static struct _kernel_AllocNode* _kernel_combineNodes(struct _kernel_AllocNode* node){

}*/