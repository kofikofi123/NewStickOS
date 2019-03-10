#include "NSOStringUtils.h"


u32 kernel_stringLength(const char* string){
	u32 length = 0;
	char* temp = (char*)string;

	if (string != NULL){
		while (*temp++ != 0) length++;
	}

	return length;
}


u32 kernel_stringOccurance(const char* string, const char character){
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
