/*
 * ExceptionHandler.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#include <exception/ExceptionHandler.h>
#include <exception/ExceptionClass.h>
#include <arch/aarch64/system_common_registers.h>
#include <generic/error.h>
#include <driver/uart/PL011.h>
#include <schedule/ProcessManager.h>
#include <data/Queue.h>
#include <runtime_def.h>
#include <asm_instructions.h>
#include <data/String.h>

#define TEMPLATE_ExceptionHandler template <class IntManager>
#define TEMPLATED_ExceptionHandler ExceptionHandler<IntManager>

TEMPLATE_ExceptionHandler
TEMPLATED_ExceptionHandler::ExceptionHandler(IntManager * intman)
	:_intm(intman)
{}

TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handle(
		uint64_t  * savedRegs,
		ExceptionType type,
		ExceptionOrigin origin,
		IntID         id
		)
{
//	kout << INFO << "Handling interrupt\n";
//	kout << "exception type = " ;
//	switch(type)
//	{
//	case ExceptionType::IRQ:
//		kout << "IRQ";
//		break;
//	case ExceptionType::SYNC:
//		kout << "SYNC";
//		break;
//	default:
//		kout << "Other";
//		break;
//	}
//	kout << "\n";
//	kout << "exception origin = ";
//	switch(origin)
//	{
//	case ExceptionOrigin::CUR_SP_EL0:
//	case ExceptionOrigin::CUR_SP_ELx:
//		kout << "Current";
//		break;
//	case ExceptionOrigin::FROM_LOWER_A64:
//		kout << "Lower";
//		break;
//	default:
//		kout << "Other";
//		break;
//	}
//	kout << "\n";
//	RegELR_EL1::read().dump();
//	RegESR_EL1::read().dump();
//	RegFAR_EL1::read().dump();

	// 保存目前的中断状态
	auto spsr = RegSPSR_EL1::read();
	auto elr = RegELR_EL1::read();

	switch(type)
	{
	case ExceptionType::IRQ:
	{
		// 这里需要提供intid
		//The CPU interface has two IARs. Reading the IAR returns the INTID, and advances the interrupt
		//state machine. In a typical interrupt handler, one of the first steps when handling an interrupt is to
		//read one of the IARs
		handleIRQ(id);//NOTE:by reading it, we  acknowledged it.So it will change to 1023 after this read
		break;
	}
	case ExceptionType::FIQ:
	{
		handleFIQ(id);
		break;
	}
	case ExceptionType::SError:
	{
		handleSError();
		break;
	}
	case ExceptionType::SYNC:
	{
		auto esr = RegESR_EL1::read();
		switch(esr.EC)
		{
		case ExceptionClass::UNDEF_INST:
			handleUndefinedInstruction();
			break;
		case ExceptionClass::SVC_AA64:
		{
			handleSVC(savedRegs,static_cast<SvcFunc>(esr.ISS & 0xFFFF));
			break;
		}
		case ExceptionClass::DATA_ABORT_LOWER_EL:  // user error
		case ExceptionClass::DATA_ABORT_SAME_EL: // system error
			handleDataAbort();
			break;
		case ExceptionClass::INSTR_ABORT_LOWER_EL:
		case ExceptionClass::INSTR_ABORT_SAME_EL:
			handleInstructionAbort();
			break;
		case ExceptionClass::SP_ALIGNMENT_FAULT:
			handleSPAlignmentFault();
			break;
		case ExceptionClass::PC_ALIGNMENT_FAULT:
			handlePCAlignmentFault();
			break;
		case ExceptionClass::SERROR_INTERRUPT:
			handleSError();
			break;
		default:
			unhandledException();
			break;
		}
		break;
	}
	case ExceptionType::DEBUG:
	{
		break;
	}
	}
	spsr.write();
	elr.write();
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleUndefinedInstruction()
{
	auto elr = RegELR_EL1::read();
	elr.dump();
	RegSPSR_EL1::read().dump();
	RegESR_EL1::read().dump();

	auto instr = reinterpret_cast<uint32_t*>(elr.returnAddr);
	for(size_t i=0;i!=10;++i)
		kout << "["<< Hex(instr+i)<<"]:" << Hex(instr[i]) << "\n";


	reportError("undefined instruction, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleInstructionAbort()
{
	RegELR_EL1::read().dump();
	RegSPSR_EL1::read().dump();
	RegESR_EL1::read().dump();
	reportError("instruction abort, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleDataAbort()
{
	RegELR_EL1::read().dump();
	RegSPSR_EL1::read().dump();
	RegESR_EL1::read().dump();
	reportError("data abort, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleSPAlignmentFault()
{
	reportError("SP alignment fault, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handlePCAlignmentFault()
{
	reportError("PC alignment fault, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleSVC(size_t  * savedRegs,SvcFunc func)
{
	switch(func)
	{
	case SvcFunc::puts:
	{
		auto cur=processManager.currentRunningProcess();
		auto str = reinterpret_cast<const char*>(savedRegs[0]);
		auto num = savedRegs[1];
		savedRegs[0] = cur->dataRef().out().print(str,num);
		break;
	}
	case SvcFunc::gets:
	{
		handleSysCallGets(savedRegs);
		break;
	}
	case SvcFunc::createShell:
	{
		handleSysCallCreateShell(savedRegs);
		break;
	}
	case SvcFunc::killProcess:
	{
		handleSysCallKillProcess(savedRegs);
		break;
	}
	case SvcFunc::ps:
	{
		processManager.printProcessInformation(
				processManager.currentRunningProcess()->dataRef().out());
		break;
	}
	case SvcFunc::pwd:
	{
		handleVFSPwd(savedRegs);
		break;
	}
	case SvcFunc::cd:
	{
		handleVFSCd(savedRegs);
		break;
	}
	case SvcFunc::ls:
	{
		handleVFSLs(savedRegs);
		break;
	}
	case SvcFunc::createFile:
	{
		handleVFSCreateFile(savedRegs);
		break;
	}
	case SvcFunc::moveFile:
	{
		handleVFSMoveFile(savedRegs);
		break;
	}
	case SvcFunc::copyFile:
	{
		handleVFSCopyFile(savedRegs);
		break;
	}
	case SvcFunc::cat:
	{
		handleVFSCat(savedRegs);
		break;
	}
	}
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleIRQ(IntID id)
{
	if(id == _intm->standardIntID(StandardInterruptType::INPUT))
	{
		fillInputBuffer();
		if(uart0.readReady())
		{
			kout << WARNING << "input buffer is full,extra inputs are discarded.\n";
			// 丢弃多余的字符
			while(uart0.readReady())
				uart0.rawRead();
		}

		// 唤醒等待程序
		auto cur=processManager.currentRunningProcess();
		if(cur!=activeInputCatcher)
			processManager.changeProcessStatus(activeInputCatcher,
					Process::READY);
		_intm->endInterrupt(ExceptionType::IRQ,id);
	}else if(id == _intm->standardIntID(StandardInterruptType::PROCESS_TIMER))
	{
		_intm->endInterrupt(ExceptionType::IRQ,id);
		processManager.scheduleNextProcess();
	}
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleFIQ(IntID id)
{
	kout << "handleFIQ(" << id << ").\n";
	_intm->endInterrupt(ExceptionType::FIQ,id);
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleSError()
{
	reportError("SError, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::unhandledException()
{
	reportError("Unhandled exception.");
}

TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleSysCallKillProcess(size_t  * savedRegs)
{
	auto plink = processManager.findAliveProcess(savedRegs[0]);
	if(plink)
	{
		auto & proc = plink->dataRef();
		kout << INFO << "kill process ["<< proc.pid() << "].\n";

		// 当前进程是交互进程时，应当将交互返回到父进程
		if(plink==activeInputCatcher)
			activeInputCatcher=proc.parent();

		// DOCME 在杀死进程这段时间内，应当禁止进程调度
		//       并且，如果杀死当前进程，应当暂时禁止内存分配
		//       因为，当前进程的内核栈已经标记为回收了
		//       但是实际上内核在调度之前仍需要使用这个栈
		//       进程杀死后，内核栈立即切换到最初使用的栈，然后进入调度程序。
		//       调度程序一定不会返回
		if(plink==processManager.currentRunningProcess()) // 当前系统栈需要切换出去
		{
			processManager.killProcess(plink);
			__asm__ __volatile__(
					"ldr  x0,=__stack_top ;"
					"mov sp,x0 ;"
					"mov x0, %0 ;"
					"br  %1 ;"
					:
					:"r"(&processManager),
					 "r"(&ProcessManager::scheduleNextProcess)
					 :"x0","sp"
			);
			processManager.scheduleNextProcess();
		}else{
			processManager.killProcess(plink);
		}
	}
}


TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleSysCallGets(size_t  * savedRegs)
{
	auto cur = processManager.currentRunningProcess();
	size_t completed = 0;//已经完成的数目
	if(activeInputCatcher==cur) // 仅当当前进程是交互进程时可以读取缓冲区
	{
		auto buff = reinterpret_cast<Queue<uint16_t>*>(savedRegs[0]);
		if(buff && buff->empty()) // 为了安全buff必须为空，因为buff不为空时，数据可能正在被操作
		{
			size_t maxNum = savedRegs[1];
			bool   block = savedRegs[2];
			fillInputBuffer();
			// 阻塞式gets的模型： user请求gets --> 当前没有输入 --> 进程阻塞，切换到其他进程 -->
								//               在某个地方，进程被唤醒 --> 进程检查inputBuffer，直到有数据
			if(block)
				while(inputBuffer.empty())
					processManager.scheduleNextProcess(Process::BLOCKED);
			while(completed<maxNum &&
					!inputBuffer.empty() &&
					!buff->full()) // 读取所有
			{
				buff->put(inputBuffer.remove());
				++completed;
			}
		}
	}

	savedRegs[0]=completed;// 返回
}

TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleSysCallCreateShell(size_t *savedRegs)
{
	// shell code is somewhere in the system
	kout << INFO << "create shell.\n";

	auto cur = processManager.currentRunningProcess();
	auto fg_or_bg = savedRegs[1];
	if(fg_or_bg==0 && cur != activeInputCatcher)//前台进程条件不满足,当前进程不能改变交互进程的状态
	{
		savedRegs[0]=PID_INVALID;
		return;
	}

	using _D = VirtualMemoryDefinition;

	extern size_t __user_code_start[];
	extern size_t __user_code_end[];
	extern size_t __user_init_ram_start[];
	extern size_t __user_init_ram_end[];

	size_t userCodeSize = reinterpret_cast<size_t>(__user_code_end) -
			 reinterpret_cast<size_t>(__user_code_start);

//	".8byte    5 ;" // 头部的域数量，不包括其本身
//			".8byte    __boot_start ;"
//			".8byte    __code_size, __kernel_stack_size,__user_stack_size,__ram_size ;"
//			".8byte __free_ram_offset;"
	size_t headNum = __user_code_start[0];
	assert(headNum == 6);
	size_t bootStart = __user_code_start[1];
	size_t kernlStackSize = __user_code_start[3];
	size_t userStackSize = __user_code_start[4];
	size_t ramSize = __user_code_start[5];
	size_t freeRamOffset = __user_code_start[6];

	// 为了描述这些，我们引入VirtualMap用于将一段物理内存映射到虚拟地址空间

	size_t codePages = reinterpret_cast<size_t>(userCodeSize)/_D::PAGE_SIZE;
	auto procLink = processManager.createNewProcess(
			cur,codePages,kernlStackSize/_D::PAGE_SIZE,
			userStackSize/_D::PAGE_SIZE,ramSize/_D::PAGE_SIZE);
	if(procLink)
	{
		auto & proc=procLink->dataRef();
		proc.ELR().returnAddr = bootStart;
		auto &pvmap = proc.virtualMap();
		pvmap.rebase(ProcessVirtualMap::R_CODE,__user_code_start,false);
		pvmap.renew(ProcessVirtualMap::R_KSTACK);
		pvmap.renew(ProcessVirtualMap::R_USTACK);
		char* rammem = reinterpret_cast<char*>(
				pvmap.renew(ProcessVirtualMap::R_FREE_RAM));
		// 复制initram
		std::memcpy(rammem,__user_init_ram_start,
				reinterpret_cast<size_t>(__user_init_ram_end) -
				reinterpret_cast<size_t>(__user_init_ram_start));

		auto ramva = pvmap.VAPageStartOf(ProcessVirtualMap::R_FREE_RAM)*_D::PAGE_SIZE;
		auto freeramva = ramva + freeRamOffset;

		// 设置入口参数
		auto &args = *reinterpret_cast<const VectorRef<String>*>(
											savedRegs[0]);

		char **argv = reinterpret_cast<char**>(rammem + freeRamOffset);
		auto strings = rammem + freeRamOffset + args.size()*sizeof(char*);
		auto p = strings;
		for(size_t i=0;i!=args.size();++i)
		{
			argv[i]= reinterpret_cast<char*>(ramva + p - rammem);
			for(size_t j=0;j!=args[i].size();++j)
				*p++=args[i][j];
			*p++='\0';
		}
		auto argstart = reinterpret_cast<size_t>(argv);
		auto argend = alignAhead(reinterpret_cast<size_t>(p), sizeof(size_t));
		auto procRegs = proc.registers();
		procRegs[0]=proc.pid(); // 0 -- pid, 1 -- freeRamStart，2 -- argc
		                       //  3 -- char *argv[]
		procRegs[1] = freeramva + (argend - argstart);
		procRegs[2] = args.size();
		procRegs[3] = freeramva;

		// 进程的内存管理器，内核使用
		size_t usedRamSize = procRegs[1] - ramva;
 		new (&proc.mman()) MemoryManager(
				reinterpret_cast<void*>(procRegs[1]),ramSize - usedRamSize, false);
		// 返回值
		savedRegs[0] = proc.pid();

		// 交互进程切换
		if(fg_or_bg==0)//前台进程
		{
			inputBuffer.reset(0); // 丢弃所有的输入
			activeInputCatcher = procLink;
		}
	}else{
		savedRegs[0]=PID_INVALID;
	}
}

TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSCd(size_t *savedRegs)
{
	auto path = reinterpret_cast<const VectorRef<String>*>(savedRegs[0]);
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	auto p = curFile->findFile(*path);
	if(p && p->type()==FileType::F_DIRECTORY)
	{
		proc.workingDir(p);
		savedRegs[0]=0;
	}else
		savedRegs[0]=1;
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSPwd(size_t *savedRegs)
{
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	size_t n=0;
	auto p=curFile;
	while(p->parent())
	{
		p = p->parent();
		++n;
	}
	auto &out=proc.out();
	if(n>0)
	{
		for(size_t i=0;i!=n;++i)
		{
			out << "/" << p->name();
			p = p->parent();
		}
	}else{
		out << "/";
	}
	out << "\n";
	savedRegs[0]=0;
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSLs(size_t *savedRegs)
{
	auto path = reinterpret_cast<const VectorRef<String>*>(savedRegs[0]); // nullptr时显示当前目录
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	auto p = curFile;
	if(path)
		p = curFile->findFile(*path);

	if(p && p->type()==FileType::F_DIRECTORY)
	{
		auto &out=proc.out();
		p->foreachFile([&out](VirtualFile *file){
			out << file->name() << " ";
		});
		out << "\n";
		savedRegs[0]=0;
	}else{
		savedRegs[0]=1;
	}
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSCreateFile(size_t *savedRegs)
{
	auto path = reinterpret_cast<const VectorRef<String>*>(savedRegs[0]);
	assert(path->size()==1);//FIXME 目前只在当前路径下创建
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	auto p = curFile->findFile(*path);

	savedRegs[0]=1;//默认失败
	if(!p) //不存在文件
	{
		p = curFile->createFile((*path)[0], static_cast<FileType>(savedRegs[1]));
		if(p)
		{
			curFile->addFile(p);
			savedRegs[0]=0;
		}
	}
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSMoveFile(size_t *savedRegs)
{
	auto pathSrc = reinterpret_cast<const VectorRef<String>*>(savedRegs[0]);
	auto pathDest = reinterpret_cast<const VectorRef<String>*>(savedRegs[1]);
	if(!pathSrc)
	{
		savedRegs[0]=-1;
		return;
	}
	assert(pathSrc->size()==1 && pathDest->size()==1);//FIXME 目前只在当前路径下创建
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	auto p = curFile->findFile(*pathSrc);

	if(!pathDest) // 删除文件
	{
		if(p)
			curFile->removeFile(p);
		savedRegs[0]=0;
		return;
	}

	if(!p)
	{
		savedRegs[0]=1;
		return;
	}
	if(curFile->findFile((*pathDest)[0]))
	{
		savedRegs[0]=1;
		return;
	}
	p->name((*pathDest)[0]);
	savedRegs[0]=0;
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSCopyFile(size_t *savedRegs)
{
	auto pathSrc = reinterpret_cast<const VectorRef<String>*>(savedRegs[0]);
	auto pathDest = reinterpret_cast<const VectorRef<String>*>(savedRegs[1]);
	if(!pathSrc || !pathDest)
	{
		savedRegs[0]=-1;
		return;
	}
	assert(pathSrc->size()==1 && pathDest->size()==1);//FIXME 目前只在当前路径下创建
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	auto p = curFile->findFile(*pathSrc);

	if(!p || p->type()!=FileType::F_FILE)
	{
		savedRegs[0]=1;
		return;
	}
	if(curFile->findFile(*pathDest))
	{
		savedRegs[0]=1;
		return;
	}
	auto f=p->copy();
	if(f)
	{
		f->name((*pathDest)[0]);
		curFile->addFile(f);
		savedRegs[0]=0;
	}else{
		savedRegs[0]=1;
	}
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleVFSCat(size_t *savedRegs)
{
	auto path = reinterpret_cast<const VectorRef<String>*>(savedRegs[0]);
	if(!path){savedRegs[0]=-1;return;}
	auto &proc = processManager.currentRunningProcess()->dataRef();
	auto curFile =proc.workingDir();
	auto p = curFile->findFile(*path);

	if(p && p->type()==FileType::F_FILE)
	{
		proc.out() << p->read(0,p->size()) << "\n";
		savedRegs[0]=0;
	}else{
		savedRegs[0]=1;
	}
}


TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::fillInputBuffer()
{
	while(uart0.readReady() && !inputBuffer.full())
	{
		inputBuffer.put(uart0.rawRead());
//		auto ch=uart0.rawRead();
//		inputBuffer.put(ch);
//		kout << Hex(ch) << "\n";
	}
}

TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::waitReadyProcess()
{
	allowEvent(StandardInterruptType::INPUT, true);
	allowEvent(StandardInterruptType::PROCESS_TIMER, false);
	asm_enable_irq(true);
	while(!processManager.nextReadyProcess()); // FIXME 会不会出现竞争条件？
	asm_enable_irq(false);
	allowEvent(StandardInterruptType::PROCESS_TIMER, true);
}
