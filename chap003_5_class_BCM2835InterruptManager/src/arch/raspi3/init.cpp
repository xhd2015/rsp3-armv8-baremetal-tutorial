#include <cstdio>
#include <generic/util.h>
#include <generic/IntegerFormatter.h>
#include <new> //placement-new
#include <cassert>
#include <arch/aarch64/system_common_registers.h>
#include <arch/aarch64/init.h>
#include <memory/MemoryManager.h>
#include <runtime_def.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <io/char/BCM2835MiniUARTWriter.h>
#include <driver/gpio/GPIO.h>
#include <exception/InterruptManager.h>
#include <exception/ExceptionHandler.h>
#include <driver/timer/BCM2835SystemTimer.h>
#include <exception/BCM2835InterruptManager.h>


constexpr size_t GPIO_BASE = 0x3F200000 ;
constexpr size_t UART1_BASE = 0x3F215000 ;
constexpr size_t SYS_TIMER_BASE = 0x3F003000 ;
constexpr size_t BCM2835_INTC_BASE = 0x3F00B000 ;

constexpr size_t RAM_SIZE = 1024*1024*1024 - 64*1024*1024; // 1GB - 64MB
extern uint64_t __free_ram_start[];
extern uint64_t EXCEPTION_VECTOR[];

extern "C" void init(uint64_t  el);
__asm__(
	".section .text.boot \n\t"
	"mrs     x0, mpidr_el1 \n\t"
	"and     x0, x0, #0b11 \n\t"
	"cmp     x0,#0 \n\t"
	"b.eq      2f \n\t"
	"1:  		\n\t"
	"wfi        \n\t"
	"b       1b	\n\t"
	"2: \n\t"
	"ldr    x0, =__stack_top \n\t"
	"mov    sp,x0 \n\t"
	"mrs    x0,CurrentEL \n\t"
	"lsr    x0,x0,#2  \n\t"
	"bl      init \n\t"
	"b      1b \n\t"
);
__attribute__((section(".text.boot")))
void init(uint64_t  el)
{
	// 初始化输出
	new (&uart1) BCM2835MiniUART(UART1_BASE);
	new (&uart1Writer) BCM2835MiniUARTWriter(&uart1);
	new (&kout) Output(&uart1Writer);

	new (&gpio) GPIO(GPIO_BASE);

	// DOCME 倘若去掉下面这两行，则树莓派不能输出信息
	//       当然，正确的输出还需要适当的初始化
	//       从这里，就可以看出QEMU模拟与树莓派
	//       的差异，前者无需额外设置，后者还需
	//       许多设置。
	gpio.selectAltFunction(14, GPIO::ALT_5);
	gpio.selectAltFunction(15, GPIO::ALT_5);

	uart1.init();

	kout << "highest EL = " << el << "\n";
	if(el==3)
	{
		auto scr=RegSCR_EL3::read();
		scr.RW=1; // aa64
		scr.SIF=0;//allow prefecth from non-secure
		scr.NS=1;// non-secure
		scr.HCE=1; //允许hvc指令
		scr.SMD=0; //允许smc指令
		scr.write();
	}
	if(el>=2)
	{
		auto hcr=RegHCR_EL2::read();
		hcr.RW=1; // aa64
		hcr.VM=0; // disable virtualization
		hcr.TGE=0; // do not trap EL1 exceptions
		hcr.DC=0;// not default cache
		hcr.HCD=0;//enable HVC
		hcr.TSC=0; // do not route SMC to EL2
		hcr.TWI=0;// wfi not routed
		hcr.write();
	}
	if(el==3)
		__asm__(
				ASM_ERET_FROM(3,0x3c5)
		);
	else if(el==2)
		__asm__(
				ASM_ERET_FROM(2,0x3c5)
		);

	RegSCTLR_EL1::make(0x30D00800).write(); // 这是对树莓派3的一个修复。对于QEMU没有影响

	// 在EL1状态，初始化
	//   内存管理器
	new (&mman) MemoryManager(
			__free_ram_start,
			RAM_SIZE - reinterpret_cast<size_t>(__free_ram_start),
			true
	);
	//exception
//	new (&intMan) InterruptManager();
	new (&bcm2835IntMan) BCM2835InterruptManager(BCM2835_INTC_BASE);
	new (&excpHandler) ExceptionHandler<BCM2835InterruptManager>(&bcm2835IntMan);
	new (&sysTimer) BCM2835SystemTimer(SYS_TIMER_BASE);
	sysTimerTick = 1000000; // 1s

	auto currentel = RegCurrentEL::read();
	size_t n=20;
	while(n--)
	{
		kout << "currentEL = " << currentel.EL << "\n";
		delayCPU(1000,1000);
	}

	RegVBAR_EL1 vbar1;
	vbar1.Addr = reinterpret_cast<uint64_t>(EXCEPTION_VECTOR);
	vbar1.write();

	//====测试
	size_t timer=1;
	bcm2835IntMan.disableAllInterrupts();
	bcm2835IntMan.enableInterrupt(
			BCM2835InterruptManager::SRC_SYS_TIMER_FIRST + timer,
			true);
	sysTimer.clearIntFlags(timer);
	sysTimer.addCompareValueUS(timer, sysTimerTick);
	auto daif=RegDAIF::read();
	daif.I=0;
	daif.write();
}

