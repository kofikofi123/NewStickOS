#include "NSOStringUtils.h"

/*
 * Function: kernel_stringLength
 * -----------------------------
 * Calculates the length of a NULL-terminated string and returns a number
 *
 * string: The NULL-terminated string to be calculated
 *
 *
 * Return: Returns a unsigned 32 bit number representing the length of the string
 */
u32 kernel_stringLength(const char* string){
	u32 length = 0;
	char* temp = (char*)string;

	if (string != NULL){
		while (*temp++ != 0) length++;
	}

	return length;
}