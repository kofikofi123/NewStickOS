#include "NSOBochs.h"
#include "NSOCoreUtils.h"
#include "NSOStringUtils.h"
#include <stdarg.h>

static u8 _kernel_bochs_debug_enabled();
static u32 _kernel_bochs_get_digit(u32);
static u32 _kernel_bochs_get_nibble(u32);
static u32 _kernel_bochs_pow(u32, u8);


static void _kernel_printCharBOCHS(char);
static void _kernel_printStringBOCHS(const char*, u32);
static void _kernel_printNumberBOCHS(s32, u32, char, u32, u8, u8);
static void _kernel_printUNumberBOCHS(u32, u32, char, u32);
static void _kernel_printHexBOCHS(u32, u32, char, u32, u8);
static void _kernel_printBoolBOCHS(u8);
static void _kernel_repPrintCharBOCHS(char, u32);
static u32 _kernel_interpretNumber(va_list*, u8, char);


u8 kernel_printfBOCHS(const char* string, ...){
	u8 result = 0;
	va_list list;
	va_start(list, string);
	result = kernel_vprintfBOCHS(string, list);
	va_end(list);
	return result;
}

u8 kernel_vprintfBOCHS(const char* string, va_list list){
	if (!_kernel_bochs_debug_enabled() || string == NULL) return 0;

	u32 length = kernel_stringLength(string);

	char *a = (char*)string, *b = a + (length);
	char current = 0;


	while (a <= b){
		u8 isRightAlign = 1;
		u8 showAllSigns = 0;
		u8 hashPrefix = 0;
		u8 blank = 0;
		char paddingChar = ' ';

		u32 width = 0;
		u32 precision = 0;

		u8 lengthType = 0;

		current = *a;
		if (*a == '%'){
			current = *++a;

			//flags
			while (1){
				if (current == '#')
					hashPrefix = 1;
				else if (current == ' ')
					blank = 1; 
				else if (current == '0')
					paddingChar = '0';
				else if (current == '-')
					isRightAlign = 0;
				else if (current == '+')
					showAllSigns = 1;
				else
					break;
				current = *++a;
			}


			//width
			if (current == '*')
				width = va_arg(list, u32);
			else {
				while (current >= '0' && current <= '9'){
					u8 num = (current - 0x30);
					width = (width * 10) + num;
					current = *++a;
				}
			}

			//precision
			if (current == '.'){
				current = *++a;
				if (current == '*')
					precision = va_arg(list, u32);
				else {
					while (current >= '0' && current <= '9'){
						u8 num = (current - 0x30);
						precision = (precision * 10) + num;
						current = *++a;
					}
				}
			}

			switch (current){
				case 'h': {
					current = *++a;

					if (current == 'h'){
						current = *++a;
						lengthType = 1;
					}
					else
						lengthType = 2;

					break;
				}
			}

			if (blank && !showAllSigns){
				_kernel_printCharBOCHS(' ');
				width--;
			}

			
			switch (current){
				case 'i':
				case 'd': {
					s32 num = (s32)_kernel_interpretNumber(&list, lengthType, 'd');
					_kernel_printNumberBOCHS(num, width, paddingChar, precision, hashPrefix, showAllSigns);
					break;
				}
				case 'u' : {
					u32 num = _kernel_interpretNumber(&list, lengthType, 'u');
					_kernel_printUNumberBOCHS(num, width, paddingChar, precision);
					break;
				}
				case 'X':
				case 'x': {
					u32 num = _kernel_interpretNumber(&list, lengthType, 'u');
					_kernel_printHexBOCHS(num, width, paddingChar, precision, hashPrefix);
					break;
				}
				case 'C':
				case 'c': {
					_kernel_printCharBOCHS((char)va_arg(list, int));
					break;
				}
				case 'S':
				case 's': { 
					_kernel_printStringBOCHS(va_arg(list, const char*), precision);
					break;
				}
				case 'B':
				case 'b': {
					_kernel_printBoolBOCHS((u8)va_arg(list, int));
					break;
				}
				case '%': {
					_kernel_printCharBOCHS('%');
					break;
				}
			}
		}else
			_kernel_printCharBOCHS(current);
		a++;
	}

	return 1;
}

//static void _kernel_handlePrePadding(u8 blank, u8 showSigns){}
static u32 _kernel_interpretNumber(va_list* list, u8 lengthType, char prefix){

	u32 ret = 0;
	switch (prefix){
		case 'd': {
			switch (lengthType) {
				case 1:
					ret = (u8)va_arg(*list, u32);
					break;
				case 2:
					ret = (s16)va_arg(*list, s32);
					break;
				case 3:
					ret = (s32)va_arg(*list, s64); //trauncated
					break;
				default:
					ret = (u32)va_arg(*list, u32);

			}
			break;
		}
		case 'u': {
			ret = va_arg(*list, u32);
			//for now
			break;
		}
	}

	return ret;
}

static inline void _kernel_printCharBOCHS(char character){
	kernel_out8(0xE9, character);
}

static inline void _kernel_printStringBOCHS(const char* string, u32 precision){
	if (string == NULL) string="NULL";
	u64 length = precision == 0 ? kernel_stringLength(string) : precision;
	char *sA = (char*)string, *sB = sA + (length);
	while (sA != sB) kernel_out8(0xE9, *sA++);
}

static void _kernel_printNumberBOCHS(s32 num, u32 width, char paddingChar, u32 precision, u8 hashPrefix, u8 showSigns){
	u8 sign = num >> 31;

	if (sign){
		_kernel_printCharBOCHS('-');
		num = ~num + 1;
		width = 0;
	}else if (showSigns){
		_kernel_printCharBOCHS('+');
		width = 0;
	}


	_kernel_printUNumberBOCHS(num, width, paddingChar, precision);
}

static void _kernel_printUNumberBOCHS(u32 number, u32 width, char paddingChar, u32 precision){
	u32 digits = _kernel_bochs_get_digit(number);
	u32 final_digits = digits > precision ? digits : precision;
	u32 base = _kernel_bochs_pow(10,final_digits - 1);
	u32 temp = number;

	_kernel_repPrintCharBOCHS(paddingChar, width);

	if (!number){
		if (!precision)
			_kernel_printCharBOCHS(' ');
		else
			_kernel_printCharBOCHS('0');
	} else {
		while (base > 0){
			char digit = (char)(temp / base) + 0x30;
			_kernel_printCharBOCHS(digit);
			temp = temp % base;
			base = base / 10;
		}
	}
}

static void _kernel_printHexBOCHS(u32 number, u32 width, char paddingChar, u32 precision, u8 prefix){
	u32 nibbles = _kernel_bochs_get_nibble(number);
	u32 final_nibbles = precision > nibbles ? precision : nibbles;
	u32 currentNibble = 0;
	
	char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	if (prefix)
		_kernel_printStringBOCHS("0x", 0);

	if (width > final_nibbles)
		_kernel_repPrintCharBOCHS(paddingChar, width - final_nibbles);
	
	while (final_nibbles > 0){
		currentNibble = (final_nibbles--) - 1;
		char hexDigit = hex[((number >> (currentNibble << 2)) & 0x0F)];
		_kernel_printCharBOCHS(hexDigit);
	}
}

static void _kernel_printBoolBOCHS(u8 boolean){
	_kernel_printStringBOCHS((boolean == 0 ? "false" : "true"), 0);
}


static void _kernel_repPrintCharBOCHS(char p, u32 rep){
	while (rep-- > 0)
		_kernel_printCharBOCHS(p);

}

inline void kernel_breakBOCHS(){
	__asm__("xchg bx, bx");
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
	if (kernel_in8(0xE9) == 0xE9) result = 1;
	return result;
}



static u32 _kernel_bochs_get_digit(u32 number){
	u32 digit = 1;
	//u32 base = 1;

	while (number > 9){
		digit++;
		number = number / 10;
	}

	return digit;
}

static u32 _kernel_bochs_get_nibble(u32 number){
	u32 nibble = 1;

	while (number > 0xF){
		nibble++;
		number = number >> 4;
	}

	return nibble;
}

static u32 _kernel_bochs_pow(u32 b, u8 p){
	u32 temp = 1;
	while (p-- > 0)
		temp = temp * b;
	return temp;
}