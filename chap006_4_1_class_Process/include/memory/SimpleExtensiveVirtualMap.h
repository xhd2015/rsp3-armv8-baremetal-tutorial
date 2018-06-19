/*
 * SimpleExtensiveVirtualMap.h
 *
 *  Created on: Jun 15, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_MEMORY_SIMPLEEXTENSIVEVIRTUALMAP_H_
#define INCLUDE_MEMORY_SIMPLEEXTENSIVEVIRTUALMAP_H_

#include <memory/VirtualMap.h>
#include <data/Vector.h>
#include <memory/VirtualManager.h>
#include <runtime_def.h>

class SimpleExtensiveVirtualMap
	:public VirtualMap
{
private:
	enum { LEVEL = 4};
public:
	SimpleExtensiveVirtualMap(size_t l2TableNum,size_t l3TableNum);
	~SimpleExtensiveVirtualMap();

	AS_MACRO void * tableAddr(size_t i)
		{ return _tableInfo[i]._addr;}
	AS_MACRO void   tableAddr(size_t i,void *addr)
		{ _tableInfo[i]._addr=addr;}
	AS_MACRO void   tableSize(size_t i, size_t size)
		{ _tableInfo[i]._size=size;}
	AS_MACRO size_t tableSize(size_t i)
		{return _tableInfo[i]._size;}

	AS_MACRO size_t freeL2EntryNum()const
		{return _tableInfo[2]._size - _lastL2Index;}

	AS_MACRO void    addManagedMem(void * addr)
	   { _mem.pushBack(addr);}

	AS_MACRO size_t  curVAPage()const
	  { return _curVAPage;}
	AS_MACRO Descriptor4KBL3 * curDescriptor()
	  { return reinterpret_cast<Descriptor4KBL3*>(
				_l3Info[_lastL3DescriptorInfoIndex]._addr
						) + _lastL3Index;}


	bool increaseMap(void *mem,size_t pageNum,Descriptor4KBL3 dup);

	 // 在尾部增加pageNum个内存，失败时页表未改变。成功返回true，失败返回false
	// 增加的是可读写的正常内存吗？
	// 返回起始内存地址
	bool     increasePages(size_t pageNum,Descriptor4KBL3 dup);

	//
	// 简单可扩展的4级页表，包括3个L0,L1,L2的页表，L3页表由pageNum决定
	// 1 <= pageNum <= 512
	static VirtualMap* makeSimpleExtensiveVirtualMap(size_t pageNum);
	static bool makeSimpleExtensiveVirtualMap(VirtualMap* vmap,size_t pageNum);


	template <class Descriptor>
	void chainNextLevel();

private:
	// 不需要改动L2表项，并且页表足够
	void fastIncreaseMap(size_t pageNum,const void * mem,Descriptor4KBL3 dup);
private:
	struct DescriptorInfo{
		void * _addr;
		size_t _size;
	};

private:
	DescriptorInfo   _tableInfo[LEVEL - 1];//前3级页表
	Vector<DescriptorInfo> _l3Info; // 第4级页表动态可变

	// L3的Index总是处于有效的状态
	size_t _lastL2Index; // 最后一个未使用的L2描述符，如果与_sizes[2]相同，意味着已满。
	size_t _lastL3DescriptorInfoIndex;// 查找L3的下标
	size_t _lastL3Index; //最后一个未使用的L3描述符，如果与_l3Info[..]._size相同，意味着已满。

	Vector<void *> _mem; // 在销毁时需要释放的内存区域。

	size_t  _curVAPage;
};


// ====template

template <class Descriptor>
void SimpleExtensiveVirtualMap::chainNextLevel()
{
	size_t level = levelOfDescriptor<Descriptor>();
	void* nextTableAddr = (level==2)?
				_l3Info[0]._addr:
				_tableInfo[level+1]._addr;
	size_t phyPage =
			virtman.translateVAToPA(
					reinterpret_cast<size_t>(nextTableAddr))/_D::PAGE_SIZE;
	makeTableDescriptors(
			reinterpret_cast<Descriptor*>(_tableInfo[level]._addr),
			1, phyPage);
	makeNullDescriptors(
			reinterpret_cast<Descriptor*>(_tableInfo[level]._addr)+1,
			_tableInfo[level]._size - 1);
}





#endif /* INCLUDE_MEMORY_SIMPLEEXTENSIVEVIRTUALMAP_H_ */
