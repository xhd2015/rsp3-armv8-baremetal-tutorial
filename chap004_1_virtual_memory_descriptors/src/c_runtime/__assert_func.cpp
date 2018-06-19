/*
 * __assert_func.cpp
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */
#include <cassert>
#include <cstdlib>
#include <runtime_def.h>
#include <io/Output.h>

// GNU C Header definition:
//#ifdef NDEBUG           /* required by ANSI standard */
//# define assert(__e) ((void)0)
//#else
//# define assert(__e) ((__e) ? (void)0 : __assert_func (__FILE__, __LINE__, \
//						       __ASSERT_FUNC, #__e))


void __attribute__ ((__noreturn__)) __assert_func(const char * file,
		int line, const char *func, const char * expr)
{
	kout << "at line " << line << ", file \"" << file << "\", function \""<<func<<"\", fails\n\t \""<<expr<<"\"\n";
	abort();
}


