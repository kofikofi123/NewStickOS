#include "NSOStringUtils.h"

/*
 * Function: kernel_stringLength
 * -----------------------------
 * Calculates the length of a NULL-terminated string and returns a number
 *
 * string: The NULL-terminated ASCII string to be calculated
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

/*
 * Function: kernel_stringCount
 * ----------------------------
 * Count the number of occurance of an ASCII character in an ASCII string and returns a number
 *
 * string: The NULL-terminated ASCII string to scan
 * character: The ASCII character to look for
 *
 *
 * Return: Returns a unsigned 32 bit number representing the number of occurances of {character}
 */
u32 kernel_stringCount(const char* string, const char character){
	u32 count = 0;
	u32 length = 0;

	if (string != NULL){
		length = kernel_stringLength(string);

		char *a = (char*)string, *b = a + (length - 1);

		while (a != b)
			if (*a++ == character) count++;
	}

	return count;
}