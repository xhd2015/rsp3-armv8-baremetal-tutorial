#include <cstdio>
#include <generic/util.h>
#include <generic/IntegerFormatter.h>
#include <new> //placement-new
#include <cassert>
#include <arch/aarch64/system_common_registers.h>
#include <arch/aarch64/vmsa_descriptors.h>
#include <arch/aarch64/init.h>
#include <memory/VirtualMemoryDefinition.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <io/char/BCM2835MiniUARTWriter.h>
#include <driver/uart/PL011.h>
#include <driver/gpio/GPIO.h>
#include <io/char/PL011Writer.h>
#include <memory/MemoryManager.h>
#include <memory/VirtualManager.h>
#include <exception/ExceptionHandler.h>
#include <driver/timer/BCM2835SystemTimer.h>
#include <asm_instructions.h>
#include <runtime_def.h>
#include <arch/aarch64/cxx_macros.h>
#include <exception/BCM2835InterruptManager.h>
#include <arch/aarch64/timer_registers.h>

extern uint64_t __free_ram_start[];
extern uint64_t __stack_top[];
extern void* __part2_start[];
extern void* __part2_stack_top[];

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

		// 在EL1/0允许访问计时器寄存器
		auto cnthctl=RegCNTHCTL_EL2::read();
		cnthctl.EL1PCEN=1;
		cnthctl.EL1PCTEN=1;//no traps
		cnthctl.write();
	}
	if(el>=1)
	{
//		kout << "cntkctl enable\n";
		auto cntk = RegCNTKCTL_EL1::read();
		cntk.EL0PTEN=1;
		cntk.EL10PCTEN=1;//freqency
		cntk.EL0VCTEN=1;
		cntk.EL0VTEN=1;
		cntk.write();
//		kout << "cntkctl enabled.\n";
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
	new (&virtman) VirtualManager;

	auto currentel = RegCurrentEL::read();
	kout << "currentEL = " << currentel.EL << "\n";

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

	virtman.init();
	virtman.enableMMU(
			__part2_start[0],
			__part2_stack_top[0],
			virtman.addressBits(),
			mman.base()
	);
}


