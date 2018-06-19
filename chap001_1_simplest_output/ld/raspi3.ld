
SECTIONS {
	.text : {
		*(.text.boot)
		*(.text)
		*(.text.*)	
	}
}