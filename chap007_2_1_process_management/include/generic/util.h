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
#include <type_traits>
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

/**
 * 整数除法，向上取整
 * @param a
 * @param b
 * @return
 */
size_t ceilDiv(size_t a, size_t b);

// math
// gcd:最大公约数
size_t gcd(size_t a,size_t b);
// lcm:最小公倍数
size_t lcm(size_t a,size_t b);

// 查找第一个设置的位
template <class T>
uint8_t         findFirstSet(T i)
{
	static_assert(std::is_unsigned<T>::value,"");
	uint8_t index=0;
	while(index < (sizeof(T)*8) && !(i& 0b1) )
	{
		i >>= 1;
		++index;
	}
	if(index==sizeof(T)*8)
		return 0xffu;
	else
		return index;
}

// 指针值增加
template <class T>
T* pointerInc(T *p,ptrdiff_t diff)
{
	return p ? reinterpret_cast<T*>(reinterpret_cast<char*>(p)+diff):
			   nullptr;
}
template <class T>
const T* pointerInc(const T *p,ptrdiff_t diff)
{
	return p ? reinterpret_cast<const T*>(reinterpret_cast<const char*>(p)+diff):
			nullptr;
}




#endif /* INCLUDE_GENERIC_UTIL_H_ */
