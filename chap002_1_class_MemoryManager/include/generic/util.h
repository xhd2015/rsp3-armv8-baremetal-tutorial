/*
 * util.h
 *
 *  Created on: Jun 9, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_GENERIC_UTIL_H_
#define INCLUDE_GENERIC_UTIL_H_

#include <cstdint>
#include <cstddef> //size_t
/**
 * 将数按指定的进制转换成字符串
 * @param i
 * @param base
 * @param dst
 * @param size
 * @param prefix 仅当二进制和十六进制时有效
 * @param lowerCase 仅当十六进制时有效，字母小写还是大写
 * @return 缓冲区内字符串的长度，不包括末尾的'\0'。返回0时表明有错误。
 */
size_t itos(size_t i,unsigned int base,char *dst,
		size_t size,bool prefix=false,bool lowerCase=true);

/**
 *
 * @param digit
 * @param lowerCase
 * @return 非法返回-1，其他返回对应数字
 */
int   digitMap(size_t digit,bool lowerCase=true);

/**
 * 字符串反转
 * @param str
 * @param size
 */
void reverse(char *str,size_t size);


/**
 * 向前对齐
 * @param d
 * @param alignment
 * @return
 */
uint64_t alignAhead(uint64_t d,uint64_t alignment);
/**
 * 向后对齐
 * @param d
 * @param alignment
 * @return
 */
uint64_t alignBackward(uint64_t d,uint64_t alignment);


// math
// gcd:最大公约数
size_t gcd(size_t a,size_t b);
// lcm:最小公倍数
size_t lcm(size_t a,size_t b);


#endif /* INCLUDE_GENERIC_UTIL_H_ */
