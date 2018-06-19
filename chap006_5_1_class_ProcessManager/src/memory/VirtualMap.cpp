/*
 * VirtualMap.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: 13774
 */

#include <memory/VirtualMap.h>


// 特化，特化不需要参数的原因：参数可以卸载形参列表中
void VirtualMap::rebase(Descriptor4KBL0 *lx,size_t n,size_t phyPageAddr)
{
	for(size_t i=0;i!=n;++i)
		lx[i].NextLevelTableAddr = phyPageAddr + i;
}
void VirtualMap::rebase(Descriptor4KBL3 *lx,size_t n,size_t phyPageAddr)
{
	for(size_t i=0;i!=n;++i)
		lx[i].OutputAddr = phyPageAddr + i;
}

Descriptor4KBL0* VirtualMap::makeTableDescriptor(Descriptor4KBL0 *descr)
{
	Descriptor4KBL0::make(descr,0);
	descr->IsTable=1;
	descr->Valid=1;
	return descr;
}
Descriptor4KBL3* VirtualMap::makeBlockDescriptor(Descriptor4KBL3 *descr,
		bool global,bool ns,size_t memAttr,bool readOnly,bool privileged)
{
	Descriptor4KBL3::make(descr,0);
	descr->Valid=1;
	descr->AF=1;
	descr->RES1_0=1;
	descr->nG = (!global);
	descr->NS = ns;
	descr->AttrIndex = memAttr;

	// bit1: set read-only(1), or read-write(0) ,
	//  bit0: not from EL0(0) or else(1)
	if(!privileged)
		descr->AP = 0b01;
	if(readOnly)
		descr->AP |= 0b10;
	return descr;
}


