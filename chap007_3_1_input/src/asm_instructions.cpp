/*
 * asm_instructions.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */
#include <asm_instructions.h>
uint32_t asm_getPSTATE()
{
#define MSR_ORR(xt,xd,sysreg) \
	"mrs " #xd ", " #sysreg " ;orr " #xt "," #xt "," #xd " ;"
	uint32_t  pstate=0;
	__asm__(
			"mov x0,xzr ;"
			MSR_ORR(x0,x1,nzcv)
			MSR_ORR(x0,x1,daif)
			MSR_ORR(x0,x1,currentel)
			MSR_ORR(x0,x1,spsel)
			//MSR_ORR(x0,x1,pan) // pan的表示
			//MSR_ORR(x0,x1,uao) // DOCME 树莓派不支持PAN和UAO寄存器，因此保留为0即可
			                     // 架构是armv8.0
			"strh w0, %0 ;"
			:"=m"(pstate)
			:
			:"x0","x1"
	);
	return pstate;
#undef MSR_ORR
}

void asm_enable_irq(bool en)
{
	auto r = RegDAIF::read();
	r.I = (!en);
	r.write();
}



