/*
 * puts.cpp
 *
 *  Created on: Jun 9, 2018
 *      Author: 13774
 */



#include <cstdio>

// http://zh.cppreference.com/w/c/io/puts
//写入每个来自空终止字符串 str 的字符及附加换行符 '\n' 到输出流 stdout ，如同以重复执行 putc 写入。
//不写入来自 str 的空终止字符
int puts(const char *str)
{
	size_t n=0;
	while(*str)
		n+=putchar(*str++);
	n+=putchar('\n');
	return n;
}

