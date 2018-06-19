/*
 * Input.h
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#ifndef USER_IO_INPUT_H_
#define USER_IO_INPUT_H_


#include <data/String.h>
#include <io/char/CharacterReader.h>

/**
 * 必须保证reader不为nullptr(因为内部没有检查)
 */
class Input{
public:
	Input(CharacterReader * reader):_chReader(reader){}
	Input(const Input &)=delete;
	Input & operator=(const Input &)=delete;
	AS_MACRO void redirect(CharacterReader * reader){ _chReader=reader;}

	AS_MACRO Input &operator>>(char &ch) { ch=getchar();return *this;}
	Input &operator>>(int &d);
	//读取字符串，略过空白符，直到遇到下一个空白符 ， 空白符=换行，tab,空格
	Input &operator>>(String &s);
	String readline();

	// will block
	AS_MACRO char      getchar(){return _chReader->read();}

private:
	CharacterReader * _chReader;
};



#endif /* USER_IO_INPUT_H_ */
