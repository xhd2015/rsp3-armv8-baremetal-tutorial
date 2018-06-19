/*
 * Progress.cpp
 *
 *  Created on: Mar 16, 2018
 *      Author: 13774
 */


#include <schedule/Process.h>
#include <memory/VirtualManager.h>
#include <schedule/PidManager.h>
#include <runtime_def.h>
#include <exception/exception_def.h>
#include <asm_instructions.h>


Process::Process(ProcessLink *     parent,size_t codePages,size_t kernelStackPages,
		size_t userStackPages, size_t freeRamPages)
	:_ttbr0(),
	 _spel0(),
	 _spel1(0),
	 _elr(),
	 _spsr(),
	 _pid(pidManager.allocate()),
	 _status(CREATED),
	 _parent(parent),
	 _vmap(codePages,kernelStackPages,userStackPages,freeRamPages),
	 _exitCode(0)
{

	_spsr = RegSPSR_EL1::make(0);
	_ttbr0 = RegTTBR0_EL1::make(
			reinterpret_cast<size_t>(virtman.translateVAToPA(_vmap.tableAddr(0)))
			);
	_elr = RegELR_EL1::make(
			_vmap.VAPageStartOf(ProcessVirtualMap::R_CODE)*_D::PAGE_SIZE);
	// sp指向top
	_spel0 = RegSP_EL0::make(
			_vmap.VAPageStartOf(ProcessVirtualMap::R_USTACK)*_D::PAGE_SIZE);
	_spel1 = _vmap.VAPageStartOf(ProcessVirtualMap::R_KSTACK)*_D::PAGE_SIZE;

}

// FIXME 具体的析构过程是怎样的？
Process::~Process()
{
	if(_status == Process::Status::DESTROYED)
		return;
	//取出_ttbr0的asid
	asm_tlbi_aside1(_ttbr0.ASID);

	_status = Process::Status::DESTROYED;
}
void Process::saveContextAndChange(Process &proc)
{
	_ttbr0.update();
	_spel0.update(); // spel0, spsr,ttbr0,spel1都是随时变化的

	_spsr = RegSPSR_EL1::make(asm_getPSTATE());
	// FIXME spsr命名为pstate更加适合。，这里,_spsr不应当使用update
	// _spsr.update(); // spsr实际上应当从pstate直接复制，现在问题转变成，
					// 怎么获取pstate的值？
	// DOCME 将PSTATE保存到SPSR的途径只有一个：中断发生。
	//       然而根据ARMv8的文档,PSTATE保存到SPSR时，大部分值是不变的
	//       改变的包括：4个屏蔽位设置，EL设置成目标位,单步取消，非法异常取消，SPSel=1,nRW=0
	//   因此， 只需按照这个过程将spsr设置，即可得到PSTATE的最初值。
	// PSTATE相关： EL, masks,SPSel,NCZV
	proc._elr.write();
	proc._spsr.write();
	proc._spel0.write();

	// 这里其实展示了如何中断进程，如果中断指令一样，进程丝毫不会感知自己被中断了。
	// 保存通用寄存器，保存内核栈指针，设置下一次返回地址为enter_again
	// 在这一个过程中，不能使用局部变量，因为栈是不一致的
	__asm__(
			// 当前进程{
			//   当前进程的TTBR0,SPSR,SP_EL0已保存
			//   保存当前进程的返回地址
			"adr x0,enter_again ;"
			"str x0,[%3] ;"

			//   保存当前进程的内核栈地址
			"mov x0,sp ;"
			"str x0,[%4] ;"

			//   将通用寄存器保存到_registers中
			"mov sp,%0 ;"
			SAVE_REGS_ASM_INSTR()  // sp值
			// }

			// 目标进程{
			//  目标进程的ELR、SPSR，SP_EL0已写入
			// 更改TTBR0
			// 为什么TTBR0只能在这儿写入呢？
			// 因为TTBR0改变之后，意味着低端地址空间不可用了。因此所有的内存访问必须位于高端地址空间
			"msr ttbr0_el1,%5 ;"
			ASM_TLBI_VMALLEL1_INSTR() // FIXME 可以通过判断_ttbr0更新与否来选择是否重置

			//  设置目标进程的内核态指针
			"mov sp, %1 ;"

			//   将目标进程的通用寄存器还原
			"mov x30,%2 ;"
			RESTORE_REGS_ASM_INSTR_X30_BASE()
			"eret ;"
			// }

			// 此处，通用寄存器已经还原
			"enter_again: ;"  // 进程重新进入的地址
//			"1: b 1b ;"
			:
			:"r"(_registers + REGISTER_NUM),"r"(proc._spel1),
			 	 "r"(proc._registers),"r"(&_elr),"r"(&_spel1),
				 "r"(proc._ttbr0)
			 :"x0","x30","sp"
	);
}

void Process::restoreContextAndExecute()
{
	_ttbr0.write();
	asm_tlbi_vmallel1(); // FIXME 可以通过判断_ttbr0更新与否来选择是否重置
	_elr.write();
	_spsr.write();
	_spel0.write();

	// 从低地址开始还原, 绝对不会发生同步异常
	__asm__ __volatile__(
		"mov  sp,  %1 ;"
		"mov  x30, %0 ;"
		RESTORE_REGS_ASM_INSTR_X30_BASE()
		"eret ;"
		::"r"(_registers),"r"(_spel1)
		:"sp"
	);

}
const char * Process::statusToString(Status s)
{
	switch(s)
	{
	case Status::BLOCKED:
		return "BLOCKED";
	case Status::CREATED:
		return "CREATED";
	case Status::CREATED_INCOMPLETE:
		return "CREATED_INCOMPLETE";
	case Status::READY:
		return "READY";
	case Status::DESTROYED:
		return "DESTROYED";
	case Status::RUNNING:
		return "RUNNING";
	case Status::STOPPED:
		return "STOPPED";
	default:
		return "";
	}
}
