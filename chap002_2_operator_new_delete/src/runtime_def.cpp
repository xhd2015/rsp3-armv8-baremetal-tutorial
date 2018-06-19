/*
 * runtime_def.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */
#include <runtime_def.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <io/char/BCM2835MiniUARTWriter.h>
#include <io/Output.h>
#include <driver/gpio/GPIO.h>
#include <generic/IntegerFormatter_forward.h> //formatBuf
#include <memory/MemoryManager.h>

// io
GPIO gpio(0);

BCM2835MiniUART uart1(0);
BCM2835MiniUARTWriter uart1Writer(nullptr);
Output kout(nullptr);

// IntegerFormatter
char   formatBuf[FORMAT_BUF_SIZE];

// memory
MemoryManager mman(nullptr,0,false);
