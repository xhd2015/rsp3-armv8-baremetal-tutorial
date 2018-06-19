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

#endif /* INCLUDE_GENERIC_BITS_H_ */
