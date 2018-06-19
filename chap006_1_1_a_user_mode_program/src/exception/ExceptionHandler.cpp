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
#include <runtime_def.h>

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
			// FIXME 为用户程序特意修改的。
			auto svcNum = esr.ISS & 0xFFFF;
			if(svcNum==0)
			{
				kout << savedRegs[0];
			}else{
				handleSVC(svcNum);
			}
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
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleUndefinedInstruction()
{
	reportError("undefined instruction, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleInstructionAbort()
{
	reportError("instruction abort, not handled.");
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleDataAbort()
{
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
void TEMPLATED_ExceptionHandler::handleSVC(size_t num)
{
	kout << "handleSVC(" << num << ").\n";
}
TEMPLATE_ExceptionHandler
void TEMPLATED_ExceptionHandler::handleIRQ(IntID id)
{
	kout << "handleIRQ(" << id << ").\n";
	if(id == _intm->standardIntID(StandardInterruptType::INPUT))
	{
		size_t n=0;
		while(uart0.readReady())
		{
			auto ch=uart0.rawRead();
			kout << "reading : "<< static_cast<char>(ch)
					<< " = " << Hex(ch) << "\n";
			++n;
		}
		uart0.clearIntFlags();
		kout << "reading count  = " << n << "\n";
	}else if(id == _intm->standardIntID(StandardInterruptType::PROCESS_TIMER))
	{
		_intm->endInterrupt(ExceptionType::IRQ,id);
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

