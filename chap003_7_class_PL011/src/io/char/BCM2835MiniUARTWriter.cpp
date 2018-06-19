/*
 * BCM2835MiniUARTWriter.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */
#include <io/char/BCM2835MiniUARTWriter.h>

size_t  BCM2835MiniUARTWriter::write(char ch)
{
	while(!_uartDriver->writeReady());
	_uartDriver->rawWrite(ch);
	return 1;
}
void   BCM2835MiniUARTWriter::flush()
{

}



