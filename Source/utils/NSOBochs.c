#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOStringUtils.h"

static u8 _kernel_bochs_debug_enabled();
static u32  _kernel_bochs_find_base(const u32);

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
u8 __attribute__((cdecl)) kernel_printCharBOCHS(const char character){
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
u8 __attribute__((cdecl)) kernel_printStringBOCHS(const char* string){
	if (!_kernel_bochs_debug_enabled() || string == NULL)
		return 0;

	u64 length = kernel_stringLength(string);

	char *sA = (char*)string, *sB = sA + (length);


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
u8 __attribute__((cdecl)) kernel_printUNumberBOCHS(const u32 number){
	if (!_kernel_bochs_debug_enabled()) 
		return 0;

	u32 base = _kernel_bochs_find_base(number);
	u32 temp = number;

	if (number == 0)
		kernel_outb(0xE9, 0x30);
	else {
		while (base != 0){
			kernel_outb(0xE9, (temp / base) | 0x30);
			temp = temp % base;
			base = base / 10;
		}
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
u8 __attribute__((cdecl)) kernel_printHexBOCHS(const u32 number){
	if (!kernel_printStringBOCHS("0x"))
		return 0;


	u64 temp = number;
	u64 mask = 0xF0000000;
	u8 nibble = 0, shifts = 28;
	u8 zero_spam = 0;

	if (number > 0){
		while (mask != 0){
			nibble = (temp & mask) >> shifts;
			if ((nibble > 0 && nibble < 10) || (nibble == 0 && zero_spam == 1)){
				zero_spam = 1;
				kernel_outb(0xE9, nibble | 0x30);
			}
			else if (nibble >= 10){
				zero_spam = 1;
				kernel_outb(0xE9, 0x61 + (nibble - 0x0A));
			}
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
u8 __attribute__((cdecl)) kernel_printBoolBOCHS(const u8 boolean){
	return kernel_printStringBOCHS((boolean == 0 ? "false" : "true"));
}

/*
 * Function: kernel_printfBOCHS
 * ----------------------------
 * Prints a formated string to  the console running bochs and returns a boolean
 *
 * string: The string to be formatted for printing
 * ...: The list of items to be formatted
 *
 *
 * Return: Returns a boolean whether port is assumed to be active with bochs
 * 
  * Note: Will probably be the slowest function because of the checks in other functions
 */
u8 __attribute__((cdecl)) kernel_printfBOCHS(const char* string, ...){
	if (!_kernel_bochs_debug_enabled() || string == NULL) return 0;

	//u32 max_expect = kernel_stringCount(string, '%'); /*Not all % are for formating*/
	u32* ebp_base = NULL;

	kernel_va_start(ebp_base);

	u32 length = kernel_stringLength(string);

	char *a = (char*)string, *b = a + (length);
	char current = 0;
	while (a != b){
		current = *a;
		if (current == '%'){
			current = *++a;
			if (current == 'd')
				kernel_printUNumberBOCHS(*ebp_base++);
			else if (current == 'c')
				kernel_printCharBOCHS((char)(*ebp_base++));
			else if (current == 'x')
				kernel_printHexBOCHS(*ebp_base++);
			else if (current == 's')
				kernel_printStringBOCHS((const char*)(*ebp_base++));
			else if (current == 'b')
				kernel_printBoolBOCHS((u8)(*ebp_base++));
			else
				kernel_printCharBOCHS(current);
		}else
			kernel_printCharBOCHS(current);
		a++;
	}

	return 1;
}



/*
 * Function: _kernel_bochs_debug_enabled
 * -------------------------------------
 * A in-source function that checks if bochs e9 port hack is enabled
 *
 *
 * Return: Returns a boolean determining if port e9 is active
 */
static u8 __attribute__((cdecl)) _kernel_bochs_debug_enabled(){
	u8 result = 0;
	if (kernel_inb(0xE9) == 0xE9) result = 1;
	return result;
}


/*
 * Function: _kernel_bochs_find_base
 * ---------------------------------
 * A function to determine the scientific notation and returns a number
 *
 * number: The number used to determine the scientific notation
 *
 *
 * Return: Returns 10^x
 */
static u32 __attribute__((cdecl)) _kernel_bochs_find_base(const u32 number){
	u32 base = 1;

	while ((number / (base)) != 0){
		base = base * 10;
	}
	return base/10;
}