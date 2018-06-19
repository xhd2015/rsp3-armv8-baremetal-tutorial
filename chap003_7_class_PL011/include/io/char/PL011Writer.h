/*
 * BCM2835MiniUARTWriter.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_IO_CHAR_PL011WRITER_H_
#define INCLUDE_IO_CHAR_PL011WRITER_H_

#include <io/char/CharacterWriter.h>
#include <driver/uart/PL011.h>
#include <generic/programming.h>

class PL011Writer
	:public CharacterWriter
{
public:
	PL011Writer(PL011 * driver)
		:_uartDriver(driver)
	{}
	DELETE_COPY(PL011Writer);
	virtual ~PL011Writer()=default;
	virtual size_t  write(char ch) override;
	virtual void  flush() override;
private:
	PL011 * _uartDriver;
};




#endif /* INCLUDE_IO_CHAR_PL011WRITER_H_ */
