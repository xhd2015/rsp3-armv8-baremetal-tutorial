/*
 * Output.h
 *
 *  Created on: Mar 15, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_IO_OUTPUT_H_
#define INCLUDE_IO_OUTPUT_H_
#include <cstdint>
#include <io/char/CharacterWriter.h>
#include <generic/programming.h>

class Output{
public:
	explicit Output(CharacterWriter *writer):
		_chWriter(writer)
	{}
	Output(const Output &)=delete;
	Output & operator=(const Output &)=delete;
	AS_MACRO void     redirect(CharacterWriter *writer){_chWriter=writer;}
	size_t print(const char *s,size_t len);
	size_t print(const char *s);
	Output & operator<<(char ch);
	// 为什么引入16,32的？为了避免歧义
	Output & operator<<(uint8_t u);
	Output & operator<<(uint16_t u);
	Output & operator<<(uint32_t u);
	Output & operator<<(bool i);
	Output & operator<<(short i);
	Output & operator<<(int i);
	Output & operator<<(double d);
	Output & operator<<(const char *s);
	Output & operator<<(size_t i);
	Output & operator<<(const void *p);
	Output & operator<<(const volatile void *p);
	Output & flush();
private:
	CharacterWriter * _chWriter;
};

#endif /* INCLUDE_IO_OUTPUT_H_ */
