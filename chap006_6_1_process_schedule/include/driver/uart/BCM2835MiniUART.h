/*
 * BCM2835MiniUART.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_DRIVER_UART_BCM2835MINIUART_H_
#define INCLUDE_DRIVER_UART_BCM2835MINIUART_H_
#include <driver/MemoryMappedDevice.h>
#include <generic/bits.h>
#include <cassert>
#include <utility>

class BCM2835MiniUART
	:public MemoryMappedDevice
{
public:
	enum RegOffset : size_t{
		AUX_IRQ = 0,  // RO,32bits. 0=MiniUART,1=SPI1,2=SPI2,other=RES0
		AUX_ENABLES = 0x4, //32bits,
		IO_DATA = 0x40,// 8bits
		IER_INT_ENABLED = 0x44,// bit0,1 = READ，WRITE interrupt enabled, RO(only when DLAB==1)
		IIR_INT_IDENTITY = 0x48, // bit0 = notIntPending  bit1-2:clear-on-write, read-to-identify
		LINE_CONTROL = 0x4C,
		MODEM_CONTROL = 0x50,
		LINE_STATUS = 0x54,
		MODEM_STATUS = 0x58,
		SCRATCH = 0x5C,
		EXTRA_CONTROL = 0x60,
		EXTRA_STATUS = 0x64,//32bits
		BAUD_RATE = 0x68, //16bits , baudrate = system_clock_freq/(8*(BAUD_RATE+1))
	};

	template <class ... Args>
	BCM2835MiniUART(Args && ... args)
		:MemoryMappedDevice(std::forward<Args>(args)...)
		{}
	int init(); //标准的init函数，成功返回0值。


	AS_MACRO void enable(bool en) { setBit(reg32(AUX_ENABLES),0,en);}
	AS_MACRO char rawRead()const { return static_cast<char>(reg32(IO_DATA));}
	AS_MACRO void rawWrite(char ch){ reg32(IO_DATA)=ch;}
	AS_MACRO void enableReceive(bool en) { setBit(reg32(EXTRA_CONTROL),0,en);}
	AS_MACRO void enableSend(bool en) { setBit(reg32(EXTRA_CONTROL),1,en);}
	AS_MACRO void baudrate(uint32_t rate) { reg32(BAUD_RATE)=rate;}
	/**
	 *
	 * @param bits 7 or 8
	 */
	AS_MACRO void dataSize(size_t bits){assert(bits==7 || bits==8);setBit(reg32(LINE_CONTROL),0,bits-7);}
	/**
	 *  至少能够接受一个byte
	 * @return
	 */
	AS_MACRO bool writeReady()const { return getBit(reg32(LINE_STATUS),5);}
	AS_MACRO bool readReady()const { return getBit(reg32(LINE_STATUS),0);}
};



#endif /* INCLUDE_DRIVER_UART_BCM2835MINIUART_H_ */
