/*
 * ProcessVirtualMap.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: 13774
 */


#include <memory/ProcessVirtualMap.h>
#include <memory/VirtualManager.h>
#include <runtime_def.h>

ProcessVirtualMap::ProcessVirtualMap(size_t l3TableNum,size_t codePages,size_t kernelStackPages,
		size_t userStackPages, size_t freeRamPages)
:SimpleExtensiveVirtualMap(
		ceilDiv(l3TableNum, _D::ENTRY_NUM_OF_EACH_TABLE), l3TableNum)
{
	_pages[R_CODE] = codePages;
	_pages[R_KSTACK] = kernelStackPages;
	_pages[R_USTACK] = userStackPages;
	_pages[R_FREE_RAM] = freeRamPages;

	Descriptor4KBL3 nullDesc;
	Descriptor4KBL3 codeDesc;
	Descriptor4KBL3 privDesc;
	Descriptor4KBL3 unprivDesc;

	VirtualMap::makeNullDescriptor(&nullDesc);
	VirtualMap::makeBlockDescriptor(&codeDesc,
			false, true, _D::MEMORY_ATTR_NORMAL, true, false);
	VirtualMap::makeBlockDescriptor(&privDesc,
			false, true, _D::MEMORY_ATTR_NORMAL, false, true);
	VirtualMap::makeBlockDescriptor(&unprivDesc,
			false, true, _D::MEMORY_ATTR_NORMAL, false, false);
	increaseMap(nullptr, 1, nullDesc);
	_vaPageStart[R_CODE] = curVAPage();
	_headEntry[R_CODE] = curDescriptor();
	increaseMap(nullptr,_pages[R_CODE],codeDesc);
	_headEntry[R_KSTACK] = curDescriptor();
	increaseMap(nullptr, _pages[R_KSTACK], privDesc);
	_vaPageStart[R_KSTACK] = curVAPage();
	_headEntry[R_USTACK] = curDescriptor();
	increaseMap(nullptr,_pages[R_USTACK], unprivDesc);
	_vaPageStart[R_USTACK] = curVAPage();
	_vaPageStart[R_FREE_RAM] = curVAPage();
	_headEntry[R_FREE_RAM] = curDescriptor();
	increaseMap(nullptr, _pages[R_FREE_RAM], unprivDesc);
}

void ProcessVirtualMap::rebase(Region region,void * mem,bool memManaged)
{

	VirtualMap::rebase(_headEntry[region],
			_pages[region],
			reinterpret_cast<size_t>(
					virtman.translateVAToPA(mem))/_D::PAGE_SIZE);
	if(memManaged)
		addManagedMem(mem);
}
void* ProcessVirtualMap::renew(Region region)
{
	void * mem = mman.allocate(_pages[region]*_D::PAGE_SIZE,_D::PAGE_SIZE);
	assert(mem);
	rebase(region,mem,true);
	return mem;
}

