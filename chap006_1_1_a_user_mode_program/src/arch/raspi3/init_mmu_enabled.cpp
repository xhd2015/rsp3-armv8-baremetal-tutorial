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
#include <filesystem/RAMVirtualFile.h>

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


	using _D = VirtualMemoryDefinition;
	// user_main函数声明
	extern void user_main(size_t id);
	extern uint64_t __user_start[];
	extern uint64_t __user_end[];
	// 申请一组页表，用户态进程包括：
	//   起始4KB留空，代码ncode*4KB，用户态栈4KB,内核态栈4KB,可用空闲内存nmem*4KB
	//   内核态栈不容易溢出，用户态栈容易溢出。

	// 为了描述这些，我们引入VirtualMap用于将一段物理内存映射到虚拟地址空间
	size_t vaddrStart = _D::PAGE_SIZE;// 前面的4KB留空
	size_t codeSize = reinterpret_cast<size_t>(__user_end) -
				reinterpret_cast<size_t>(__user_start);
	size_t kernelStackSize = _D::PAGE_SIZE;
	size_t stackSize = _D::PAGE_SIZE;
	size_t freeRamSize = _D::PAGE_SIZE;
	size_t memSize = kernelStackSize + stackSize + freeRamSize;

	// 需要将code映射到vaddrStart
	// 将 kernelStackSize,stackSize,freeRamSize映射到vaddrStart + codeSize上
	// 给定3级页表的映射。

	void * mem = mman.allocate(memSize, _D::PAGE_SIZE);
	void * tables = mman.allocate(4*_D::PAGE_SIZE,_D::PAGE_SIZE); // 4级页表
	assert(mem && tables);

	auto temp=reinterpret_cast<char*>(tables);
	auto l0 = reinterpret_cast<Descriptor4KBL0*>(temp);
	auto l1 = reinterpret_cast<Descriptor4KBL1*>(temp + _D::PAGE_SIZE);
	auto l2 = reinterpret_cast<Descriptor4KBL2*>(temp + 2*_D::PAGE_SIZE);
	auto l3 = reinterpret_cast<Descriptor4KBL3*>(temp + 3*_D::PAGE_SIZE);

	// 因为l0,l1,l2,l3都是虚拟地址，需要将其转换成物理地址
	auto pl0 = virtman.translateVAToPA(l0);
	auto pl1 = virtman.translateVAToPA(l1);
	auto pl2 = virtman.translateVAToPA(l2);
	auto pl3 = virtman.translateVAToPA(l3);
	auto pacode = virtman.translateVAToPA(__user_start);
	auto pamem = virtman.translateVAToPA(mem);
	auto pastack = pointerInc(pamem, stackSize);
	auto pakernelStack = pointerInc(pastack, kernelStackSize);
	auto paFreeRam = pointerInc(pakernelStack, freeRamSize);
	// 设置L0
	Descriptor4KBL0::make(l0,0);
	l0->IsTable=1;
	l0->Valid = 1;
	l0->NextLevelTableAddr =
			reinterpret_cast<size_t>(pl1)/_D::PAGE_SIZE ;
	// 设置L1
	Descriptor4KBL1::make(l1 ,0);
	l1->S1.IsTable=1;
	l1->S1.Valid=1;
	l1->S1.NextLevelTableAddr =
			reinterpret_cast<size_t>(pl2)/_D::PAGE_SIZE ;

	// 设置L2
	Descriptor4KBL2::make(l2,0);
	l2->S1.IsTable=1;
	l2->S1.Valid=1;
	l2->S1.NextLevelTableAddr =
			reinterpret_cast<size_t>(pl3)/_D::PAGE_SIZE ;


	// 设置L3
	// 0：未映射 1.代码，只读   2.内核栈    3.用户态栈  4.可用内存
	// 这样，当栈溢出时，会直接报错。特别是内核态栈，直接导致系统崩溃。
	// 如果直接使用高端地址的内核态栈，则栈溢出不会出现错误。
	size_t entryNumOfStart = vaddrStart/_D::PAGE_SIZE;
	size_t entryNumOfCode = codeSize/_D::PAGE_SIZE;
	size_t entryNumOfKernelStack = kernelStackSize/_D::PAGE_SIZE;
	size_t entryNumOfStack = stackSize/_D::PAGE_SIZE;
	size_t entryNumOfFreeRam = freeRamSize/_D::PAGE_SIZE;

	Descriptor4KBL3 * entry = l3;


	for(size_t i=0;i!=entryNumOfStart;++i,++entry)
		Descriptor4KBL3::make(entry,0);

	for(size_t i=0;i!=entryNumOfCode;++i,++entry)
	{
		Descriptor4KBL3::make(entry,0);
		entry->Valid=1;
		entry->AF=1;
		entry->RES1_0=1;
		entry->nG = 1;// 非全局的
		entry->NS = 1;
		entry->AttrIndex = _D::MEMORY_ATTR_NORMAL;
		entry->AP = 0b11;// bit1: set read-only(1), or read-write(0) ,
							//  bit0: not from EL0(0) or else(1)
							// RO,EL0
		entry->OutputAddr =
				reinterpret_cast<size_t>(pacode)/_D::PAGE_SIZE + i;
	}
	for(size_t i=0;i!=entryNumOfKernelStack;++i,++entry)
	{
		Descriptor4KBL3::make(entry,0);
		entry->Valid=1;
		entry->AF=1;
		entry->RES1_0=1;
		entry->nG = 1;// 非全局的
		entry->NS = 1;
		entry->AttrIndex = _D::MEMORY_ATTR_NORMAL;
		entry->AP = 0b00;//RW,EL1
		entry->OutputAddr =
					reinterpret_cast<size_t>(pakernelStack)/_D::PAGE_SIZE + i;
	}

	for(size_t i=0;i!=entryNumOfStack;++i,++entry)
	{
		Descriptor4KBL3::make(entry,0);
		entry->Valid=1;
		entry->AF=1;
		entry->RES1_0=1;
		entry->nG = 1;// 非全局的
		entry->NS = 1;
		entry->AttrIndex = _D::MEMORY_ATTR_NORMAL;
		entry->AP = 0b01; //RW,EL0
		entry->OutputAddr =
					reinterpret_cast<size_t>(pastack)/_D::PAGE_SIZE + i;
	}
	// 更加高效的做法是，先构造一个模板，将其放在寄存器中，然后复制到其他部分，最后逐个更改地址
	for(size_t i=0;i!=entryNumOfFreeRam;++i,++entry)
	{
		Descriptor4KBL3::make(entry,0);
		entry->Valid=1;
		entry->AF=1;
		entry->RES1_0=1;
		entry->nG = 1;// 非全局的
		entry->NS = 1;
		entry->AttrIndex = _D::MEMORY_ATTR_NORMAL;
		entry->AP = 0b01;//RW,EL0
		entry->OutputAddr =
					reinterpret_cast<size_t>(paFreeRam)/_D::PAGE_SIZE + i;
	}

	auto ttbr0 = RegTTBR0_EL1::make(reinterpret_cast<size_t>(pl0));
	auto spsr = RegSPSR_EL1::make(0);
	auto elr = RegELR_EL1::make(vaddrStart);
	// sp指向top
	auto spel1 = vaddrStart + codeSize + kernelStackSize;
	auto spel0 = RegSP_EL0::make(spel1 + stackSize);

	spsr.IRQMask = 1;
	spsr.FIQMask = 1;
	spsr.DebugMask = 1;
	spsr.SErrorMask = 1; // 禁用中断

	virtman.enableTTBR0(true);

	ttbr0.write();
	spsr.write();
	spel0.write();
	elr.write();
	size_t id=1;
	__asm__ __volatile__(
			"mov sp,%0 ;" // 设置SP_EL1
			"mov x0,%1 ;"
			"eret ;"
			:
			:"r"(spel1),
			 "r"(id)
			 :"x0","sp"
		);

	// 不能返回
	while(true){asm_wfi();}
}


