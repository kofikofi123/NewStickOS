ENTRY(kernel_main)
OUTPUT("Stick.bin")
OUTPUT_FORMAT("binary")

SECTIONS 
{
    . = 0x10000;
    kernel_begin = .;
    .text :
    {
    	*(._main);
    	*(.text);
    }
    .data : {*(.data);}
    .bss : {*(.bss);}

    kernel_end = .;
    /DISCARD/ :
    {
    	*(.comment)
    	*(.eh_frame_hdr)
    	*(.eh_frame)
    }
}

