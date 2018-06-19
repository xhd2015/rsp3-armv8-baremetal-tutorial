/*
 * UserModeCharacterReader.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#include <io/UserModeCharacterReader.h>
#include <runtime_def.h>
#include <exception/svc_call.h>
#include <generic/util.h>

char  UserModeCharacterReader::read()
{
	if(_inputBuffer.empty())
	{
		_inputBuffer.reset(0);
		while(true)
		{
			// 因为是BLOCKED的，返回时数据仍然不足是可能的
			size_t n= svc_call<SvcFunc::gets>(
							reinterpret_cast<uint64_t>(&_inputBuffer),
							_inputBuffer.capacity(),
							true //blocked
							);
			if(n>0)
				break;
		}
	}
	return _inputBuffer.remove();
}



