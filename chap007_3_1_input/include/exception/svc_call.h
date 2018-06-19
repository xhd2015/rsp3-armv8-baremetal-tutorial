/*
 * svc_call.h
 *
 *  Created on: May 18, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_INTERRUPT_SVC_CALL_H_
#define INCLUDE_INTERRUPT_SVC_CALL_H_

#include <cstddef>
#include <cstdint>
#include <generic/programming.h>


// svc_functions
// `class` ， 为了避免 与全局定义的符号冲突
enum class SvcFunc{
	                           // prototype:
	puts,                      //  size_t    puts(const char *, size_t n=0);
	                           //  输出字符串s的n个字符串， 当n为0时，表明s是以NULL字符结尾的，
	                           // 因此输出直到遇到NULL字符才停止
	                           //  返回实际输出的字符个数
	gets,                      // size_t    gets(Queue<uint16_t>* buf,
							   //              size_t maxNum,bool block);
                               // 读取尽量多的字符，如果没有字符，则根据条件阻塞等待
                               // 返回实际读取的字符数量
	killProcess,               //  void      killProcess(Pid pid, int exitStatus)
							   //              kill进程, pid的取值有特殊含义
	ps,                        //  void      ps();
	                           //            显示系统进程信息
};


// ==forward declarations:svc_call
//template 用于提供立即数
template <SvcFunc func>
AS_MACRO uint64_t svc_call(uint64_t arg0=0,uint64_t arg1=0,
		uint64_t arg2=0,uint64_t arg3=0,uint64_t arg4=0,
		uint64_t arg5=0,uint64_t arg6=0,uint64_t arg7=0);


template <SvcFunc func>
uint64_t svc_call(uint64_t arg0,uint64_t arg1,uint64_t arg2,
			uint64_t arg3,uint64_t arg4,uint64_t arg5,uint64_t arg6,uint64_t arg7)
{
	uint64_t res=0;
	__asm__ __volatile__("mov x0, %2 \n\t"
						"mov  x1, %3  \n\t"
						"mov  x2, %4 \n\t"
						"mov  x3, %5  \n\t"
						"mov  x4, %6  \n\t"
			 	 	 	"mov  x5, %7  \n\t"
						"mov  x6, %8  \n\t"
						"mov  x7, %9  \n\t"
						"svc %1 \n\t"
						"str x0,%0 \n\t"
						:"=m"(res)
						:"i"(func), // 立即数
						 "r"(arg0),"r"(arg1),"r"(arg2),"r"(arg3),
						 	 	 "r"(arg4),"r"(arg5),"r"(arg6),"r"(arg7)
						 :"x0","x1","x2","x3","x4","x5","x6","x7");
	return res;
}


#endif /* INCLUDE_INTERRUPT_SVC_CALL_H_ */
