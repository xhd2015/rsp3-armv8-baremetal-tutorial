/*
 * bits.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_GENERIC_BITS_H_
#define INCLUDE_GENERIC_BITS_H_

#include <cstdint>
#include <generic/programming.h>

// example: upperMaskBits(4) = 0xf000 0000 0000 0000
//  意味着，高4位为1，其他位全为0
AS_MACRO uint64_t upperMaskBits(uint64_t i)
{
	return 0xffffffffffffffff >> (64-i) << (64-i);
}
// example:lowerMaskBits(4) = 0x0000 0000 0000 000f
AS_MACRO uint64_t lowerMaskBits(uint64_t i)
{
	return 0xffffffffffffffff << (64-i) >> (64-i);
}
AS_MACRO uint64_t middleMaskBits(uint64_t lowerBound,uint64_t upperBound)
{
	// 清除低端范围和高端范围
	return 0xffffffffffffffff >> lowerBound
			<< (64 - upperBound - 1 + lowerBound)
			>> (64-upperBound -1);
}


// no check, set bits[lowerBound,upperBound]=v, others keep unchanged
template <class Type>
AS_MACRO void setBits(Type & i, uint8_t lowerBound,uint8_t upperBound,uint64_t v)
{
	// clear middle, and validate v, shift v to proper position, concate i,v together
	i = (i & (~middleMaskBits(lowerBound, upperBound)))
			| ((v & lowerMaskBits(upperBound - lowerBound + 1))<<lowerBound );
}
AS_MACRO uint64_t getBits(uint64_t i, uint8_t lowerBound,uint8_t upperBound)
{
	return (i>>lowerBound)&lowerMaskBits(upperBound - lowerBound + 1);
}

template <class Type,class ValueType>
AS_MACRO void     setBit(Type & i,uint8_t index,ValueType v)
{
	setBits(i,index,index,v);
}
AS_MACRO uint64_t getBit(uint64_t i,uint8_t index)
{
	return getBits(i,index,index);
}

namespace{

	template <size_t bit,size_t ... bits>
	struct BitMaskStruct{
		static constexpr size_t value= BitMaskStruct<bit>::value|BitMaskStruct<bits...>::value;
	};
	template <size_t bit>
	struct BitMaskStruct<bit>{
		static constexpr size_t value=(1u<<bit);
	};

}

//==MACROS
// 保留为1的位
template <size_t ... bit>
AS_MACRO constexpr size_t bitOnes()
{
	return BitMaskStruct<bit...>::value;
}
// 保留为0的位
template <size_t ...bit>
AS_MACRO constexpr size_t bitZeros()
{
	return ~(BitMaskStruct<bit...>::value);
}
// 设置目标位为1，其他位不变
template <size_t ...bit,class T>
AS_MACRO T bitsSet(T t)
{
	return t | bitOnes<bit...>();
}
// 设置目标位为0,其他位不变
template <size_t ...bit,class T>
AS_MACRO T bitsClear(T t)
{
	return (t &  bitZeros<bit...>());
}
// 保留目标位，其他位为0
// 可用如下的语句检测目标位是否设置： if(bitsKept<16>(t))...
template <size_t ...bit,class T>
AS_MACRO T bitsKept(T t)
{
	return (t & bitOnes<bit...>());
}
template <size_t ...bit,class T>
AS_MACRO bool bitsAnySet(T t)
{
	return bitsKept<bit...>(t);
}
template <size_t ...bit,class T>
AS_MACRO bool bitsNonSet(T t)
{
	return !bitsAnySet<bit...>(t);
}



#endif /* INCLUDE_GENERIC_BITS_H_ */
