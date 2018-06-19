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
#include <generic/memory.h>

class MemoryManager{
public:
	enum { CHUNK_SIZE = sizeof(MemoryChunk), MINIMAL_ALLOC_SIZE = sizeof(size_t), MINIMAL_ALIGNMENT = sizeof(size_t) };
	MemoryManager()=delete;
	/*
	 * @param base 不能为空
	 */
	MemoryManager(void *base,size_t size,bool initChunks=true);
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
	AS_MACRO size_t size() const {return _size;}

	template <class T>
	T       reallocate(T origin,size_t newSize,size_t constructedSize,
			size_t oldRawSize = SIZE_MAX);

private:
	MemoryChunk *_headChunk;
	void		*_base;
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

template <class T>
T MemoryManager::reallocate(T origin,size_t newSize,
		size_t constructedSize,size_t oldRawSize)
{
	static_assert(std::is_pointer<T>::value,"");
	using V = typename std::remove_pointer<T>::type;
	if(!origin)
			return nullptr;
	auto newRawSize = newSize * sizeof(V);
	if(oldRawSize==SIZE_MAX)
	{
		oldRawSize = getAllocatedLength(origin);
//		assert(rawSize >=  sizeof(T)==0); // FIXME 这里不一定，只需要大于oldSize * sizeof(T)即可
//		oldSize = rawSize / sizeof(T);
	}
	if(oldRawSize==SIZE_MAX) // still can not retrive the original size
		reportError("MemoryManager cannot determine the original pointer length");
	if(newRawSize==0)
	{
		destruct(origin, constructedSize);
		deallocate(origin);
		return nullptr;
	}
	if(oldRawSize >= newRawSize)
		return origin;
	MemoryChunk *chunk = MemoryChunk::chunkPtrOfDataPtr(origin);
	//try to collect unallocated,must be tested
	chunk->allocated(false);
	chunk->mergeTrailingsUnallocated();
	if(chunk->size() >= newRawSize) // 直接extend
	{
		chunk->split(newRawSize);
		chunk->allocated(true);
		return origin;
	}else{ //need to be moved to another place , newSize > oldSize
		chunk->allocated(true);
		T ptr = this->allocateAs<T>(newRawSize);
		if(!ptr) // failed
			return nullptr;
		// 调用移动构造函数
		if(newSize >= constructedSize) // 全部move
			for(size_t i=0;i!=constructedSize;++i)
				new (ptr+i) V(std::move(origin[i]));
		else // 部分需要destruct
		{
			for(size_t i=0;i!=newSize;++i)
				new (ptr+i) V(std::move(origin[i]));
			destruct(ptr+newSize, constructedSize - newSize);
		}
		chunk->allocated(false);
		return ptr;
	}
}



#endif /* INCLUDE_MEMORYMANAGER_H_ */
