/*
 * operator_new.cpp
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */
#include <new>
#include <runtime_def.h>
#include <memory/MemoryManager.h>

void* operator new(size_t n)
{
	return mman.allocate(n);
}



