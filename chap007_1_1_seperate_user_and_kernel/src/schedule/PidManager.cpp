/*
 * PidManager.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: 13774
 */

#include <schedule/PidManager.h>

PidManager::PidManager(size_t pidNum)
	:_bitmap(pidNum)
{
	_bitmap.value(PID_INVALID,1);
	_bitmap.value(PID_CURRENT,1);
	_bitmap.value(PID_PARENT,1);
}

Pid PidManager::allocate()
{
	auto i = _bitmap.findUnset();
	if(i==SIZE_MAX)
		return PID_INVALID;
	_bitmap.value(i,true);
	return static_cast<Pid>(i);
}
void    PidManager::deallocate(Pid pid)
{
	if(pid>=PID_FIRST_USEABLE)
		_bitmap.value(pid, false);
}

