/*
 * ProcessVirtualMap.h
 *
 *  Created on: Jun 15, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_MEMORY_PROCESSVIRTUALMAP_H_
#define INCLUDE_MEMORY_PROCESSVIRTUALMAP_H_

#include <memory/SimpleExtensiveVirtualMap.h>
#include <generic/programming.h>

class ProcessVirtualMap
	:public SimpleExtensiveVirtualMap
{
public:
		enum Region { R_CODE,R_KSTACK,R_USTACK,R_FREE_RAM,
			REGIONS
		};
		ProcessVirtualMap(size_t codePages,size_t kernelStackPages,
				size_t userStackPages, size_t freeRamPages)
			:ProcessVirtualMap(
					ceilDiv( 1 + codePages +  kernelStackPages+
			userStackPages + freeRamPages,_D::ENTRY_NUM_OF_EACH_TABLE),
			 codePages,kernelStackPages,userStackPages,freeRamPages)
		{}
		DELETE_COPY(ProcessVirtualMap);
private:
		ProcessVirtualMap(size_t l3TableNum,size_t codePages,size_t kernelStackPages,
				size_t userStackPages, size_t freeRamPages);
public:
		/**
		 * mem不为nullptr
		 * @param region
		 * @param mem
		 * @param memManaged
		 */
		void rebase(Region region,void * mem,bool memManaged);
		void* renew(Region region);
		AS_MACRO size_t  VAPageStartOf(Region region)
		{
			return _vaPageStart[region];
		}

private:
	size_t  _pages[REGIONS];
	size_t  _vaPageStart[REGIONS]; // 注意，对于栈而言，这个地址是顶部地址
	Descriptor4KBL3 * _headEntry[REGIONS];
};


#endif /* INCLUDE_MEMORY_PROCESSVIRTUALMAP_H_ */
