/*
 * VirtualMap_forward.h
 *
 *  Created on: Jun 13, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_MEMORY_VIRTUALMAP_H_
#define INCLUDE_MEMORY_VIRTUALMAP_H_

#include <arch/aarch64/vmsa_descriptors.h>
#include <memory/VirtualMemoryDefinition.h>
#include <cassert>



class VirtualMap{
public:
	using _D = VirtualMemoryDefinition;

	template <class Descriptor>
	static void  duplicateEntries(Descriptor *lx,size_t n,Descriptor dup);

	template <class Descriptor>
	static void rebase(Descriptor *lx,size_t n,size_t phyPageAddr);
	// 特化，特化不需要参数的原因：参数可以卸载形参列表中
	static void rebase(Descriptor4KBL0 *lx,size_t n,size_t phyPageAddr);
	static void rebase(Descriptor4KBL3 *lx,size_t n,size_t phyPageAddr);

	template <class Descriptor>
	static Descriptor* makeTableDescriptor(Descriptor *descr);
	static Descriptor4KBL0* makeTableDescriptor(Descriptor4KBL0 *descr);

	// 返回结束项
	template <class Descriptor>
	static Descriptor* makeTableDescriptors(Descriptor *start,
			size_t num,size_t phyPageAddr);

	template <class Descriptor>
	static Descriptor* makeBlockDescriptor(Descriptor *p);
	static Descriptor4KBL3* makeBlockDescriptor(Descriptor4KBL3 *descr,
			bool global,bool ns,size_t memAttr,bool readOnly,bool privileged);

	template <class Descriptor>
	static Descriptor* makeNullDescriptor(Descriptor *start);

	// 返回结束项
	template <class Descriptor>
	static Descriptor* makeBlockDescriptors(Descriptor *start,
			size_t num,size_t phyPageAddr,
			bool global,bool ns,size_t memAttr,bool readOnly,bool privileged);

	// 返回结束项
	template <class Descriptor>
	static Descriptor* makeNullDescriptors(Descriptor *start,size_t num);

};



// ====== template
template <class Descriptor>
void  VirtualMap::duplicateEntries(Descriptor *lx,size_t n,Descriptor dup)
{
	for(size_t i=0;i!=n;++i)
		lx[i]=dup;
}

template <class Descriptor>
void VirtualMap::rebase(Descriptor *lx,size_t n,size_t phyPageAddr)
{
	assert(lx->S0.IsTable);
	for(size_t i=0;i!=n;++i)
		lx[i].S1.NextLevelTableAddr = phyPageAddr + i;
}

template <class Descriptor>
Descriptor* VirtualMap::makeTableDescriptor(Descriptor *descr)
{
	Descriptor::make(descr,0);
	descr->S1.IsTable=1;
	descr->S1.Valid=1;
	return descr;
}

template <class Descriptor>
Descriptor* VirtualMap::makeTableDescriptors(Descriptor *start,
		size_t num,size_t phyPageAddr)
{
	makeTableDescriptor(start);
	duplicateEntries(start, num, *start);
	rebase(start, num, phyPageAddr);
	return start+num;
}

template <class Descriptor>
Descriptor* VirtualMap::makeBlockDescriptor(Descriptor *p)
{
//		static_assert(false,"unimplemented");
	return nullptr;
}

template <class Descriptor>
Descriptor* VirtualMap::makeBlockDescriptors(Descriptor *start,
		size_t num,size_t phyPageAddr,
		bool global,bool ns,size_t memAttr,bool readOnly,bool privileged)
{
	makeBlockDescriptor(start, global, ns, memAttr, readOnly, privileged);
	duplicateEntries(start, num, *start);
	rebase(start,num,phyPageAddr);
	return start+num;
}
template <class Descriptor>
Descriptor* VirtualMap::makeNullDescriptor(Descriptor *start)
{
	Descriptor::make(start,0);
	return start;
}
template <class Descriptor>
Descriptor* VirtualMap::makeNullDescriptors(Descriptor *start,size_t num)
{
	for(size_t i=0;i!=num;++i,++start)
		Descriptor::make(start,0);
	return start;
}


#endif /* INCLUDE_MEMORY_VIRTUALMAP_H_ */
