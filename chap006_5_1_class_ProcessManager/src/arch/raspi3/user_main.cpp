/*
 * user.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: 13774
 */
#include <cstddef>
// svc #0的作用：打印id
__attribute__((section(".user")))
void  user_main(size_t id)
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


