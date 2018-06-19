/*
 * init.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */
#include <cstddef>

__asm__(
		".section .text.header ;"
		".8byte    6 ;" // 头部的域数量，不包括其本身
		".8byte    __boot_start ;"
		".8byte    __code_size, __kernel_stack_size,__user_stack_size,__ram_size ;"
		".8byte    __free_ram_offset;"
		);
__attribute__((section(".text.init")))
void init(size_t id)
{
	while(true)
	{
		__asm__ __volatile__(
				"ldr x0,%0 \n\t"
				"svc #0 \n\t"
				::"m"(id)
				 :"x0"
				);
	}
}

