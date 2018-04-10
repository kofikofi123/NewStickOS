#include "NSOString.h"

u32 kernel_stringLength(const char* source){
    u32 temp = 0;
    
    while (*source++ != 0)
        temp++;
        
    return temp;
}

u8 kernel_stringCompare(const char* sourceA, const char* sourceB){
    u32 length = kernel_stringLength(sourceA);
    
    return kernel_stringCompareRaw(sourceA, sourceB, length);
}


u8 kernel_stringCompareRaw(const char* sourceA, const char* sourceB, u32 length){
    while (length-- > 0){
        if (*sourceA++ != *sourceB++)
            return 0;
    }
    
    return 1;
}