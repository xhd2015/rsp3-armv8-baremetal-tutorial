/*
 * VirtualMemory.h
 *
 *  Created on: Apr 6, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_MANAGER_VIRTUALMEMORY_H_
#define INCLUDE_MANAGER_VIRTUALMEMORY_H_


#include <exception/exception_def.h>
#include <arch/aarch64/system_common_registers.h>
#include <arch/aarch64/vmsa_descriptors.h>
#include <memory/VirtualMemoryDefinition.h>
#include <generic/bits.h>
#include <cassert>

class VirtualManager{
public:
	using _D = VirtualMemoryDefinition;

	VirtualManager()
		:_addressBits(0)
	{}
	/**
	 * 构造一个对象，但是并不立即执行它的功能。
	 * 因为这个对象的功能具有副作用。
	 */
	VirtualManager(size_t addressBits)
		:_addressBits(addressBits)
	{}


	int   init();

	void enableMMU(void* jmpFunc,void *newSp,size_t addressBits,void* lowFreeRamStart);
	/**
	 * 使TTBR0置为无效，该函数应当在跳转到高端地址之后调用
	 * 通过设置TCR.EPD=1, 重置所有TTBR0的缓存即可。
	 */
	void enableTTBR0(bool enable);
	void updateTTBR0(const Descriptor4KBL0 *l0Table);
	void updateTTBR1(const Descriptor4KBL0 *l0Table);
	// DOCME because change of PAR_EL1 does not effect the
	//     memory system,so const applies here.
	void*  translateVAToPA(const void * va)const;
	uint64_t  translateVAToPA(uint64_t va)const;

	AS_MACRO size_t addressBits() const { return _addressBits;}
	AS_MACRO size_t highMemBase() const { return upperMaskBits(64 - _addressBits);}
private:
	size_t          _addressBits;
};


#endif /* INCLUDE_MEMORY_VIRTUALMANAGER_H_ */
