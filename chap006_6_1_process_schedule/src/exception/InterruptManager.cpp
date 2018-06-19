/*
 * InterruptManager.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#include <exception/InterruptManager.h>
#include <io/Output.h>
#include <runtime_def.h>


IntID  InterruptManager::standardIntID(StandardInterruptType type)const
{
	return INT_ID_IS_INVALID;
}
void   InterruptManager::endInterrupt(ExceptionType type,IntID id)
{
	kout << INFO << "InterruptManager endInterrupt [" << id << "].\n";
}
void   InterruptManager::enableInterrupt(IntID id,bool enable)
{
	kout << INFO << "InterruptManager enableInterrupt [" << id << "] = "
			<< enable <<".\n";
}

