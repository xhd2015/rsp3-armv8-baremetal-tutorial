/*
 * asm_delay_us.cpp
 *
 *  Created on: Jun 17, 2018
 *      Author: 13774
 */
#include <asm_instructions.h>
#include <arch/aarch64/timer_registers.h>

void asm_delay_us(size_t usec)
{
	uint64_t neededCount = static_cast<uint64_t>(RegCNTFRQ_EL0::read().ClockFrq)/1000*usec/1000;
	uint64_t initCount = RegCNTPCT_EL0::read().PhysicalCount;
	while(RegCNTPCT_EL0::read().PhysicalCount - initCount < neededCount) // wait
		;
}

