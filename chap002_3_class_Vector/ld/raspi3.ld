

SECTIONS {
	. = 0x80000;
	.text : {
		*(.text.boot)
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
	}
	.bss (NOLOAD) : {
	    *(.bss)
	    *(COMMON)
	}
	.data : {
		*(.data)
		*(.data.*)
	}
	.free : {
		PROVIDE(__free_ram_start = . );
	}
	
	PROVIDE(__stack_top = 0x80000);
}