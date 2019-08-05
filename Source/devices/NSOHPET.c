#include "NSOHPET.h"
#include "NSOACPI.h"
#include "NSOAllocator.h"
#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOPaging.h"

#define _KERNEL_CONVTIMERREG(t) (t << 5) + 0x100

struct _kernel_timerInfo {
	u8 timerBlock;
	u8 timer;


	struct _kernel_timerInfo* next;
};

u32* _kernel_hpetAddrs = NULL;
u32 _kernel_numTimers = 0; //num of timer blocks
struct _kernel_timerInfo _kernel_tiHead = {
	.timerBlock = 0xFF,
	.timer = 0xFF,
	.next = NULL
};

static u8 _kernel_numOfUsedTimers();
static struct _kernel_timerInfo* _kernel_getLastTimer();
//static void _kernel_appendTimer(struct _kernel_timerInfo*);
static u8 _kernel_getNumTimerInBlock(u8);

static u8 _kernel_timersAreConsumed();

static u32 _kernel_readBlock(u8, u16);
static void _kernel_writeBlock(u8, u16, u32);


void kernel_initHPET(){
	ACPI_TABLE_HEADER* hpetHeader = NULL;

	while (!ACPI_FAILURE(AcpiGetTable("HPET", (_kernel_numTimers + 1), &hpetHeader))){
		_kernel_hpetAddrs = (u32*)kernel_realloc(_kernel_hpetAddrs, sizeof(u32), 4);

		if (_kernel_hpetAddrs == NULL) kernel_panic("Not enough memory");

		u8* hpetIndex = (u8*)hpetHeader + sizeof(ACPI_TABLE_HEADER);

		kernel_printfBOCHS("hpetIndex: %x\n", hpetIndex);

		struct kernel_AcpiGAS* gas = (struct kernel_AcpiGAS*)(hpetIndex + 4);
		u32 address = gas->Address;

		_kernel_hpetAddrs[_kernel_numTimers++] = address;

		kernel_mapAddress(address, address, 0x02);
	}


}

u8 kernel_getNumOfTimers(){
	u8 num = 0;
	u32 temp = 0;
	for (u8 i = 0; i < _kernel_numTimers; i++){
		temp = _kernel_readBlock(i, 0);
		num = num + ((temp >> 8) & 0x1F) + 1;
	}

	return num;
}

inline u32 kernel_getTimerFreq(u8 block){
	return _kernel_readBlock(block, 1);
}

u8 kernel_setupTimer(u32 compTick){ //for  now
	if (_kernel_timersAreConsumed()) return 0;
	struct _kernel_timerInfo* info = _kernel_getLastTimer();

	u8 block = 0, timer = 0;
	if (info != NULL){
		block = info->timerBlock;
		timer = info->timer + 1;
	}

	if (timer == 32){
		block++;
		timer = 0;
	}

	u16 offset = _KERNEL_CONVTIMERREG(timer);

	_kernel_writeBlock(block, offset + 0x08, compTick);
	_kernel_writeBlock(block, offset, (1 << 2) | (1 << 3) | (1 << 8));

	return timer;//fornow
}

static u32 _kernel_readBlock(u8 block, u16 offset){
	u32* tblock = (u32*)_kernel_hpetAddrs[block];

	return tblock[offset];
}

static void _kernel_writeBlock(u8 block, u16 offset, u32 value){
	u32* tblock = (u32*)_kernel_hpetAddrs[block];

	tblock[offset] = value;
}

static u8 _kernel_numOfUsedTimers(){
	u8 num = 0;
	struct _kernel_timerInfo* timers = _kernel_tiHead.next;

	while (timers != NULL)
		num++;

	return num;
}

static struct _kernel_timerInfo* _kernel_getLastTimer(){
	struct _kernel_timerInfo* timers = _kernel_tiHead.next;

	if (timers != NULL){
		while (timers->next != NULL){
			timers = timers->next;
		}
	}
	
	return timers;
}

/*static void _kernel_appendTimer(struct _kernel_timerInfo* timer){
	struct _kernel_timerInfo* base = _kernel_getLastTimer();

	if (base == NULL)
		base = &_kernel_tiHead;

	base->next = timer;
}*/

static u8 _kernel_timersAreConsumed(){
	return !(kernel_getNumOfTimers() - _kernel_numOfUsedTimers()); 
}
