#include "NSOBochs.h"
#include "NSOString.h"
#include "NSOUtils.h"

static u8 kernel_checkDebugBOCHS(void);

void kernel_printCharBOCHS(char character){
	if (!kernel_checkDebugBOCHS())
		return;
	

	kernel_outB(0xE9, character);
}

void kernel_printStringBOCHS(const char* string){
	if (!kernel_checkDebugBOCHS())
		return;

	char* tempA = (char*)string;
	char temp;
	while ((temp = *tempA++) != 0)
		kernel_outB(0xE9, temp);
}



static u8 kernel_checkDebugBOCHS(void){
	return (kernel_inB(0xE9) == 0xE9); 
}
