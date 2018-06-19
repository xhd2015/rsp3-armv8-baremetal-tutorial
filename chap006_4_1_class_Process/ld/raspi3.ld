
/* 需要提供的符号： IMAGE_BASE  代码的基地址，PART1=0, PART2=0xFFFFFF_0000000000
               KERNEL_BASE 内核头部预留
               STACK_SIZE  4KB的整数倍，至少是4KB，该区域只在系统初始化期间有用。
                                                                      一旦系统加载了第一个进程，此后系统至少存在一个进程运行。每个进程有自己独特的系统栈。
                                                                      因此，该区域可以回收给内存管理器使用。
               RAM_START   0，内存的起始地址，对树莓派3而言就是0
               FREE_RAM_SIZE  可使用的内存大小，对于树莓派3而言，其内存一部分被GPU使用，一部分与外围设备地址交叉。
                                                                             起始，两种都是可以视为外围设备。可使用大小为RAM_SIZE - GPU_RAM_SIZE, 
                             GPU_RAM_SIZE由树莓派的gpu_mem参数确定，默认64MB,RAM_SIZE=1GB
               
   其中： PART2_IMAGE_SIZE = sizeof(text) + sizeof(data)，由output文件决定
 在代码中引用符号使用，一律使用脚本提供的形式(__,双下划线开头)，因为这样其基地址是确定的。                
*/

SECTIONS {
	. = IMAGE_BASE + KERNEL_BASE; 
	
	.text : {
		*(.text.boot)
		__part2_start = . ;
		*(.text.init)
		*(.text)
		*(.text.*)
		*(.init_array) /* FIXME 舍弃该分区 */
		*(.rodata)
		*(.rodata.*)
	}
	
	/* 用户态程序,4KB对齐 */
	. = ALIGN(0x1000);
	.user : {
		PROVIDE(__user_start = . );
		*(.user)
		. = ALIGN(0x1000);
		PROVIDE(__user_end = . );
	}
	
	.data : {
		*(.data)
		*(.data.*)
	}
	
	__kernel_start = . - IMAGE_BASE ; /* config.txt 的KERNEL_START需要的 */
	
	/* 实际上，所有的NOLOAD分区，都可以通过 . = . + SIZEOF(sec)的方式替代
		因为这些分区并不加载，最后，剩下的就是需要加载的有内容的分区
		但是使用NOLOAD更加清晰（当有内容时）。
	 */
	.bss (NOLOAD) : {
	    *(.bss)
	    *(COMMON)
	}
	
	/* 栈 */
	. = . + STACK_SIZE ;
	__stack_top = . ; /* part1加载需要*/
	
	/* 可自由分配的内存  */
	. = ALIGN(8);
	PROVIDE(__free_ram_start = . );
	. =  RAM_START + FREE_RAM_SIZE + IMAGE_BASE ;
	PROVIDE(__free_ram_end = . );
}