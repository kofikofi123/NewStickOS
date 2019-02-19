#include "NSOPaging.h"
#include "NSOCoreUtils.h"
#include "NSOBochs.h"

void kernel_initPaging(){
	kernel_printfBOCHS("cpuid enabled: %b", kernel_isCpuidSupported());

}
/*
u8 kernel_reservePage(void*);
u8 kernel_reservePage2(void*, void*);
u8 kernel_releasePage(void*);*/