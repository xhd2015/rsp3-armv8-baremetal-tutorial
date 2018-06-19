/*
 * exception_def.h
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_EXCEPTION_EXCEPTION_DEF_H_
#define INCLUDE_EXCEPTION_EXCEPTION_DEF_H_
#include <cstdint>
#include <cstddef>
#include <generic/programming.h>


// 提供宏定义以便调用
#define EXCEPTION_TYPE_SYNC 0
#define EXCEPTION_TYPE_IRQ 1
#define EXCEPTION_TYPE_FIQ 2
#define EXCEPTION_TYPE_SERROR 3
#define EXCEPTION_TYPE_DEBUG 4

#define EXCEPTION_ORIGIN_CUR_SP_EL0 0
#define EXCEPTION_ORIGIN_CUR_SP_ELx 1
#define EXCEPTION_ORIGIN_FROM_LOWER_A64 2
#define EXCEPTION_ORIGIN_FROM_LOWER_A32 3

// 用于异常的辨别
enum ExceptionType:uint64_t{
	SYNC=EXCEPTION_TYPE_SYNC,
	IRQ=EXCEPTION_TYPE_IRQ,
	FIQ=EXCEPTION_TYPE_FIQ,
	SError=EXCEPTION_TYPE_SERROR,
	DEBUG=EXCEPTION_TYPE_DEBUG
};
enum ExceptionOrigin:uint64_t{
	CUR_SP_EL0=EXCEPTION_ORIGIN_CUR_SP_EL0,
	CUR_SP_ELx=EXCEPTION_ORIGIN_CUR_SP_ELx,
	FROM_LOWER_A64=EXCEPTION_ORIGIN_FROM_LOWER_A64,
	FROM_LOWER_A32=EXCEPTION_ORIGIN_FROM_LOWER_A32,
};

enum class ExceptionLevel {EL0 = 0,EL1 = 1,EL2 = 2,EL3 = 3};

#define SAVE_REGS_ASM_INSTR() \
		"stp x29,x30,[sp,#-16]! \n\t" \
		"stp x27,x28,[sp,#-16]! \n\t" \
		"stp x25,x26,[sp,#-16]! \n\t" \
		"stp x23,x24,[sp,#-16]! \n\t" \
		"stp x21,x22,[sp,#-16]! \n\t" \
		"stp x19,x20,[sp,#-16]! \n\t" \
		"stp x17,x18,[sp,#-16]! \n\t" \
		"stp x15,x16,[sp,#-16]! \n\t" \
		"stp x13,x14,[sp,#-16]! \n\t" \
		"stp x11,x12,[sp,#-16]! \n\t" \
		"stp x9,x10,[sp,#-16]! \n\t" \
		"stp x7,x8,[sp,#-16]! \n\t" \
		"stp x5,x6,[sp,#-16]! \n\t" \
		"stp x3,x4,[sp,#-16]! \n\t" \
		"stp x1,x2,[sp,#-16]! \n\t" \
		"str x0,[sp,#-8]! \n\t"


#define SAVE_CONTEXT_AND_CALL_HANDLER(handler,type,origin) \
		SAVE_REGS_ASM_INSTR() \
		"mov x0,sp \n\t"  \
		"mov x1," __stringify(type) "\n\t" \
		"mov x2," __stringify(origin) "\n\t" \
		"bl  " __stringify(handler) "\n\t"


// X30可以作为基址寄存器来加载，这样解决sp无法设置的问题
#define RESTORE_REGS_ASM_INSTR_X0_X28(base) \
		"ldr x0,[" __stringify(base) "],#8 \n\t" \
		"ldp x1,x2,[" __stringify(base) "],#16 \n\t" \
		"ldp x3,x4,[" __stringify(base) "],#16 \n\t" \
		"ldp x5,x6,[" __stringify(base) "],#16 \n\t" \
		"ldp x7,x8,[" __stringify(base) "],#16 \n\t" \
		"ldp x9,x10,[" __stringify(base) "],#16 \n\t" \
		"ldp x11,x12,[" __stringify(base) "],#16 \n\t" \
		"ldp x13,x14,[" __stringify(base) "],#16 \n\t" \
		"ldp x15,x16,[" __stringify(base) "],#16 \n\t" \
		"ldp x17,x18,[" __stringify(base) "],#16 \n\t" \
		"ldp x19,x20,[" __stringify(base) "],#16 \n\t" \
		"ldp x21,x22,[" __stringify(base) "],#16 \n\t" \
		"ldp x23,x24,[" __stringify(base) "],#16 \n\t" \
		"ldp x25,x26,[" __stringify(base) "],#16 \n\t" \
		"ldp x27,x28,[" __stringify(base) "],#16 \n\t"

#define RESTORE_CONTEXT_AND_ERET() \
		RESTORE_REGS_ASM_INSTR_X0_X28(sp) \
		"ldp x29,x30,[sp],#16 \n\t"       \
        "eret \n\t"

// 使用x30作为基址寄存器加载，并且不改变x30的值
#define RESTORE_REGS_ASM_INSTR_X30_BASE() \
		RESTORE_REGS_ASM_INSTR_X0_X28(x30) \
		"ldp x29,x30,[x30]\n\t"

// 异常处理函数原型
extern "C"
void exceptionHandle(uint64_t  * savedRegs,ExceptionType type,
		ExceptionOrigin origin);

// 定义IntID类型
using IntID = uint32_t;
constexpr IntID INT_ID_IS_INVALID = 0xFFFFFFFF;

// 定义标准的中断类型
enum class StandardInterruptType { PROCESS_TIMER,INPUT };



#endif /* INCLUDE_EXCEPTION_EXCEPTION_DEF_H_ */
