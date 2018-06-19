/*
 * cpu.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#include <generic/cpu.h>
#include <asm_instructions.h>

void delayCPU(size_t i)
{
	while(i--)
		asm_nop();
}
void delayCPU(size_t i,size_t j)
{
	while(i--)
		delayCPU(j);
}

