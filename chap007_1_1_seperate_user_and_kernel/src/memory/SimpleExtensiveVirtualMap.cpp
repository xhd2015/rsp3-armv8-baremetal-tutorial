/*
 * SimpleExtensiveVirtualMap.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: 13774
 */

#include <memory/SimpleExtensiveVirtualMap.h>
#include <memory/VirtualManager.h>
#include <runtime_def.h>

SimpleExtensiveVirtualMap::SimpleExtensiveVirtualMap(
		size_t l2TableNum,size_t l3TableNum)
:_l3Info(),
 _lastL2Index(0),
 _lastL3DescriptorInfoIndex(0),
 _lastL3Index(0),
 _mem(),
 _curVAPage(0)
 {
	// 假定l0page和l1page数量都为1
	assert(l2TableNum >= 1 && l3TableNum >= 1
			&& l3TableNum <= l2TableNum*_D::ENTRY_NUM_OF_EACH_TABLE);
	auto  mem = mman.allocateAs<char*>(
			(2+l2TableNum+l3TableNum)*_D::PAGE_SIZE, _D::PAGE_SIZE);
	assert(mem);
	// 需要管理的页表
	addManagedMem(mem);
	for(size_t i=0;i!=LEVEL-2;++i)//设置L0，L1
	{
		_tableInfo[i]._size = _D::ENTRY_NUM_OF_EACH_TABLE;
		_tableInfo[i]._addr = mem + _D::PAGE_SIZE*i;
	}
	_tableInfo[2]._size = l2TableNum*_D::ENTRY_NUM_OF_EACH_TABLE;
	_tableInfo[2]._addr = mem + 2*_D::PAGE_SIZE;
	// 单独设置L2的数量
	// 单独设置L3的数量
	_l3Info.emplaceBack();
	_l3Info[0]._addr = mem + _D::PAGE_SIZE*3;
	_l3Info[0]._size = l3TableNum * _D::ENTRY_NUM_OF_EACH_TABLE;

	// L0首项-->L1,L1首项-->L2,L2首项-->第一个L3， 其他项无效
	chainNextLevel<Descriptor4KBL0>();
	chainNextLevel<Descriptor4KBL1>();
	chainNextLevel<Descriptor4KBL2>();

	// L3所有项 无效
	makeNullDescriptors(
			reinterpret_cast<Descriptor4KBL3*>(_l3Info[0]._addr),
			_l3Info[0]._size);
 }

SimpleExtensiveVirtualMap::~SimpleExtensiveVirtualMap()
{
	for(size_t i=0;i!=_mem.size();++i)
		mman.deallocate(_mem[i]);
	_mem.clear();
}




bool SimpleExtensiveVirtualMap::increaseMap(void *mem,size_t pageNum,Descriptor4KBL3 dup)
{
	size_t leftL2= _tableInfo[2]._size - _lastL2Index;
	if(leftL2==0)//L2不足
		return false;
	size_t fastSatisfy = _l3Info[_lastL3DescriptorInfoIndex]._size - _lastL3Index;
	if(fastSatisfy < pageNum)//无法快速满足，需要增加L2项
	{
		size_t newl2IndexNeeded =
						alignAhead(pageNum - fastSatisfy, _D::ENTRY_NUM_OF_EACH_TABLE)/
							_D::ENTRY_NUM_OF_EACH_TABLE;
		if(newl2IndexNeeded > leftL2 - 1) // L2不足
					return false;
		auto tablesMem = mman.allocate(
				newl2IndexNeeded *_D::PAGE_SIZE,_D::PAGE_SIZE );
		if(!tablesMem)
			return false;
		addManagedMem(tablesMem);

		// DOCME 包含一个假设，VA对应的PA是连续的
		// 映射L2
		auto l2=reinterpret_cast<Descriptor4KBL2*>(_tableInfo[2]._addr)
				+ _lastL2Index + 1;
		makeTableDescriptors(l2, newl2IndexNeeded, reinterpret_cast<size_t>(
				virtman.translateVAToPA(tablesMem))/_D::PAGE_SIZE);

		// 增加L3页表描述
		_l3Info.emplaceBack();
		_l3Info.last()._addr = tablesMem;
		_l3Info.last()._size = newl2IndexNeeded*_D::ENTRY_NUM_OF_EACH_TABLE;
	}
	fastIncreaseMap(pageNum, mem, dup);
	return true;
}
 // 在尾部增加pageNum个内存，失败时页表未改变。成功返回true，失败返回false
// 增加的是可读写的正常内存吗？
// 返回起始内存地址
bool     SimpleExtensiveVirtualMap::increasePages(size_t pageNum,Descriptor4KBL3 dup)
{
	void * mem = mman.allocate(pageNum*_D::PAGE_SIZE, _D::PAGE_SIZE);
	if(!mem)
		return false;
	if(!increaseMap(mem, pageNum, dup))
	{
		mman.deallocate(mem);
		return false;
	}else{
		addManagedMem(mem);
		return true;
	}
}

void SimpleExtensiveVirtualMap::fastIncreaseMap(size_t pageNum,const void * mem,
		Descriptor4KBL3 dup)
{
	size_t phyPage = reinterpret_cast<size_t>(
			virtman.translateVAToPA(mem))/_D::PAGE_SIZE;
	_curVAPage += pageNum;
	while(pageNum > 0)
	{
		auto l3 = curDescriptor();
		size_t thisNum = _l3Info[_lastL3DescriptorInfoIndex]._size - _lastL3Index;
		if(thisNum>pageNum)
			thisNum = pageNum;

		duplicateEntries(l3	, thisNum,dup);
		if(phyPage)
			rebase(l3, thisNum,phyPage);

		pageNum -= thisNum;
		_lastL3Index += thisNum;
		phyPage += thisNum;

		if(_lastL3Index == _l3Info[_lastL3DescriptorInfoIndex]._size)
		{
			++_lastL3DescriptorInfoIndex;
			_lastL3Index=0;
		}
	}
}


