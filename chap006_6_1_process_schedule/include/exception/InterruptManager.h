/*
 * InterruptManager.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_EXCEPTION_INTERRUPTMANAGER_H_
#define INCLUDE_EXCEPTION_INTERRUPTMANAGER_H_

#include <exception/exception_def.h>

class InterruptManager{
public:
	InterruptManager()=default;
	// INTERCACE
	IntID  standardIntID(StandardInterruptType type)const;
	void   endInterrupt(ExceptionType type,IntID id);
	void   enableInterrupt(IntID id,bool enable);
};




#endif /* INCLUDE_EXCEPTION_INTERRUPTMANAGER_H_ */
