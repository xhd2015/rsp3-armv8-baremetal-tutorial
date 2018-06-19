#include <io/char/PL011Writer.h>

size_t  PL011Writer::write(char ch)
{
	while(!_uartDriver->writeReady());
	_uartDriver->rawWrite(ch);
	return 1;
}
void   PL011Writer::flush()
{

}



