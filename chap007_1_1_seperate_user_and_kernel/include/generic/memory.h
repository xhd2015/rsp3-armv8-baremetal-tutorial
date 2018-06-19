/*
 * memory.h
 *
 *  Created on: Jun 13, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_GENERIC_MEMORY_H_
#define INCLUDE_GENERIC_MEMORY_H_


#include <type_traits>
template <class T>
void destruct(T t,size_t n);


//=== template
template <class T>
void destruct(T t,size_t n)
{
	static_assert(std::is_pointer<T>::value,"");
	using V = typename std::remove_pointer<T>::type;
	if(!t)return;
	for(size_t i=0;i!=n;++i)
			t[i].~V();
}


#endif /* INCLUDE_GENERIC_MEMORY_H_ */
