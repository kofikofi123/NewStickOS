#ifndef KERNEL_BOCHS_H
#define KERNEL_BOCHS_H

#include "DataType.h"

u8 kernel_printCharBOCHS(const char);
u8 kernel_printStringBOCHS(const char*);
u8 kernel_printUNumberBOCHS(const u32);
u8 kernel_printHexBOCHS(const u32);
u8 kernel_printBoolBOCHS(const u8);
u8 kernel_printfBOCHS(const char*, ...);
void kernel_breakBOCHS();

#endif