/*
 * operator_delete.cpp
 *
 *  Created on: Feb 5, 2018
 *      Author: 13774
 */
#include <new>

void operator delete(void *p)
{

}
// in system, the same with delete(void*p);
void operator delete(void *p,size_t size)
{

}
//
//void operator delete[](void* p)
//{
//	mman.deallocate(p);
//}
//
//void operator delete[](void* p,size_t size)
//{
//	mman.deallocate(p);
//}
