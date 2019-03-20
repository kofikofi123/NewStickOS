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

u8 kernel_stringCompare(const char* sA, const char* sB){
	u32 len = kernel_stringLength(sA);

	if (len != kernel_stringLength(sB)) return 0;

	return kernel_stringCompareRAW(sA, sB, len);
}

u8 kernel_stringCompareRAW(const char* sA, const char* sB, u32 raw){
	char* a = (char*)sA;
	char* b = (char*)sB;

	char* c = a + raw;

	while (a != c){
		if (*a++ != *b++) 
			return 0;
	}

	return 1;
}