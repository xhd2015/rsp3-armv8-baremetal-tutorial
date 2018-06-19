/*
 * BCM2835SystemTimer.h
 *
 *  Created on: Apr 16, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_DRIVER_TIMER_BCM2835SYSTEMTIMER_H_
#define INCLUDE_DRIVER_TIMER_BCM2835SYSTEMTIMER_H_
#include <driver/MemoryMappedDevice.h>
#include <generic/error.h>
#include <generic/cpu.h>
#include <cassert>

/**
 * BASE= 0x3F003000
 * 系统时钟的频率是1MHz
 *
 * 注意:  在实际的测试中，发现该Timer的Timer0和Timer2可以产生中断，但是中断寄存器没有任何提示（也就是说中断实际上发生了，但是状态寄存器全部是0值），似乎是因为内部的线路有bug。
 *  我们将使用Timer1和Timer3作为系统的定时器。
 */
class BCM2835SystemTimer
	:public MemoryMappedDevice
{
public:
	enum RegOffset : size_t{
		CS=0, // 状态寄存器和控制寄存器，bit0-3 包括4个compare value的匹配情况，当向相应的位写入1时清除匹配记录，同时取消中断的激活状态
		CLO=0x4, // 计数器的低32位
		CHI=0x8,  // 计数器高32位
		C0=0xC, // compare value ， 每当 CLO与Cx相等时，就在CS中的x上置位并产生中断
		C1=0x10,
		C2=0x14,
		C3=0x18
	};
	template <class ... Args>
	BCM2835SystemTimer(Args && ... args)
		:MemoryMappedDevice(std::forward<Args>(args)...)
		{}
	// FIXME CLO一定比CHI先读吗？
	/* 参考下面的正确实现
	    uint64_t lo=reg32(CLO);
		uint64_t hi=reg32(CHI);
		return (hi<<32)|lo;
	 */
	// 注意：不加static_cast会出现reg32()移位超限的问题。
	uint64_t counterValue()const
	{ return reg32(CLO)| (static_cast<uint64_t>(reg32(CHI))<<32);}

	// 设置compareValue
	AS_MACRO void compareValueUS(size_t n,uint32_t us)
		{compareValue(n, timeToCountUS(us));}
	AS_MACRO void compareValue(size_t n,uint32_t cval)
		{ assert(n<=3);reg32(C0+n*4)=cval;}

	// 将compareValue设置为下一个周期
	AS_MACRO void addCompareValue(size_t n,uint32_t cdiff)
		{ assert(n<=3);reg32(C0+n*4)=counterValue()+cdiff;}
	AS_MACRO void addCompareValueUS(size_t n,uint32_t diffus)
		{ addCompareValue(n, timeToCountUS(diffus));}

	// 当CS寄存器的第n位为1时，表明中断已经发生。写入1清除该中断。
	AS_MACRO bool intFlagSet(size_t n){ assert(n<=3); return reg32(CS)&(1u<<n);}
	AS_MACRO void clearIntFlags(size_t n){ assert(n<=3);reg32(CS)=(1u<<n);}
	AS_MACRO uint64_t clockFreq() const { return 1000000;}

	// counterValue和time之间的转换。
	// c = t*freq , t = c/freq
	// 当freq=1000000时，每经过1个计数,经历的时间是1us；如果频率稍微增大，则经历的时间将是0
	AS_MACRO size_t   timeToCountUS(size_t us)const{ return clockFreq()*us/1000000;}
	AS_MACRO size_t   countToTimeUS(size_t count) const
			{return 1000000*count/clockFreq();}
};



#endif /* INCLUDE_DRIVER_TIMER_BCM2835SYSTEMTIMER_H_ */
