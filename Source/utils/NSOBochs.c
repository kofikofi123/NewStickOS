#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOStringUtils.h"

static u8 _kernel_bochs_debug_enabled();


/*
 * Function: kernel_printCharBOCHS
 * --------------------------------
 * Prints a character to the console running bochs and returns a boolean
 *
 * character: An ASCII character to be printed
 *
 *
 * Return: Returns a boolean whether port is assumed to be active with bochs
 */
u8 kernel_printCharBOCHS(const char character){
	if (!_kernel_bochs_debug_enabled())
		return 0;

	kernel_outb(0xE9, character);

	return 1;
}

/*
 * Function: kernel_printStringBOCHS
 * ---------------------------------
 * Prints a string to the console running bochs and returns a boolean
 *
 * string: A NULL-terminated string to be printed
 *
 *
 * Return: Returns a boolean whether port is assumed to be active with bochs
 */
u8 kernel_printStringBOCHS(const char* string){
	if (!_kernel_bochs_debug_enabled() || string == NULL)
		return 0;

	u64 length = kernel_stringLength(string);

	char *sA = (char*)string, *sB = sA + (length - 1);


	while (sA != sB) kernel_outb(0xE9, *sA++);

	return 1;
}

/*
 * Function: kernel_printUNumberBOCHS
 * ---------------------------------
 * Prints a number to the console running bochs and returns a boolean
 *
 * number: An unsigned number to be printed
 *
 *
 * Return: Returns a boolean whether port is assumed to be active with bochs
 */
u8 kernel_printNumberBOCHS(const u32 number){
	if (!_kernel_bochs_debug_enabled()) 
		return 0;

	u32 temp = number;

	while (temp != 0){
		kernel_outb(0xE9, (temp % 10) | 0x30);
		temp = temp / 10;
	}

	return 1;
}

/*
 * Function: kernel_printHexBOCHS
 * ------------------------------
 * Prints a hexidecimal number to the console runnng bochs and returns a boolean
 *
 * number: An number to be printed (in hex form)
 *
 *
 * Return: Returns a boolean whether port is assumed to be active with bochs
 */
u8 kernel_printHexBOCHS(const u32 number){
	if (!kernel_printStringBOCHS("0x"))
		return 0;


	u64 temp = number;
	u64 mask = 0xF0000000;
	u8 nibble = 0, shifts = 28;

	if (number > 0){
		while (mask != 0){
			nibble = (temp & mask) >> shifts;
			if (nibble > 0 && nibble < 10)
				kernel_outb(0xE9, nibble | 0x30);
			else if (nibble >= 10)
				kernel_outb(0xE9, 0x10 + (nibble - 0x0A));
			mask = mask >> 4;
			shifts -= 4;
		}
	}else
		kernel_outb(0xE9, 0x30);

	return 1;
}


/*
 * Function: kernel_printBoolBOCHS
 * -------------------------------
 * Prints a boolean value (true|false) to the console running bochs and returns a boolean
 *
 * boolean: The byte representing the bool to be printed
 *
 *
 * Return: Returns a boolean whether port is assumed to be active with bochs
 * 
 * Note: Tail calls kernel_printStringBOCHS, which should return the same value if implemented in this function
 */
u8 kernel_printBoolBOCHS(const u8 boolean){
	return kernel_printStringBOCHS((boolean == 0 ? "false" : "true"));
}



/*
 * Function: _kernel_bochs_debug_enabled
 * -------------------------------------
 * A in-source function that checks if bochs e9 port hack is enabled
 *
 *
 * Return: Returns a boolean determining if port e9 is active
 */
static u8 _kernel_bochs_debug_enabled(){
	u8 result = 0;
	if (kernel_inb(0xE9) == 0xE9) result = 1;
	return result;
}

