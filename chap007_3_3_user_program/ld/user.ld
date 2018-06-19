/*
  需要提供的变量： IMAGE_BASE
  				KERNEL_STACK_SIZE 必须对齐到0x1000
  				USER_STACK_SIZE  必须对齐到0x1000
  
本脚本向外提供的符号：
		__code_size,__kernel_stack_sizes,__user_stack_size,__ram_size
		__used_ram_start,__used_ram_end
		__bss_start, __bss_end
  
*/

SECTIONS{

	. = IMAGE_BASE ;
	
	. = ALIGN(0x1000);
	PROVIDE(__code_start = .);
    .text : {
        *(.text.header)
        PROVIDE(__boot_start = .);
        *(.text.init)
        *(.text)
        *(.text.*)
        *(.init_array) /* DOCME 由readelf -a发现*/
        *(.rodata)
        *(.rodata.*)
    }
    . = ALIGN(0x1000);
    PROVIDE(__code_size = . - __code_start);
    PROVIDE(__kernel_stack_size = KERNEL_STACK_SIZE);
    PROVIDE(__user_stack_size = USER_STACK_SIZE );
    . = . + KERNEL_STACK_SIZE + USER_STACK_SIZE ;
    
    
    PROVIDE(__used_ram_start = .);   
    .data :  {
    	*(.data)
    	*(.data.*)
    } 
    
    /* 该区需要初始化为0 */
    .bss (NOLOAD) : {
    	. = ALIGN(8);
    	PROVIDE(__bss_start = .);
    	*(.bss)
    	*(COMMON)
    	. = ALIGN(8);
    	PROVIDE(__bss_end = .);
    }
    . = ALIGN(8);
    PROVIDE(__used_ram_end = .);
    PROVIDE(__free_ram_start = .);
    PROVIDE(__free_ram_offset = __free_ram_start - __used_ram_start );
     /* RAM_SIZE - (__used_ram_end - __used_ram_start) = free_ram */
    . = . + RAM_SIZE - ( __used_ram_end - __used_ram_start ) ;
    PROVIDE(__free_ram_end = .);
    PROVIDE(__ram_size = RAM_SIZE );
    
   
}