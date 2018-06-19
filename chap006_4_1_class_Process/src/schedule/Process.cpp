/*
 * Progress.cpp
 *
 *  Created on: Mar 16, 2018
 *      Author: 13774
 */


#include <schedule/Process.h>
#include <memory/VirtualManager.h>
#include <runtime_def.h>
#include <exception/exception_def.h>
#include <asm_instructions.h>


Process::Process(size_t codePages,size_t kernelStackPages,
		size_t userStackPages, size_t freeRamPages)
	:_ttbr0(),
	 _spel0(),
	 _spel1(0),
	 _elr(),
	 _spsr(),
	 _pid(0),
	 _status(CREATED),
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

//Process::Process(const Process & rhs)
//	:_pid(pidManager.allocate()), // 这些值需要从rhs设置，其他的保留默认值
//	 _priority(rhs._priority),
//	 _status(CREATED_INCOMPLETE),
//	 _parent(processManager.findAliveProcess(rhs.pid())),//TESTME 正确吗
//	 _memory(mman.allocate(rhs._memsize,VirtualMap::_D::PAGE_SIZE)),
//	 _memoryBase(rhs._memoryBase),
//	 _memsize(rhs._memsize),
//	 _pmman(_memory,_memsize,false),
//	 _ttbr0(rhs._ttbr0),
//	 _spEL0(rhs._spEL0),
//	 _ELR(rhs._ELR),
//	 _SPSR(rhs._SPSR),
//	 _vmap(rhs._vmap),
//	 _catchInput(rhs._catchInput),
//	 _inputBuffer(rhs._inputBuffer),
//	 _exitCode(rhs._exitCode)
//{
//	if(_pid == PID_INVALID)
//		return;
//	assert(_memory);//占用的内存不为空
//	std::memcpy(_memory, rhs._memory,_memsize);
//	// 改变L3层所指的页面基地址
//	_vmap.rebaseL3(reinterpret_cast<uint64_t>(_memory)/VirtualMap::_D::PAGE_SIZE);
//	// 赋值寄存器
//	// FIXME 使用高效的memcpy
//	std::memcpy(_registers, rhs._registers, sizeof(_registers));
//	// DOCME 将ttbr0设置为物理地址，这十分重要
//	virtman.setTTBR0Addr(_ttbr0,
//			reinterpret_cast<uint64_t>(_vmap.l0Table()));
//}


void Process::restoreContextAndExecute()
{
	kout << INFO << "Process restore\n";
	_ttbr0.write();
	asm_tlbi_vmallel1(); // FIXME 可以通过判断_ttbr0更新与否来选择是否重置
	_elr.write();
	_spsr.write();
	_spel0.write();

	// 从低地址开始还原, 绝对不会发生同步异常
	__asm__ __volatile__(
		"mov  sp,  %1 \n\t"
		"mov  x30, %0 \n\t"
		RESTORE_REGS_ASM_INSTR_X30_BASE()
		"eret \n\t"
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
