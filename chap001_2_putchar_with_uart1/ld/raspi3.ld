
SECTIONS {
	. = 0x80000;
	.text : {
		*(.text.boot)
		*(.text)
		*(.text.*)
	} 
	
	PROVIDE(__stack_top = 0x80000);
}