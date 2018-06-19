/*
 * runtime_def.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#include <runtime_def.h>
#include <io/Output.h>
#include <memory/MemoryManager.h>

Output kout(0);

MemoryManager mman(nullptr,0,false);

// IntegerFormatter
#include <generic/IntegerFormatter_forward.h>
char   formatBuf[FORMAT_BUF_SIZE];

