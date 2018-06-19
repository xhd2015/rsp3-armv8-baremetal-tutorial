/*
 * BCM2835MiniUARTWriter.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_IO_CHAR_BCM2835MINIUARTWRITER_H_
#define INCLUDE_IO_CHAR_BCM2835MINIUARTWRITER_H_

#include <io/char/CharacterWriter.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <generic/programming.h>

class BCM2835MiniUARTWriter
	:public CharacterWriter
{
public:
	BCM2835MiniUARTWriter(BCM2835MiniUART * driver)
		:_uartDriver(driver)
	{}
	DELETE_COPY(BCM2835MiniUARTWriter);
	virtual ~BCM2835MiniUARTWriter()=default;
	virtual size_t  write(char ch) override;
	virtual void  flush() override;
private:
	BCM2835MiniUART * _uartDriver;
};




#endif /* INCLUDE_IO_CHAR_BCM2835MINIUARTWRITER_H_ */
