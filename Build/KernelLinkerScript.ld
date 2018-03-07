ENTRY(kernel_main)
OUTPUT("Stick.bin")
OUTPUT_FORMAT("binary")

SECTIONS 
{
    . = 0x10000;
    .text : { *(.text) }
}