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
#include <memory/MemoryManager.h>
// #include <exception/InterruptManager.h> // 替换
#include <exception/ExceptionHandler.h>
#include <generic/IntegerFormatter_forward.h> //formatBuf
#include <driver/timer/BCM2835SystemTimer.h>
#include <exception/BCM2835InterruptMananger.h>
#include <driver/uart/PL011.h>
#include <io/char/PL011Writer.h>

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

// exception
#include "../src/exception/ExceptionHandler.cpp"
template class ExceptionHandler<BCM2835InterruptManager>;
//InterruptManager                    intMan;
BCM2835InterruptManager            bcm2835IntMan(0);
ExceptionHandler<BCM2835InterruptManager> excpHandler(nullptr);

//timer
BCM2835SystemTimer sysTimer(0);
size_t          sysTimerTick;//us为单位
