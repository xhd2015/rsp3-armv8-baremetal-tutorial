/*
 * UserModeCharacterWriter.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */
#include <io/UserModeCharacterWriter.h>
#include <exception/svc_call.h>

// DOCME Queue的一种特殊用法：永远保证_data的数据从0开始，到_size结束。
size_t  UserModeCharacterWriter::write(char ch)
{
	if(_buffer.full())
		flush();
	_buffer.put(ch);
	return 1;
}
void  UserModeCharacterWriter::flush()
{
	if(!_buffer.empty())
	{
		svc_call<SvcFunc::puts>(
				reinterpret_cast<uint64_t>(_buffer.data()),_buffer.size());
	}
	_buffer.reset(0);
}






