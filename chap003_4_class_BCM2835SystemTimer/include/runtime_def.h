/*
 * runtime_def.h
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_RUNTIME_DEF_H_
#define INCLUDE_RUNTIME_DEF_H_

class GPIO;
class BCM2835MiniUART;
class BCM2835MiniUARTWriter;
class Output;
class MemoryManager;
class InterruptManager;
template <class IntManager>
     class ExceptionHandler;
class BCM2835SystemTimer;

// io
extern GPIO gpio;
extern BCM2835MiniUART uart1;
extern BCM2835MiniUARTWriter uart1Writer;
extern Output kout;

// error
#define FATAL "[FATAL] "
#define INFO  "[INFO] "
#define WARNING "[WARNING] "

// memory
extern MemoryManager mman;

// exception
extern InterruptManager                    intMan;
extern ExceptionHandler<InterruptManager> excpHandler;


//timer
extern BCM2835SystemTimer sysTimer;

#endif /* INCLUDE_RUNTIME_DEF_H_ */
