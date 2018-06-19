/*
 * BCM2835InterruptManager.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */
#include <exception/BCM2835InterruptMananger.h>
#include <generic/util.h>
#include <runtime_def.h>
#include <io/Output.h>
#include <driver/timer/BCM2835SystemTimer.h>


void BCM2835InterruptManager::disableAllInterrupts()
{
	reg32(DISABLE_BASIC_IRQ)=0xFF;
	reg32(DISABLE_IRQ_1)=0xFFFFFFFF;
	reg32(DISABLE_IRQ_1)=0xFFFFFFFF;
}
IntID BCM2835InterruptManager::locateInterrupt() const
{
	auto i=findFirstSet(static_cast<uint8_t>(reg32(PENDING_IRQ_BASIC)));//只查找低8位
	if(i < 8)
		return i;
	i=findFirstSet(reg32(PENDING_IRQ_1));
	if(i!=0xFFu)
		return i+8;
	i=findFirstSet(reg32(PENDING_IRQ_2));
	if(i!=0xFFu)
		return i+40;//32+8
	return i;//failed
}

void   BCM2835InterruptManager::endInterrupt(ExceptionType tp,IntID src)
{
	assert(src< INT_NUM);
	switch(src)
	{
	case SRC_UART_INT:
		//uart0.clearIntFlags(); // FIXME 这里什么也不做合适吗？
		break;
	case SRC_SYS_TIMER_FIRST:
	case SRC_SYS_TIMER_FIRST+1:
	case SRC_SYS_TIMER_FIRST+2:
	case SRC_SYS_TIMER_LAST:
		sysTimer.clearIntFlags(src - SRC_SYS_TIMER_FIRST);
		sysTimer.addCompareValueUS(src - SRC_SYS_TIMER_FIRST, sysTimerTick);
		break;
	default:
		kout << FATAL << "un-ended  interrupt\n";
	}
//	uint8_t grp=0;
//	uint8_t offset=src; // default src<8
//	if(src>=8)
//	{
//		grp=(src-8)/32;
//		offset=(src-8)%32;
//	}
//	disableWord(grp) = (1u<<offset);
//	enableWord(grp) = (1u<<offset);

}

void    BCM2835InterruptManager::enableInterrupt(IntID src,bool en)
{
	assert(src< INT_NUM);
	if(src<=7)
	{
		if(en)
			reg32(ENABLE_BASIC_IRQ)=(1u<<src);
		else
			reg32(DISABLE_BASIC_IRQ)=(1u<<src);
	}else{
		src-=8;
		if(en)
			reg32(ENABLE_IRQ_1 + (src/32)*4) = (1u<< (src%32));
		else
			reg32(DISABLE_IRQ_1 + (src/32)*4) = (1u<< (src%32));
	}
}
IntID  BCM2835InterruptManager::standardIntID(StandardInterruptType type)const
{
	switch(type)
	{
	case StandardInterruptType::INPUT:
		return SRC_UART_INT;
	case StandardInterruptType::PROCESS_TIMER:
		return SRC_SYS_TIMER_FIRST+1; // DOCME 使用的是第一个Timer，因为Timer0 Timer2均不可用
	default:
//		kout << FATAL << "Unmapped standard interrupt type\n";
		reportError("Unmapped standard interrupt type");
		return INT_ID_IS_INVALID;
	}
}





