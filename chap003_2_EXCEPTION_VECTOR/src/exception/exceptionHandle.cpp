/*
 * exceptionHandler.cpp
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */
#include <exception/exception_def.h>
#include <io/Output.h>
#include <runtime_def.h>

void exceptionHandle(uint64_t  * savedRegs,ExceptionType type,
		ExceptionOrigin origin)
{
	kout << "ExceptionType = " << type << ","
			<< "ExceptionOrigin = " << origin << "\n";

	__asm__ __volatile__(
		"mov sp,%0 \n\t"
		RESTORE_CONTEXT_AND_ERET()
		::"r"(savedRegs)
	);
}


