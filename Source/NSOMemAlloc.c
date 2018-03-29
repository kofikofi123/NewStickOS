#include "NSOMemMap.h"
#include "NSOMemAlloc.h"
#include "NSOCpuID.h"
#include "NSOUtils.h"

u32 pd[1024] __attribute__((aligned(4096)));
u32 pt[1024] __attribute__((aligned(4096)));





void kernel_initPaging(void){
    for (u16 i = 0; i < 1024; i++){
        pd[i] = 0x02;
        pt[i] = (i * 0x1000) | 0x03;
    }
    
    pd[0] |= ((u32)&(pt)) | 0x01;
    
    kernel_loadPageDirectory(pd);
}

void* kernel_findPhysicalAddress(u32 virtualAddress){
    u16 directory = ((virtualAddress >> 22) & 0x3FF);
    u16 table = ((virtualAddress >> 12) & 0x3FF);
    u16 offset = (virtualAddress & 0xFFF);
    
    
}

void kernel_mapPage(u32 physicalAddress, u32 virtualAddress){
    u16 directory = ((virtualAddress >> 22) & 0x3FF);
    u16 table = ((virtualAddress >> 12) & 0x3FF);
    //u16 offset = (virtualAddress & 0xFFF);
    physicalAddress = (physicalAddress & ~(0xFFF));
    
    u32* c_pd = &pd[directory];
    u32* b_pt = (u32*)(*c_pd & 0xFFFFF000);
    u32* c_pt = &b_pt[table];
    
    *c_pt = (physicalAddress | (*c_pt & 0xFFF));
}


inline void kernel_updatePaging(void){
    __asm__("mov eax, cr3\nmov cr3, eax");
}