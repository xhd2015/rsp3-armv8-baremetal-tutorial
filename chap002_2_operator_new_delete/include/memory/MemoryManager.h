/*
 * MemoryManager.h
 *
 *  Created on: Jan 27, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_MEMORYMANAGER_H_
#define INCLUDE_MEMORYMANAGER_H_

#include <cstddef>
#include <cstdint>
#include <memory/MemoryChunk.h>
#include <generic/programming.h>
#include <generic/error.h>
#include <cstring>
#include <type_traits>
#include <new>
#include <utility> //move

class MemoryManager{
public:
	enum { CHUNK_SIZE = sizeof(MemoryChunk), MINIMAL_ALLOC_SIZE = sizeof(size_t), MINIMAL_ALIGNMENT = sizeof(size_t) };
	MemoryManager()=delete;
	/*
	 * @param base 不能为空
	 */
	MemoryManager(void *base,size_t size,bool initChunks=true);
	AS_MACRO void rebase(size_t diff) { _base += diff;*reinterpret_cast<char**>(&_headChunk)+=diff;}
public:
	// 最小分配要求
	size_t normalizeAllocSize(size_t n);

	// 默认对齐的分配
	void* allocate(size_t n);
	// 自带对齐要求的分配
	// 关于对齐分配，参见 c++ std::aligned_alloc, http://en.cppreference.com/w/cpp/memory/c/aligned_alloc
	void* allocate(size_t n,size_t alignment);
	template <class T>
	T        allocateAs(size_t n);
	template <class T>
	T        allocateAs(size_t n,size_t alignment);
	/**
	 * 返回合适的alignment, 同时满足alignment和MINIMAL_ALIGNMENT
	 * 返回最小公倍数，使用欧几里得算法
	 * @param alignment
	 * @return
	 */
	size_t    properAlignment(size_t alignment)const;

	/**
	 * withdraw an allocated pointer.
	 * note:for nullptr,nothing happens.
	 */
	void  deallocate(void *p);

	/**
	 *  get size that the origin spanned, only if it is allocated from this MemoryManager,otherwise 0.
	 *  return SIZE_MAX, means that "I do not know"
	 */
	size_t getAllocatedLength(void *origin)const;

	AS_MACRO void* base()  {return _base;}
	AS_MACRO const size_t size() const {return _size;}

private:
	MemoryChunk *_headChunk;
	char		*_base;
	size_t	    _size;
};




//===template impl
template <class T>
T		MemoryManager::allocateAs(size_t n)
{
	return reinterpret_cast<T>(this->allocate(n));
}
template <class T>
T        MemoryManager::allocateAs(size_t n,size_t alignment)
{
	return reinterpret_cast<T>(this->allocate(n,alignment));
}


#endif /* INCLUDE_MEMORYMANAGER_H_ */
