/*
 * GPIO.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_DRIVER_GPIO_GPIO_H_
#define INCLUDE_DRIVER_GPIO_GPIO_H_

#include <driver/MemoryMappedDevice.h>
#include <generic/programming.h>
#include <utility>
#include <generic/cpu.h>

class GPIO
	:public MemoryMappedDevice
{
public:
	enum RegOffset : size_t {
		GPFSELx = 0, // x=0-5
		GPSETx =0x1C, //x=0-1
		GPCLRx =0x28, //x=0-1
		GPLEVx = 0x34, //x=0-1
		GPPUD = 0x94, // Pull-up/down enable
		GPUDCLKx = 0x98,// x=0-1
	};
	enum PUD { P_OFF=0b00, P_EN_PULL_DOWN=0b01, P_EN_PULL_UP=0b10};
	enum ALT { ALT_INPUT=0b000, ALT_OUTPUT=0b001, ALT_0=0b100,ALT_1=0b101,
		       ALT_2=0b110,ALT_3=0b111,ALT_4=0b011,ALT_5=0b010};
	static constexpr size_t MAX_PIN = 60;
	static constexpr size_t SEL_OF_EACH_GROUP=10;
	static constexpr size_t BITS_PER_SEL=3;

	template <class ... Args>
	GPIO(Args && ... args)
		:MemoryMappedDevice(std::forward<Args>(args)...)
		{}

	// 等待150个周期
	AS_MACRO static void waitSignalDone() { delayCPU(150);}
	void signalModify(uint32_t pin,PUD initPUD=P_OFF);
	void selectAltFunction(uint32_t pin, ALT alt);
};



#endif /* INCLUDE_DRIVER_GPIO_GPIO_H_ */
