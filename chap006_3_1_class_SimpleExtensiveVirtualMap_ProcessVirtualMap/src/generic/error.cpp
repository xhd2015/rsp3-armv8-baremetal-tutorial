/*
 * error.cpp
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */


#include <generic/error.h>
#include <io/Output.h>
#include <cstdlib> //abort
#include <runtime_def.h>

void __attribute__((noreturn)) reportError(const char *s)
{
	kout << FATAL << s << "\n";
	abort();
}

