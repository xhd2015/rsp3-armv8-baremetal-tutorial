/*
 * init.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */
#include <cstddef>
#include <new>
#include <exception/svc_call.h>
#include <runtime_def.h>
#include <io/Output.h>
#include <io/UserModeCharacterWriter.h>
#include <memory/MemoryManager.h>
#include <schedule/forward.h>
#include <io/Input.h>
#include <io/UserModeCharacterReader.h>

int main();
extern size_t __bss_start[];
extern size_t __bss_end[];
extern size_t __free_ram_start[];
extern size_t __free_ram_end[];


__asm__(
		".section .text.header ;"
		".8byte    5 ;" // 头部的域数量，不包括其本身
		".8byte    __boot_start ;"
		".8byte    __code_size, __kernel_stack_size,__user_stack_size,__ram_size ;"
		);
__attribute__((section(".text.init")))
void init(size_t id)
{
	for(auto p=__bss_start;p!=__bss_end;++p)
		*p=0;

	new (&mman) MemoryManager(__free_ram_start,
			reinterpret_cast<size_t>(__free_ram_end)-
				reinterpret_cast<size_t>(__free_ram_start),
			true);
	new (&characterWriter) UserModeCharacterWriter(512);
	new (&kout) Output(&characterWriter);
	new (&characterReader) UserModeCharacterReader(512);
	new (&kin) Input(&characterReader);

	int res=main();
	kout.flush();
	svc_call<SvcFunc::killProcess>(PID_CURRENT,static_cast<uint64_t>(res));
}

