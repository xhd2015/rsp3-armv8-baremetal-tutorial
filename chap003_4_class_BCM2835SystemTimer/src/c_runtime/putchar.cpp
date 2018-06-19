/*
 * putchar.cpp
 *
 *  Created on: May 1, 2018
 *      Author: 13774
 */
#include <cstdio>
#include <driver/uart/BCM2835MiniUART.h>
#include <runtime_def.h>

int putchar(int ch)
{
	while(!uart1.writeReady());
	uart1.rawWrite(ch);
	return ch;
}



