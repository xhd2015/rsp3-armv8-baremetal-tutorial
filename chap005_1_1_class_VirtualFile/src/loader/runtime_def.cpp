/*
 * runtime_def.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: 13774
 */

#include <runtime_def.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <io/char/BCM2835MiniUARTWriter.h>
#include <io/Output.h>
#include <driver/gpio/GPIO.h>
#include <memory/MemoryManager.h>
#include <generic/IntegerFormatter_forward.h> //formatBuf
#include <driver/uart/PL011.h>
#include <io/char/PL011Writer.h>
#include <memory/VirtualManager.h>

// io
GPIO gpio(0);

BCM2835MiniUART uart1(0);
PL011           uart0(0);
BCM2835MiniUARTWriter uart1Writer(nullptr);
PL011Writer     uart0Writer(nullptr);
Output kout(nullptr);

// IntegerFormatter
char   formatBuf[FORMAT_BUF_SIZE];

// memory
MemoryManager mman(nullptr,0,false);

// virtual memory
VirtualManager virtman;

