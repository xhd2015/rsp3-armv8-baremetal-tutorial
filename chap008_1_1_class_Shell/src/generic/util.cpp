/*
 * util.cpp
 *
 *  Created on: Jun 9, 2018
 *      Author: 13774
 */


#include <generic/util.h>
#include <cstdint>
#include <cstring>

size_t itos(size_t i,unsigned int base,char *dst,
		size_t size,bool prefix,bool lowerCase)
{
	// 检查base是否是允许的值
	if(base<2 || base > 16)
		return 0;
	if(size == 0)
		return 0;

	size_t index=0;
	if(base==2 && prefix)
	{
		dst[index++]='0';
		dst[index++]='b';
	}else if(base==16 && prefix){
		dst[index++]='0';
		dst[index++]='x';
	}

	size_t start = index;

	if(i==0)
		dst[index++]='0';
	else{
		while(i && (index+1) < size)
		{
			dst[index++]=digitMap(i%base, lowerCase);
			i /= base;
		}
	}
	dst[index]='\0';
	// FIXME 手动bug
//	reverse(dst,index);
	reverse(dst + start, index - start);
	return index;
}

int   digitMap(size_t digit,bool lowerCase)
{
	if(digit<=9)
		return digit + '0';
	else if(digit<=15)
		return digit - 10 + (lowerCase?'a':'A');
	return -1;
}

void reverse(char *str,size_t size)
{
	if(size > 1)
		for(size_t i=0,j=size-1;i<j;++i,--j)
		{
			char temp=str[i];
			str[i]=str[j];
			str[j]=temp;
		}
}

size_t stoi(const char *src,size_t size,bool *failed)
{
	size_t base=10;
	size_t i=0;
	if(size >= 2 && src[0]=='0')
	{
		if(src[1]=='x')
			base=16;
		else if(src[1]=='b')
			base=2;
		else{
//			kout << FATAL << "Unsupported prefix :0" << src[1] << "\n";
			if(failed)
				*failed=true;
			return SIZE_MAX;
		}
		i=2;

	} // no prefix
	size_t v=0;
	for(;i!=size;++i)
	{
		size_t itv=SIZE_MAX;
		if(src[i]>='0' && src[i]<='9')
			itv=src[i]-'0';
		else if(src[i]>='a' && src[i]<='f')
			itv=src[i]-'a' + 10;
		else if(src[i]>='A' && src[i]<='F')
			itv=src[i]-'A' + 10;
		if(itv >= base)
		{
//			kout << FATAL << "Bad format, ch=" << src[i] << "\n";
			if(failed)
				*failed=true;
			return SIZE_MAX;
		}
		v*=base;
		v+=itv;
	}
	return v;
}
size_t stoi(const char *src,bool *failed)
{
	return stoi(src,std::strlen(src),failed);
}
uint64_t alignAhead(uint64_t d,uint64_t alignment)
{
	auto left=d%alignment;//
	return left?(d + alignment - left):d;
}
uint64_t alignBackward(uint64_t d,uint64_t alignment)
{
	auto left=d%alignment;
	return left?(d - left):d;
}

size_t ceilDiv(size_t a, size_t b)
{
	return a/b + (!!(a%b)); // 0-1化
}
size_t gcd(size_t a,size_t b)
{
	return b?gcd(b,a%b):a;
}
size_t lcm(size_t a,size_t b)
{
	return a/gcd(a,b)*b;
}


