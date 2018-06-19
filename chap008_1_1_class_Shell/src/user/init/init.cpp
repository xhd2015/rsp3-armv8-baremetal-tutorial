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

int main(int argc, char **argv);
extern size_t __bss_start[];
extern size_t __bss_end[];
//extern size_t __free_ram_start[];
extern size_t __free_ram_end[];


__asm__(
		".section .text.header ;"
		".8byte    6 ;" // 头部的域数量，不包括其本身
		".8byte    __boot_start ;"
		".8byte    __code_size, __kernel_stack_size,__user_stack_size,__ram_size ;"
		".8byte    __free_ram_offset;"
		);
__attribute__((section(".text.init")))
void init(Pid id,void * realFreeRamStart, int argc, char **argv)
{
	for(auto p=__bss_start;p!=__bss_end;++p)
		*p=0;

	new (&mman) MemoryManager(realFreeRamStart,
			reinterpret_cast<size_t>(__free_ram_end)-
				reinterpret_cast<size_t>(realFreeRamStart),
			true);
	new (&characterWriter) UserModeCharacterWriter(512);
	new (&kout) Output(&characterWriter);
	new (&characterReader) UserModeCharacterReader(512);
	new (&kin) Input(&characterReader);

	::pid = id;

	int res=main(argc,argv);
	kout.flush();
	svc_call<SvcFunc::killProcess>(PID_CURRENT,static_cast<uint64_t>(res));
}

