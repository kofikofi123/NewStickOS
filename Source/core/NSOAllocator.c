#include "NSOAllocator.h"
#include "NSOBiosMaps.h"
#include "NSOBochs.h"

/*
#define KERNEL_AU_PAGE 0xA0
#define KERNEL_AU_MEM 0xA1
#define KERNEL_AU_FREEC 0x01ABBEEF
#define KERNEL_AU_USEDC 0xDEAD9128

extern u32 kernel_begin;
extern u32 kernel_end;

struct _kernel_AllocHeader {
	u32 size;
	u32 isFree;
};

struct _kernel_AllocInfo {
	struct _kernel_AllocHeader header;
	struct _kernel_AllocInfo* next;
	struct _kernel_AllocInfo* prev;
};

struct _kernel_AllocInfo head;
struct _kernel_AllocInfo* end;

static struct _kernel_AllocInfo* _kernel_findSpace(u32);
static struct _kernel_AllocInfo* _kernel_findNearestNode(struct _kernel_AllocInfo*);
static u32 _kernel_calculatePaddingSize(u32);
static void _kernel_allocCombine(struct _kernel_AllocInfo*);
static void _kernel_allocCombineGlobal();


void kernel_init_allocation(){
	u32 k_end = (u32)&kernel_end;

	struct _kernel_AllocInfo* info = (struct _kernel_AllocInfo*)((k_end + 0x1000) & ~(0xFFF));

	struct kernel_BMM* map = kernel_lookupMemMapB(info);

	end = (struct _kernel_AllocInfo*)(((u32)map->len - 1));

	info->header.size = ((u32)((char*)end - (char*)info)) + 1;
	info->header.isFree = KERNEL_AU_FREEC;

	head.next = info;
	head.prev = NULL;
	
	info->prev = &head;
	info->next = NULL;
}


void* kernel_malloc(u32 size){
	u32 inc_size = size + sizeof(struct _kernel_AllocHeader);
	u32 final_size = _kernel_calculatePaddingSize(inc_size);
	struct _kernel_AllocInfo* info = _kernel_findSpace(final_size);
	void* final = NULL;
	if (info != NULL){
		struct _kernel_AllocHeader* temp = (struct _kernel_AllocHeader*)info;
		final = (temp + 1);
		{
			struct _kernel_AllocInfo* p_info = info->prev, *n_info = info->next;

			if (n_info == NULL){
				n_info = (struct _kernel_AllocInfo*)((char*)info + final_size);
				
				if (n_info > end){
					p_info->next = NULL;
				}else{
					n_info->header.size = ((u32)((char*)end - (char*)n_info)) + 1;
					n_info->header.isFree = KERNEL_AU_FREEC;
					n_info->prev = p_info;
					n_info->next = NULL;
					p_info->next = n_info;
				}
			}else{
				p_info->header.size = ((u32)((char*)n_info - (char*)p_info)) + 1;
				p_info->header.isFree = KERNEL_AU_FREEC;
				p_info->next = n_info;
				n_info->prev = p_info;
			}
		}
		
		temp->size = final_size;
		temp->isFree = KERNEL_AU_USEDC;
	}

	return final;
}


void kernel_free(void* ptr){
	if (ptr == NULL) return;

	struct _kernel_AllocHeader* header = ((struct _kernel_AllocHeader*)ptr);
	header--;

	if (header->isFree != KERNEL_AU_USEDC) return;

	u32 new_size = header->size;

	struct _kernel_AllocInfo* info = (struct _kernel_AllocInfo*)header;

	info->header.size = new_size;
	info->header.isFree = KERNEL_AU_FREEC;

	struct _kernel_AllocInfo* nearest = _kernel_findNearestNode(info);
	struct _kernel_AllocInfo* nn = nearest->next;

	nearest->next = info;
	info->prev = nearest;

	if (nn != NULL){
		nn->prev = nearest;
		info->next = nn;
	}

	_kernel_allocCombineGlobal();
}


void kernel_alloc_debug(){
	struct _kernel_AllocInfo* temp = head.next;

	kernel_printCharBOCHS('{');
	while (temp != NULL){
		kernel_printfBOCHS("\n\tBase: %x\n\tSize: %x\n\tNext: %x\n", (u32)temp, (u32)temp->header.size, (u32)temp->next);
		temp = temp->next;
	}
	kernel_printStringBOCHS("}\n");
}

static struct _kernel_AllocInfo* _kernel_findNearestNode(struct _kernel_AllocInfo* info){
	struct _kernel_AllocInfo* ft = &head, *temp = ft->next;

	while (temp != NULL){
		if (temp < info)
			ft = temp;
		else
			break;
		temp = temp->next;
	}

	return ft;
} 

static struct _kernel_AllocInfo* _kernel_allocFindNext(struct _kernel_AllocInfo* info){
	struct _kernel_AllocInfo* temp = head->next;

	while (temp != NULL){

	}
}

static void _kernel_allocCombineGlobal(){
	struct _kernel_AllocInfo* info = head.next, *temp = NULL;
	char* tempA = NULL, *tempB = NULL, *tempC = NULL;


	while (info->next != NULL){
		temp = info->next;
		tempA = (char*)info;
		tempB = tempA + (info->header.size);

		tempC = (char*)temp + (temp->header.size - 1);
		if (tempB >= (char*)temp){
			info->header.size = (tempC - tempA) + 1;
			info->next = temp->next;
			temp->next->prev = info;
		}else{
			info = temp;
		}
	}
	
}

static struct _kernel_AllocInfo* _kernel_findSpace(u32 size){
	struct _kernel_AllocInfo* info = head.next;

	while (info != NULL){
		if (info->header.size >= size)
			return info;
		info = info->next;
	}

	return NULL;
}

static u32 _kernel_calculatePaddingSize(u32 size){
	return (size + 0x04) & ~(0x03);
}*/