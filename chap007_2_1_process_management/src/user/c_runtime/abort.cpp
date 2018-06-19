/*
 * abort.cpp
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */

//Defined in header <stdlib.h>
//void abort(void);
//(until C11)
//_Noreturn void abort(void);


#include <cstdlib>
#include <cstdio>
#include <io/Output.h>
#include <runtime_def.h>
#include <asm_instructions.h>

void __attribute__((noreturn)) abort()
{
	kout << FATAL << "-- ABORT --\n";
	while(true){asm_wfi();}
}
