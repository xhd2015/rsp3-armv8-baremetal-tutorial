/*
 * VirtualMemoryDefinition.h
 *
 *  Created on: Jun 12, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_MEMORY_VIRTUALMEMORYDEFINITION_H_
#define INCLUDE_MEMORY_VIRTUALMEMORYDEFINITION_H_

#include <cstddef>
#include <generic/util.h>
#include <generic/programming.h>

class VirtualMemoryDefinition{
public:
	static constexpr size_t PAGE_BITS=12;
	static constexpr size_t ENTRY_BITS=3;
	static constexpr size_t ENTRY_SIZE=(1<<ENTRY_BITS);
	static constexpr size_t ENTRY_NUM_OF_EACH_TABLE = (1 << (PAGE_BITS - ENTRY_BITS));
	static constexpr size_t PAGE_SIZE=(1u<<PAGE_BITS);

	static constexpr size_t MEMORY_ATTR_NORMAL=0;  // typically:RAM
	static constexpr size_t MEMORY_ATTR_PERIPHERAL=1; // PERIPHERAL
	static constexpr size_t MEMORY_ATTR_NON_CACHEABLE=2; //不可缓存

	// 1ul<<39, 1ul<<30,1ul<<21,1ul<<12
	/**
	 * 某一级的表项能够映射到最大内存大小
	 * @param level
	 * @return
	 */
	AS_MACRO static size_t  maxMapSizeOfLevel(size_t level)
		{return (1ul<< (39 - 9*level));}
	AS_MACRO static size_t  entryNumOfMapSize(size_t mapSize,size_t level)
		{return alignAhead(mapSize, maxMapSizeOfLevel(level))/maxMapSizeOfLevel(level);}
	AS_MACRO static size_t  memSizeOfMapSize(size_t mapSize,size_t level)
		{return entryNumOfMapSize(mapSize, level)*ENTRY_SIZE;}
};



#endif /* INCLUDE_MEMORY_VIRTUALMEMORYDEFINITION_H_ */
