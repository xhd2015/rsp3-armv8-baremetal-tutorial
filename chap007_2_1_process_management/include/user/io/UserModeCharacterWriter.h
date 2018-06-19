/*
 * UserModeCharacterWriter.h
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#ifndef USER_IO_USERMODECHARACTERWRITER_H_
#define USER_IO_USERMODECHARACTERWRITER_H_

#include <io/char/CharacterWriter.h>
#include <data/Queue.h>

class UserModeCharacterWriter
	:public CharacterWriter
{
public:
	UserModeCharacterWriter(size_t bufSize=512):_buffer(bufSize){}
	virtual ~UserModeCharacterWriter()=default;
	virtual size_t  write(char ch) override;
	virtual void  flush() override;
private:
	Queue<char> _buffer;
};





#endif /* USER_IO_USERMODECHARACTERWRITER_H_ */
