/*
 * IntegerFormatter_forward.h
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_GENERIC_INTEGERFORMATTER_FORWARD_H_
#define INCLUDE_GENERIC_INTEGERFORMATTER_FORWARD_H_

#include <cstdint>
#include <cstddef>
#include <io/Output.h>

template <int base,bool prefix,bool lowerCase>
class IntegerFormatter{
public:
	IntegerFormatter(size_t num);
	IntegerFormatter(const void *p);
	char *format()const;
private:
	size_t _n;

};

// 带有前缀的版本
using Hex=IntegerFormatter<16,true,false>;
using Dec=IntegerFormatter<10,false,false>;
using Bin=IntegerFormatter<2,true,false>;

constexpr size_t FORMAT_BUF_SIZE = 64;
extern    char   formatBuf[FORMAT_BUF_SIZE];

template <int base,bool prefix,bool lowerCase>
Output & operator<<(Output &out,
		const IntegerFormatter<base,prefix,lowerCase> &formatter);





#endif /* INCLUDE_GENERIC_INTEGERFORMATTER_FORWARD_H_ */
