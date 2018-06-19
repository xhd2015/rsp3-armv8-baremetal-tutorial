/*
 * ProcessManager.cpp
 *
 *  Created on: Mar 17, 2018
 *      Author: 13774
 */

#include <memory/MemoryManager.h>
#include <schedule/ProcessManager.h>
#include <asm_instructions.h>
#include <runtime_def.h>

ProcessManager::ProcessManager()
	:_running(nullptr)
{
	for(size_t i=0;i!=PROCESS_LIST_NUM;++i)
		new (_statedProcess + i) ProcessList();
}

ProcessLink* ProcessManager::findProcess(Process::Status status,Pid pid)
{
	auto p=_statedProcess[statusToListType(status)].head();
	while(p && p->dataRef().pid()!=pid)
		p=p->next();
	if(p)
		return p;

	return nullptr;
}

ProcessLink* ProcessManager::findAliveProcess(Pid pid)
{
	if(pid==PID_CURRENT)
		return currentRunningProcess();
	else if(pid==PID_PARENT || pid==PID_INVALID)
		return nullptr;
	auto p=findProcess(Process::READY, pid);
	if(!p)
		p=findProcess(Process::BLOCKED, pid);
	return p;
}

// FIXME killProcess应当适时清除DESTROYED的进程
void ProcessManager::killProcess(ProcessLink* p)
{
	auto status=p->dataRef().status();//old status
	if(status!=Process::DESTROYED)
	{
		// 调用析构函数之后，进程处于DESTROYED状态，但是进程所在的实际组没有变。
		p->dataRef().~Process();
		// DOCME
		// 进程的所有子进程应当重置它们的父进程
		// 有的进程可能会等待某一个特定的进程，当存在至少一个进程等待进程时，进程就应当保留退出状态
		// 因此将进程移到DESTROYED中而不是直接销毁
		changeProcessStatus(p,status,Process::DESTROYED);

		// FIXME 采用更加高效的方式实现对parent域的改变
		//       但是效率和复杂度之间总存在一个折中，如果
		//       希望更高的效率，显然需要在Process中增加
		//       一个链表记录所有的子进程，则开销更大
		//		changeProcessParent(p, p->dataRef().parent());
	}

}

void     ProcessManager::scheduleNextProcess(Process::Status swappedOutStatus)
{
	auto cur = currentRunningProcess();
	auto next = nextReadyProcess();

	// 进入idle的前提条件： RUNNING为空
	// 无就绪进程 --> 等待中断发生 wfi
	// 中断发生     --> 系统处于idle状态  --> 直接调度就绪的进程
	//                             若无就绪进程，则继续等待
	// idle进程状态：没有任何就绪进程和正在运行的进程
	//   退出idle的可能： 输入中断-->唤醒某个进程
	//				     持续检查READY队列，直到有为止
	//  因此，idle应当： 只允许输入中断，允许IRQ中断，等待100us，禁用IRQ中断，检查READY队列
	//  因为ProcessManager应当是通用的，所有只允许输入中断这件事情应当由中断处理器完成。

	if(!next)
	{
		if(cur) // 没有下一个就绪的，且有当前进程，则继续运行
		{
//			kout << "schedule with current process continuing\n";
			return;// 直接从中断现场返回
		}else{
			// 系统闲置
			kout << INFO << "schedule with idle \n";
			while(true){asm_wfi();}
		}
	}else{// 存在这种情况：当前进程已经被杀死，系统暂时处于无正在运行进程状态，但却有就绪进程
		kout << "schedule with next ready process,from [";
		if(cur)
			kout << cur->dataRef().pid();
		else
			kout << "-";
		kout << "] to [" << next->dataRef().pid() << "].\n";

		if(cur)
			changeProcessStatus(cur, swappedOutStatus);
		changeProcessStatus(next, Process::RUNNING);

		if(cur)
			cur->dataRef().saveContextAndChange(next->dataRef());
		else
			next->dataRef().restoreContextAndExecute();
	}
}

void          ProcessManager::changeProcessStatus(ProcessLink *p, Process::Status oldStatus,Process::Status newStatus)
{
	if(oldStatus==newStatus)
		return ;
	if(oldStatus!=Process::RUNNING)
		_statedProcess[statusToListType(oldStatus)].removeNode(p);
	else
		_running=nullptr;
	if(newStatus!=Process::RUNNING)
		_statedProcess[statusToListType(newStatus)].insertTail(p);
	else
		_running=p;
	p->dataRef().status(newStatus);
}
void          ProcessManager::changeProcessStatus(ProcessLink *p, Process::Status newStatus)
{
	changeProcessStatus(p, p->dataRef().status(),newStatus);
}


void          ProcessManager::printProcessInformation(Output &out)const
{
	out << "Pid     " << "Status   \n";
	printPorcessInformation(out,_running);
	printPorcessInformation(out,_statedProcess[P_READY]);
	printPorcessInformation(out,_statedProcess[P_BLOCKED]);
}

void         ProcessManager::printPorcessInformation(Output &out,const ProcessLink *p)const
{
	if(p)
	{
		out << p->dataRef().pid() << "      " <<
				Process::statusToString(p->dataRef().status()) << "\n";
	}
}
void         ProcessManager::printPorcessInformation(Output &out,const ProcessList &plist)const
{
	auto p=plist.head();
	while(p)
	{
		printPorcessInformation(out, p);
		p=p->next();
	}
}


ProcessManager::ProcessListType  ProcessManager::statusToListType(
		Process::Status status)
{
	switch(status)
	{
	case Process::READY:
		return P_READY;
	case Process::BLOCKED:
		return P_BLOCKED;
	case Process::DESTROYED:
		return P_DESTROYED;
	default:
		return P_READY; // impossible
	}
}
