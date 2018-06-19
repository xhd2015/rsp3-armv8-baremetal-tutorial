/*
 * IntegerFormatter.h
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_GENERIC_INTEGERFORMATTER_H_
#define INCLUDE_GENERIC_INTEGERFORMATTER_H_

#include <generic/IntegerFormatter_forward.h>

template <int base,bool prefix,bool lowerCase>
IntegerFormatter<base,prefix,lowerCase>::IntegerFormatter(size_t num)
	:_n(num){}

template <int base,bool prefix,bool lowerCase>
IntegerFormatter<base,prefix,lowerCase>::IntegerFormatter(const void *p)
	:_n(reinterpret_cast<size_t>(p)){}

template <int base,bool prefix,bool lowerCase>
char *IntegerFormatter<base,prefix,lowerCase>::format()const
{
	itos(_n, base, formatBuf, FORMAT_BUF_SIZE,prefix,lowerCase);
	return formatBuf;
}



#endif /* INCLUDE_GENERIC_INTEGERFORMATTER_H_ */
