/*
 * init.h
 *
 *  Created on: Jun 10, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_ARCH_AARCH64_INIT_H_
#define INCLUDE_ARCH_AARCH64_INIT_H_

#include <generic/programming.h>

#define ASM_ERET_FROM(el,spsr) \
	  "mov x0,sp \n\t" \
	  "msr sp_el1, x0 \n\t" \
	  "ldr x0,=" __stringify(spsr) "\n\t"                  \
	  "msr spsr_el" __stringify(el) ",x0 \n\t"             \
	  "adr x0,1f \n\t"                                     \
	  "msr elr_el" __stringify(el)  ",x0 \n\t"             \
	  "eret \n\t"                                          \
	  "1: \n\t"



#endif /* INCLUDE_ARCH_AARCH64_INIT_H_ */
