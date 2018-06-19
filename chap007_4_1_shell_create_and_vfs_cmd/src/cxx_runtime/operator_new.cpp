/*
 * operator_new.cpp
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */
#include <new>
#include <memory/MemoryManager.h>
#include <runtime_def.h>

void* operator new(size_t n)
{
	return mman.allocate(n);
}



