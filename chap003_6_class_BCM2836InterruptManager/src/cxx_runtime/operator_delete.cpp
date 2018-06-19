/*
 * operator_delete.cpp
 *
 *  Created on: Feb 5, 2018
 *      Author: 13774
 */
#include <new>
#include <memory/MemoryManager.h>
#include <runtime_def.h>

void operator delete(void *p)
{
	mman.deallocate(p);
}
// in system, the same with delete(void*p);
void operator delete(void *p,size_t size)
{
	mman.deallocate(p);
}

