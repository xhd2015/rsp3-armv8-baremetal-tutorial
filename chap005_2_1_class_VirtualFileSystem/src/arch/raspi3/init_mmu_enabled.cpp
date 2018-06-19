/*
 * init_mmu_enabled.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: 13774
 */
#include <arch/aarch64/init.h>
#include <memory/VirtualManager.h>
#include <memory/MemoryManager.h>
#include <runtime_def.h>
#include <driver/gpio/GPIO.h>
#include <arch/aarch64/cxx_macros.h>
#include <io/Output.h>
#include <io/char/PL011Writer.h>
#include <driver/uart/PL011.h>
#include <exception/BCM2835InterruptManager.h>
#include <exception/ExceptionHandler.h>
#include <driver/timer/BCM2835SystemTimer.h>
#include <asm_instructions.h>
#include <filesystem/VirtualFile.h>
#include <filesystem/VirutalFileSystem.h>

extern uint64_t __free_ram_start[];
extern uint64_t __free_ram_end[];
extern uint64_t EXCEPTION_VECTOR[];



 /* part1跳转需要这个符号。因为树莓派3默认
                        loader的地址和kernel的起始地址相同，
                                                               而真正的kernel起始地址在part1中，所以
                                                               首条需要放置一条跳转指令，该指令跳转到part1
                         而part1实际上就放在.bss开始处（有效数据结束后)
                        */
// 而且只能使用绝对跳转，不能直接使用b label，那样会生成错误的指令
// FIXME 查找原因
__asm__(
		".section .text.boot;"
		"ldr x0,=__kernel_start;"
		"br   x0;"
);

__attribute__((section(".text.init")))
void  init_mmu_enabled(size_t addressBits,void* lowFreeRamStart)
{
	new (&virtman) VirtualManager(addressBits);
	virtman.enableTTBR0(false); // 禁用低端地址

	// 重新初始化中断
	RegVBAR_EL1::make(reinterpret_cast<size_t>(EXCEPTION_VECTOR)).write();

	// 内存管理器，与第一阶段的内存管理器链接
	auto headChunkPtr = reinterpret_cast<MemoryChunk *>(__free_ram_start);
	// [headChunk, ----free space----] [lowMemHead,----]
	new (headChunkPtr) MemoryChunk(false,
			pointerInc(reinterpret_cast<char*>(lowFreeRamStart),virtman.highMemBase()) -
		      reinterpret_cast<char *>(__free_ram_start) -
					sizeof(MemoryChunk)
				);
	new (&mman) MemoryManager(__free_ram_start,
			reinterpret_cast<char*>(__free_ram_end) -
				reinterpret_cast<char*>(__free_ram_start),
			false
			);
	size_t highBase = virtman.highMemBase();

	new (&bcm2835IntMan) BCM2835InterruptManager(BCM2835_INTC_BASE + highBase);
	new (&excpHandler) ExceptionHandler<BCM2835InterruptManager>(&bcm2835IntMan);
	new (&sysTimer) BCM2835SystemTimer(SYS_TIMER_BASE + highBase);
	sysTimerTick = 1000000; // 1s

	new (&gpio) GPIO(GPIO_BASE + highBase);
	new (&uart0) PL011(UART0_BASE + highBase);
	new (&uart0Writer) PL011Writer(&uart0);
	new (&kout) Output(&uart0Writer);
	new (&vfs) VirtualFileSystem();


	kout << INFO <<"init_mmu_enabled.\n";

	auto sub  = new VirtualFile("sub");
	auto sub2 = new VirtualFile("sub2");
	vfs.addRootFile(sub);
	vfs.addRootFile(sub2);

	kout << "files:\n";
	vfs.rootFile()->foreachFile([](VirtualFile * f){
		kout << "    " << f->name() << "\n";
	});

	// 不能返回
	while(true){asm_wfi();}
}


