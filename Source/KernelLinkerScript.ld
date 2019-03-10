ENTRY(kernel_main)
OUTPUT("Stick.bin")
OUTPUT_FORMAT("binary")

SECTIONS 
{
    . = 0x10000;
    kernel_begin = .;
    .text ALIGN(4k) :
    {
    	*(._main);
    	*(.text);
    }
    .data ALIGN(4k) : {*(.data);}
    .bss ALIGN(4k) : {*(.bss);}


    kernel_end = .;
    /DISCARD/ :
    {
    	*(.comment)
    	*(.eh_frame_hdr)
    	*(.eh_frame)
    }
}

