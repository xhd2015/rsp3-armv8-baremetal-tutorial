/*
 * asm_instructions.h
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_ASM_INSTRUCTIONS_H_
#define INCLUDE_ASM_INSTRUCTIONS_H_

#include <generic/programming.h>
#include <arch/aarch64/system_common_registers.h>

AS_MACRO void asm_nop(){__asm__("nop");}
AS_MACRO void asm_isb(){__asm__("isb");}
AS_MACRO void asm_wfi(){__asm__("wfi");}
// 地址翻译
AS_MACRO RegPAR_EL1 asm_at(uint64_t va)
{
	__asm__ __volatile__("at S1E1R,%0"::"r"(va));
	return RegPAR_EL1::read();
}
// 重置TLB，根据ASID进行
// tlbi aside1, xt  xt= [ASID][RES0]
AS_MACRO void asm_tlbi_aside1(uint16_t asid)
{
	// DOCME dsb,dsb,isb这个顺序也是非常重要的
	__asm__ __volatile__(
			"dsb osh;tlbi aside1,%0;dsb osh;isb"
			::"r"(static_cast<uint64_t>(asid)<<(64 - 16))
	);
}
// 重置EL1的所有TLB
// 注意，该指令在EL1不可执行
AS_MACRO void asm_tlbi_allel1()
{
	__asm__ __volatile__("dsb osh;tlbi ALLE1;dsb osh;isb");
}
// 重置EL1的VMALL
AS_MACRO void asm_tlbi_vmallel1()
{
	__asm__ __volatile__("dsb osh;tlbi  VMALLE1;dsb osh;isb");
}




#endif /* INCLUDE_ASM_INSTRUCTIONS_H_ */
