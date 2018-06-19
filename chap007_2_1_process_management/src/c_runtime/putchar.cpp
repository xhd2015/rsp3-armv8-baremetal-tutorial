/*
 * putchar.cpp
 *
 *  Created on: May 1, 2018
 *      Author: 13774
 */
#include <cstdio>
#include <io/Output.h>
#include <runtime_def.h>

int putchar(int ch)
{
	kout << ch;
	return ch;
}



