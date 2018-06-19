/*
 * ProgressManager.h
 *
 *  Created on: Mar 17, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_SCHEDULE_PROGRESSMANAGER_H_
#define INCLUDE_SCHEDULE_PROGRESSMANAGER_H_

#include <data/DoublyLinkedList.h>
#include <schedule/Process.h>
#include <generic/programming.h>

class ProcessManager{
private:
	enum ProcessListType { P_READY,P_BLOCKED,P_DESTROYED,
		PROCESS_LIST_NUM };
public:

	ProcessManager();
	DELETE_COPY(ProcessManager);

	// status只能是有效的
	ProcessLink* findProcess(Process::Status status,Pid pid);
	ProcessLink* findAliveProcess(Pid pid);
	void     killProcess(ProcessLink *p);

	void     scheduleNextProcess(Process::Status swappedOutStatus=Process::READY);
	/**
	 * 创建一个新的进程，如果成功，将其加入READY队列
	 * TODO 新增其他状态可能
	 * @param initArgs
	 * @return
	 */
	template <class ... Args>
	ProcessLink*  createNewProcess(Args && ... initArgs);

	// 用于改变进程的状态，注意，不会改变当前正在运行的进程，即_running不受影响
	/**
	 * 前置条件： oldStatus,newStatus均只是RUNNING,READY,BLOCKED,DESTROYED中的一个
	 * @param p
	 * @param oldStatus
	 * @param newStatus
	 */
	void          changeProcessStatus(ProcessLink *p, Process::Status oldStatus,Process::Status newStatus);

	void          changeProcessStatus(ProcessLink *p,Process::Status newStatus);



	void    printProcessInformation(Output &out)const;
	void    printPorcessInformation(Output &out,const ProcessLink *p)const;
	void    printPorcessInformation(Output &out,const ProcessList & plist)const;


	// 如果是单核的，则某一时刻只有一个PID在运行
	// 实际上即使是多核情况下，我们对某个核实际上只赋予一个唯一的正在运行的进程
	AS_MACRO ProcessLink* currentRunningProcess()
			{return _running;}
	AS_MACRO ProcessLink* nextReadyProcess()
	        {return _statedProcess[P_READY].head();}
	AS_MACRO ProcessLink* nextBlockedProcess()
	         {return _statedProcess[P_BLOCKED].head();}

private:
	// _s: 完成status 到 ProcessListType的转换
	static       ProcessListType  statusToListType(Process::Status status);
private:
	ProcessLink * _running;
	ProcessList  _statedProcess[PROCESS_LIST_NUM];
			// 实际有效的仅仅是READY,BLOCKED,DESTROYED
};


//== template
template <class ... Args>
ProcessLink*   ProcessManager::createNewProcess(Args && ... initArgs)
{
	auto node = _statedProcess[P_DESTROYED].insertTail(std::forward<Args>(initArgs)...); // 默认放在destroyed中
	if(node)
	{
		if(node->dataRef().status()==Process::Status::CREATED)
			changeProcessStatus(node, Process::DESTROYED,Process::READY);
		else //创建失败
		{
			_statedProcess[P_DESTROYED].removeNode(node);
			delete node;
			return nullptr;
		}
	}
	return node;
}


#endif /* INCLUDE_SCHEDULE_PROCESSMANAGER_H_ */
