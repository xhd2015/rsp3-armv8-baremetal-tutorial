/*
 * MemoryMappedDevice.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_DRIVER_MEMORYMAPPEDDEVICE_H_
#define INCLUDE_DRIVER_MEMORYMAPPEDDEVICE_H_

#include <generic/programming.h>
#include <cstddef>
#include <cstdint>

//

class MemoryMappedDevice
{
public:
	MemoryMappedDevice(volatile void * base)
		:_base(reinterpret_cast<volatile char*>(base))
	{}

	MemoryMappedDevice(size_t base)
		:_base(reinterpret_cast<volatile char*>(base))
	{}

	template <class T>
	AS_MACRO volatile T & reg(size_t offset)
		{return *reinterpret_cast<volatile T*>(_base +  offset);}
	template <class T>
	AS_MACRO const volatile T &  reg(size_t offset)const
		{return *reinterpret_cast<const volatile T*>(_base +  offset);}

	AS_MACRO volatile uint8_t & reg8(size_t offset)
			{return reg<uint8_t>(offset);}
	AS_MACRO const volatile uint8_t & reg8(size_t offset)const
			{return reg<uint8_t>(offset);}
	AS_MACRO volatile uint16_t & reg16(size_t offset)
			{return reg<uint16_t>(offset);}
	AS_MACRO const volatile uint16_t& reg16(size_t offset)const
			{return reg<uint16_t>(offset);}
	AS_MACRO volatile uint32_t & reg32(size_t offset)
			{return reg<uint32_t>(offset);}
	AS_MACRO const volatile uint32_t & reg32(size_t offset)const
			{return reg<uint32_t>(offset);}
	AS_MACRO volatile uint64_t & reg64(size_t offset)
			{return reg<uint64_t>(offset);}
	AS_MACRO const volatile uint64_t & reg64(size_t offset)const
			{return reg<uint64_t>(offset);}
private:
	volatile char * _base;
};





#endif /* INCLUDE_DRIVER_MEMORYMAPPEDDEVICE_H_ */
