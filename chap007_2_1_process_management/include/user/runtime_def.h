/*
 * runtime_def.h
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#ifndef USER_RUNTIME_DEF_H_
#define USER_RUNTIME_DEF_H_


// error
#define FATAL "[FATAL] "
#define INFO  "[INFO] "
#define WARNING "[WARNING] "

// memory
class MemoryManager;
extern MemoryManager mman;


// io
class Output;
extern Output kout;

class UserModeCharacterWriter;
extern UserModeCharacterWriter characterWriter;



#endif /* USER_RUNTIME_DEF_H_ */
