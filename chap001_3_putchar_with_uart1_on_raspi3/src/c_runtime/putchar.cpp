/*
 * putchar.cpp
 *
 *  Created on: May 1, 2018
 *      Author: 13774
 */
#include <cstdio>

#define DATA_PORT 0x3F215040
int putchar(int ch)
{
    *reinterpret_cast<volatile int*>(DATA_PORT)=ch;
    return ch;
}



