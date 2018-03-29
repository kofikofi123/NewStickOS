#include "DataType.h"
#include "NSOUtils.h" //temporary
#include "NSODescriptors.h"
#include "NSOCpuID.h"
#include "NSOAPIC.h"
#include "NSOMsr.h"
#include "NSOMemMap.h"
#include "NSOMemAlloc.h"

void kernel_test(void);

void kernel_main(void){
    kernel_cli();

    kernel_initExceptions();
    kernel_initMemoryMap();
    {//vvvvvv MemMap stuff vvvvvv
        //Order memory map
        u32 bmm_size = sizeof(struct BIOSMemoryMap);
        struct BIOSMemoryMap map_temp_hold;
        struct BIOSMemoryMap *map_ptr_a, *map_ptr_b;
        u32 entries = *(u32*)(0x500);
        u32 i, j;
        for (i = 0; i < entries; i++){
            map_ptr_a = kernel_getMemEntry(i);
            for (j = i + 1; j < entries; j++){
                map_ptr_b = kernel_getMemEntry(j);
                
                if (map_ptr_a->base > map_ptr_b->base){
                    kernel_memcpy(map_ptr_a, &map_temp_hold, bmm_size);
                    kernel_memcpy(map_ptr_b, map_ptr_a, bmm_size);
                    kernel_memcpy(&map_temp_hold, map_ptr_b, bmm_size);
                }
            }  
        }
        /*
        //Fix overlaps and combine 
        u64 temp = entries - 1;
        u8 isOdd = (entries % 2);
        for (i = 0; i < entries; i++){
            if (isOdd && (i == temp))
                break;
            map_ptr_a = kernel_getMemEntry(i);
            map_ptr_b = kernel_getMemEntry(i + 1);
            
        }*/
    }//^^^^^^ MemMap stuff ^^^^^^
    
    kernel_initPaging();
    kernel_updatePaging();
    kernel_enablePaging();
    
    {
        //testing
        kernel_mapPage(0x500, 0x2000);
        kernel_updatePaging();
        
        void* addr = kernel_findPhysicalAddress(0x2530);
        
        __asm__("mov eax, %0"
                :
                : "r" ((u32)addr));
    }
    while(1);
}
