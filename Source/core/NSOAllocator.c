#include "NSOPaging.h"
#include "NSOBiosMaps.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOAllocator.h"

//basic linked list virtual allocator

struct _kernel_AllocNode {
	u32 size;
	struct _kernel_AllocNode* next;
	struct _kernel_AllocNode* prev;
};


struct _kernel_AllocNode head;
struct _kernel_AllocNode* end;


static void* _kernel_allocatorReserveSpace(u32, u8);
static struct _kernel_AllocNode* _kernel_findNearestPrevNode(const struct _kernel_AllocNode*);
static u8 _kernel_extendHEAP(u32);

void kernel_initAllocation(){
	struct _kernel_AllocNode* temp = NULL;
	temp = end = (struct _kernel_AllocNode*)0xC0000000;
	if (!_kernel_extendHEAP(4))
		kernel_panic("Cannot setup kernel heap");

	head.size = 0;
	head.next = temp;
	head.prev = NULL;

	temp->size = (u32)end - 0xC0000000;
	temp->next = end;
	temp->prev = &head;
}


inline void* kernel_malloc(u32 allocSize, u8 alignment){
	return _kernel_allocatorReserveSpace(allocSize, alignment);
}

static void* _kernel_allocatorReserveSpace(u32 allocSize, u8 alignment){
	struct _kernel_AllocNode* currentNode = head.next;

	u32 currentAddr, tempAddr, totalSize, nextAddr;
	u8 alignmentPadding, padding;

	const u32 addingTemp = 6 + allocSize;

	while (currentNode != NULL){
		currentAddr = tempAddr = (u32)currentNode + 6;
		nextAddr = (u32)currentNode->next;
		alignmentPadding = padding = 0;

		if ((currentAddr % alignment) != 0){
			tempAddr = (currentAddr + alignment) & ~(alignment - 1);
			alignmentPadding = (tempAddr - currentAddr);
			currentAddr = tempAddr + alignmentPadding;
		}


		currentAddr += allocSize;

		if ((currentAddr % 4) != 0){
			tempAddr = (currentAddr + 4) & ~(3);
			padding = (tempAddr - currentAddr);
			currentAddr = tempAddr + padding;
		}

		u32 diffAddr = (nextAddr - currentAddr);
		if (diffAddr < sizeof(struct _kernel_AllocNode))
			padding += diffAddr;
		

		totalSize = addingTemp + alignmentPadding + padding;
		if (currentNode->size >= (totalSize))
			break;
		else
			currentNode = currentNode->next;
	}

	void* final = currentNode, *finalTemp = final;
	if (final != NULL){
		struct _kernel_AllocNode* prevNode = currentNode->prev, *nextNode = currentNode->next;

		u32* sizeBlock = (u32*)(final);
		u16* paddingBlock = (u16*)(final + 4 + alignmentPadding);

		*sizeBlock = totalSize;
		
		*paddingBlock = (((u16)(alignmentPadding)) << 8) | padding;

		final = (finalTemp + 6 + alignmentPadding);

		struct _kernel_AllocNode* newNode = (struct _kernel_AllocNode*)(finalTemp + totalSize);
		
		if (newNode < nextNode){
			newNode->next = nextNode;
			newNode->size = (u32)nextNode - (u32)newNode;

			if (nextNode != end)
				nextNode->prev = newNode;

		}
		newNode->prev = prevNode;
		prevNode->next = newNode;
	}

	return final;
}

void* kernel_calloc(u32 allocSize, u8 alignment){
	void* ptr = _kernel_allocatorReserveSpace(allocSize, alignment);

	if (ptr != NULL)
		kernel_memset(ptr, 0, allocSize);

	return ptr;
}

void* kernel_realloc(void* old, u32 allocSize, u8 alignment){
	if (allocSize == 0) return NULL;

	void* new = _kernel_allocatorReserveSpace(allocSize, alignment);
	if (new == NULL) return NULL;

	if (!old)
		kernel_memcpy(new, old, allocSize);

	kernel_free(old);
	return new;
}

void kernel_free(void* ptr){
	if (ptr == NULL) return;
	if (ptr < (void*)0xC0000000 || ptr >= (void*)end) return;

	u32* sizeBlock = NULL;
	u16* paddingBlock = (u16*)(ptr) - 1;

	u32 totalSize = 0;
	u16 tempPadding = *paddingBlock;

	u8 alignmentPadding = (tempPadding >> 8);

	sizeBlock = (u32*)(ptr - 6 - alignmentPadding);
	totalSize = *sizeBlock;

	struct _kernel_AllocNode* node = (struct _kernel_AllocNode*)sizeBlock;
	struct _kernel_AllocNode* prevNode = _kernel_findNearestPrevNode(node);
	struct _kernel_AllocNode* nextNode = prevNode->next;

	node->size = totalSize;
	node->prev = prevNode;
	node->next = nextNode;


	if (nextNode != end)
		nextNode->prev = node;
	prevNode->next = node;


}

u32 kernel_allocatorSize(){
	struct _kernel_AllocNode* current = head.next;
	u32 size = 0;

	while (current != NULL){
		size += current->size;
		current = current->next;
	}

	return size;
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



static struct _kernel_AllocNode* _kernel_findNearestPrevNode(const struct _kernel_AllocNode* other){
	struct _kernel_AllocNode* current = head.next, *temp = &head;

	while (current != NULL){
		if (current < other)
			temp = current;
		else
			break;
		current = current->next;
	}

	return temp;
}

static u8 _kernel_extendHEAP(u32 pages){
	u32 temp = ((u32)end);

	u32* tempA = kernel_vAllocatePage(temp, pages, 0x02);

	//kernel_printfBOCHS("Allocated: %x\n", (u32)tempA);

	if (tempA == NULL) return 0;

	end = (void*)tempA + (pages << 12);

	head.size = ((u32)end - 0xC0000000);

	return 1;
}