#ifndef UTILS_H
#define UTILS_H

#ifndef NULL
#define NULL (void*)0
#endif

#define IS_NULL(x) (x == NULL)

void kernel_halt(void);
void kernel_cli(void);
void kernel_warn(const char*);

#endif