/*
 * UserModeCharacterReader.h
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#ifndef USER_IO_USERMODECHARACTERREADER_H_
#define USER_IO_USERMODECHARACTERREADER_H_

#include <io/char/CharacterReader.h>
#include <data/Queue.h>
#include <cstdint>
#include <generic/programming.h>

class UserModeCharacterReader
	:public CharacterReader
{
public:
	UserModeCharacterReader(size_t size=512)
		:_inputBuffer(size)
	{}
	virtual ~UserModeCharacterReader()=default;
	virtual char  read() override ;
	AS_MACRO Queue<uint16_t> & inputBuffer() { return _inputBuffer;}
private:
	Queue<uint16_t> _inputBuffer;
};





#endif /* USER_IO_USERMODECHARACTERREADER_H_ */
