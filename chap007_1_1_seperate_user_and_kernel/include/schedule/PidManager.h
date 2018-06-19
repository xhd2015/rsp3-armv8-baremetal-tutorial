/*
 * PidManager.h
 *
 *  Created on: Mar 16, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_SCHEDULE_PIDMANAGER_H_
#define INCLUDE_SCHEDULE_PIDMANAGER_H_


#include <data/Bitmap.h>
#include <schedule/forward.h>

// 将PidManager从ProcessManager中，主要是考虑解耦的问题。
class PidManager{
public:
	PidManager(size_t pidNum);
	Pid allocate();
	void    deallocate(Pid pid);
private:
	Bitmap  _bitmap;
};

#endif /* INCLUDE_SCHEDULE_PIDMANAGER_H_ */
