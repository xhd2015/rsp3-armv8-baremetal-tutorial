/*
 * putchar.cpp
 *
 *  Created on: May 1, 2018
 *      Author: 13774
 */
#include <cstdio>
#include <runtime_def.h>
#include <io/Output.h>

int putchar(int ch)
{
	kout << ch;
	return ch;
}



