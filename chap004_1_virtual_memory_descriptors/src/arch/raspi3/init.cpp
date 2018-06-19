#include <cstdio>
#include <generic/util.h>
#include <generic/IntegerFormatter.h>
#include <new> //placement-new
#include <cassert>
#include <arch/aarch64/system_common_registers.h>
#include <arch/aarch64/vmsa_descriptors.h>
#include <arch/aarch64/init.h>
#include <memory/VirtualMemoryDefinition.h>
#include <runtime_def.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <io/char/BCM2835MiniUARTWriter.h>
#include <driver/uart/PL011.h>
#include <io/char/PL011Writer.h>
#include <io/Output.h>
#include <driver/gpio/GPIO.h>
#include <memory/MemoryManager.h>
#include <exception/BCM2835InterruptMananger.h>
#include <exception/ExceptionHandler.h>
#include <driver/timer/BCM2835SystemTimer.h>

constexpr size_t GPIO_BASE = 0x3F200000;
constexpr size_t UART1_BASE = 0x3F215000;
constexpr size_t UART0_BASE = 0x3F201000;
constexpr size_t SYS_TIMER_BASE = 0x3F003000;
constexpr size_t BCM2835_INTC_BASE = 0x3F00B000;
constexpr size_t PERIPHBASE = 0x3F000000;
constexpr size_t PERIPHEND = 0x40001000;
constexpr size_t RAM_START = 0;
constexpr size_t RAM_SIZE  = PERIPHBASE;
constexpr size_t FREE_RAM_SIZE = RAM_SIZE - 64*1024*1024;

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
	new (&uart0) PL011(UART0_BASE);
	new (&uart0Writer) PL011Writer(&uart0);
//	new (&kout) Output(&uart1Writer);
	new (&kout) Output(&uart0Writer);

	new (&gpio) GPIO(GPIO_BASE);

	// PL011对应的是ALT0功能
	gpio.selectAltFunction(14, GPIO::ALT_0);
	gpio.selectAltFunction(15, GPIO::ALT_0);

//	uart1.init();
	uart0.init();

	uart0.waitInput();

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
			FREE_RAM_SIZE - reinterpret_cast<size_t>(__free_ram_start),
			true
	);
	//exception
//	new (&intMan) InterruptManager();
	new (&bcm2835IntMan) BCM2835InterruptManager(BCM2835_INTC_BASE);
	new (&excpHandler) ExceptionHandler<BCM2835InterruptManager>(&bcm2835IntMan);
	new (&sysTimer) BCM2835SystemTimer(SYS_TIMER_BASE);
	sysTimerTick = 1000000; // 1s

	auto currentel = RegCurrentEL::read();
	kout << "currentEL = " << currentel.EL << "\n";

	RegVBAR_EL1 vbar1;
	vbar1.Addr = reinterpret_cast<uint64_t>(EXCEPTION_VECTOR);
	vbar1.write();

	using _D = VirtualMemoryDefinition;

	// 树莓派的地址空间概览：
	//   RAM：0 - 0x3F000000
	//   PERI : 0x3F000000 - 0x40001000
	// 最大的级别为2
	size_t maxLevel = 2;
	(void)maxLevel;

	// 计算映射所需的表大小，一次性分配
	size_t mapSize = RAM_SIZE + (PERIPHEND - PERIPHBASE);
	// 先将mapSize对齐到最近的整数大小，
	// 因为单个表项能够映射的必然是mapSizeOfLevel的整数倍
	// ceil(mapSize/_D::mapSizeOfLevel(0))
	size_t memSize0 = _D::memSizeOfMapSize(mapSize, 0);
	size_t memSize1 = _D::memSizeOfMapSize(mapSize, 1);
	size_t memSize2 = _D::memSizeOfMapSize(mapSize, 2);

	void * p0 = mman.allocate(memSize0,_D::PAGE_SIZE);
	void * p1 = mman.allocate(memSize1,_D::PAGE_SIZE);
	void * p2 = mman.allocate(memSize2,_D::PAGE_SIZE);
	assert(p0 && p1 && p2);

	Descriptor4KBL0 * l0 =reinterpret_cast<Descriptor4KBL0*>(p0);
	Descriptor4KBL1 * l1 =reinterpret_cast<Descriptor4KBL1*>(p1);
	Descriptor4KBL2 * l2 =reinterpret_cast<Descriptor4KBL2*>(p2);

	// 设置L0
	for(size_t i=0;i!=memSize0/_D::ENTRY_SIZE;++i)
	{
		Descriptor4KBL0::make(l0 + i,0);
		l0[i].IsTable=1;
		l0[i].Valid = 1;
		l0[i].NextLevelTableAddr =
				reinterpret_cast<size_t>(l1)/_D::PAGE_SIZE + i;
	}
	// 设置L1
	for(size_t i=0;i!=memSize1/_D::ENTRY_SIZE;++i)
	{
		Descriptor4KBL0::make(l1 + i,0);
		l1[i].S1.IsTable=1;
		l1[i].S1.Valid=1;
		l1[i].S1.NextLevelTableAddr =
				reinterpret_cast<size_t>(l2)/_D::PAGE_SIZE + i;
	}

	// 计算RAM和外设的分界线
	size_t periL2Index = PERIPHBASE/_D::maxMapSizeOfLevel(2);

	for(size_t i=0;i!=memSize2/_D::ENTRY_SIZE;++i)
	{
		Descriptor4KBL2::make(l2+i,0);
		l2[i].S0.AF=1; // 通过改变AF的值可以看到
//		l2[i].S0.Contiguous=0;
//		l2[i].S0.IsTable=0;
		if(i < periL2Index) // 设置 RAM部分
			l2[i].S0.AttrIndex = _D::MEMORY_ATTR_NORMAL;
		else  				// 设置 外设部分
			l2[i].S0.AttrIndex = _D::MEMORY_ATTR_PERIPHERAL;
		l2[i].S0.NS=1;
//		l2[i].S0.nG=0;
		l2[i].S0.Valid=1;
		l2[i].S0.OutputAddr=i; // 从0开始
//		l2[i].S0.AP=0b00;//RW, non EL0
		l2[i].S0.SH= 0b11;//OSH
	}

	RegTTBR0_EL1::make(reinterpret_cast<size_t>(l0)).write();
	RegTTBR1_EL1::make(reinterpret_cast<size_t>(l0)).write();

	// 初始化所有的内存属性
	RegMAIR_EL1 mair=RegMAIR_EL1::make(0);
	mair.Attr0 = 0xff; //Normal
//	mair.Attr1 = 0x04; // Device-nGnRE, PERIPHERAL
	mair.Attr1 = 0x00;// nGnRnE
//	mair.Attr1 = 0xff;
	mair.Attr2 = 0x44; //Normal,non-cacheable
	mair.write();

	// 设置TCR，TTBR0,TTBR1均使用4KB映射
	auto tcr = RegTCR_EL1::read();
	// disable address tag
	tcr.TBI0 = tcr.TBI1 = 0;
	// set output address size,same with PARange
	// IPS has 3bits, the extra bit is alwasy RES0
	tcr.IPS = 2; // 设置输出的虚拟地址的范围，包括0级页表。
	// set input address size, TxSZ is the most significant bits
	tcr.T0SZ = tcr.T1SZ = 24;//设置输入的物理地址的范围
	tcr.A1 = 0; //ASID is in ttbr0
	tcr.TG0 = 0b00;//TTBR0 4KB
	tcr.TG1 = 0b10;//TTBR1 4KB
	tcr.SH0 = tcr.SH1 = 0b11;//inner shareable
	tcr.IRGN0 = tcr.IRGN1 = 0b01;//inner cacheable
	tcr.ORGN0 = tcr.ORGN1 =0b01;// outer cacheable
	tcr.EPD0 = tcr.EPD1 = 0; // not disable page walk on TLB miss
	tcr.write();
	__asm__("isb");

	// 启用MMU
	auto sctl = RegSCTLR_EL1::read();
	sctl.RES1_2=1;
	sctl.RES1_5=1;
	sctl.RES1_6=1;
	sctl.EE = 0;//little endian
	sctl.E0E = 0; // little endian EL0
	sctl.WXN = 0; // no effect,XNs normal
	sctl.I = 0; // no instruction fetch
	sctl.SA = 0; // no SPAlignment check
	sctl.SA0 = 0; // no SP Chekc EL0
	sctl.C = 0; // no cache
	sctl.A = 0;// no data alignment check
	sctl.M = 1;
	sctl.write();
	__asm__("isb");

	// 如果执行到这一句，就说明MMU启用成功了。
	kout << "enable MMU successfully.\n";

	const char * hello = "hello\n";
	const char * highPtr = reinterpret_cast<const char *>(
			reinterpret_cast<size_t>(hello) + 0xFFFFFF0000000000
			);
	kout << "high ptr = " << Hex(highPtr) << "\n";
	kout << highPtr;
}
