/*
 * ExceptionHandler.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_EXCEPTION_EXCEPTIONHANDLER_H_
#define INCLUDE_EXCEPTION_EXCEPTIONHANDLER_H_
#include <cstddef>
#include <cstdint>
#include <exception/exception_def.h>
#include <generic/programming.h>
#include <exception/svc_call.h>

template <class _IntManager>
class ExceptionHandler{
public:
	using IntManager = _IntManager;
	ExceptionHandler(IntManager * intman);
	DELETE_COPY(ExceptionHandler);
	/**
	 * 该处理函数被exceptionCHandler调用，为了效率上的考虑，将其inline
	 * @param savedRegs
	 * @param origin
	 * @param sp
	 */
	void handle(uint64_t  * savedRegs,
			ExceptionType type,
			ExceptionOrigin origin,
			IntID         id
			);
	void handleUndefinedInstruction();
	void handleInstructionAbort();
	void handleDataAbort();
	void handleSPAlignmentFault();
	void handlePCAlignmentFault();
	void handleSVC(size_t  * savedRegs,SvcFunc func);
	void handleIRQ(IntID id);
	void handleFIQ(IntID id);
	void handleSError();
	void unhandledException();

	void handleSysCallKillProcess(size_t  * savedRegs);
	void handleSysCallGets(size_t  * savedRegs);
	void fillInputBuffer();
	void          allowEvent(StandardInterruptType event,bool en) {	_intm->enableInterrupt(
			_intm->standardIntID(event),en);}
	/**
	 * 通过等待那些可能出现READY状态进程的事件
	 */
	void          waitReadyProcess();
private:
	IntManager        * _intm;
};



#endif /* INCLUDE_EXCEPTION_EXCEPTIONHANDLER_H_ */
