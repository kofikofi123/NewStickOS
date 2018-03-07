#include "NSOUtils.h" //temporary
#include "NSODescriptors.h"
#include "DataType.h"

void kernel_test(void);

void kernel_main(void){
    kernel_cli();
    
    kernel_initExceptions();
    
    {
        __asm__("int 0x03");
    }
    
    while(1);
}
