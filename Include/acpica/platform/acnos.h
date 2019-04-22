#ifndef __ACNOS_H__
#define __ACNOS_H__

#define COMPILER_DEPENDENT_INT64 s64 //signed long long int
#define COMPILER_DEPENDENT_UINT64 u64 //unsigned long long int

#define ACPI_DIV_64_BY_32(hi, lo, d32, q32, r32) \
{ \
	__asm__("mov edx, %3\n\tmov eax, %4\n\tdiv %2\n\tmov %0, eax\n\tmov %1, edx" \
			: "=r" (q32), "=r" (r32) \
			: "r" (d32), "r" (hi), "r" (lo)); \
}

#define ACPI_SHIFT_RIGHT_64(hi, lo) \
{ \
	__asm__("shr %0, 1\n\trcr %1, 1" \
			: "=r" (hi), "=r" (lo)); \
}

#define ACPI_INTERNAL_XFACE __attribute__((cdecl))
#define ACPI_INTERNAL_VAR_XFACE __attribute__((cdecl))
#define ACPI_SYSTEM_XFACE __attribute__((cdecl))


#undef ACPI_USE_SYSTEM_CLIBRARY


#define ACPI_MACHINE_WIDTH 32

#define ACPI_DISASSEMBLER
#define ACPI_DEBUGGER
#define ACPI_DEBUG_OUTPUT

#endif