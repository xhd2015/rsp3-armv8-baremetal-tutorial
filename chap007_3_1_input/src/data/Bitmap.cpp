/*
 * Bitmap.cpp
 *
 *  Created on: Jun 14, 2018
 *      Author: 13774
 */
#include <data/Bitmap.h>
#include <cstring>
#include <cassert>
#include <memory/MemoryManager.h>
#include <runtime_def.h>
#include <generic/bits.h>

Bitmap::Bitmap(size_t bitmapSize)
	:_mem(nullptr),
	 _bitmapSize(bitmapSize),
	 _memEntryNum(bitmapSize/sizeof(size_t))
{
	_mem = mman.allocateAs<size_t*>(_memEntryNum * sizeof(size_t));
	assert(_mem);
	size_t i=0;
	for(;i!=_memEntryNum-1;++i)
		_mem[i]=0;
	_mem[i] = upperMaskBits(bitmapSize - _memEntryNum*sizeof(size_t)); // 高位置1

}
Bitmap::~Bitmap()
{
	if(_mem)
		mman.deallocate(_mem);
	_mem = nullptr;
}
bool Bitmap::value(size_t i)const
{
    size_t index=i/sizeof(size_t);
    size_t offset=i%sizeof(size_t);
    return _mem[index]&(1<<offset);
}

void Bitmap::value(size_t i,bool v)
{
    size_t index=i/sizeof(size_t);
    size_t offset=i%sizeof(size_t);
    if(v)
    	_mem[index] |= (1<<offset);
    else
    	_mem[index] &= ~(1<<offset);
}

size_t Bitmap::findUnset()
{
	for(size_t i=0;i!=_memEntryNum;++i)
	{
		if(_mem[i]!=SIZE_MAX)// 非全1
			return findFirstSet(~_mem[i]); // 将数据变反，查找第一个set的
	}
	return SIZE_MAX;
}



