
#include <memory/VirtualManager.h>
#include <arch/aarch64/system_common_registers.h>
#include <arch/aarch64/vmsa_descriptors.h>
#include <memory/MemoryManager.h>
#include <asm_instructions.h>
#include <new>
#include <generic/error.h>

int   VirtualManager::init()
{
	kout << INFO << "VirtualManager init\n";
	// 先判断是否支持4KB映射
	auto aa64 = RegID_AA64MMFR0_EL1::read();
	assert(aa64.TGran4 == 0b0000);

	// 初始化所有的内存属性
	RegMAIR_EL1 mair=RegMAIR_EL1::make(0);
	mair.Attr0 = 0xff; //Normal
	mair.Attr1 = 0x00;// nGnRnE
	mair.Attr2 = 0x44; //Normal,non-cacheable
	mair.write();

	// 后面设置虚拟地址的范围与物理地址范围相同, 并且只能是40,42,44,48这几个值,如果在这些值之外，就选择40
    size_t paBitsMap[]={32,  36,  40,  42,  44,  48,  52};
	size_t paIndex = aa64.PARange;
	kout << "PARangeIndex = " << aa64.PARange << "\n";
	kout << "PARange = " << paBitsMap[aa64.PARange] << "\n";
	if(paBitsMap[paIndex]<40 || paBitsMap[paIndex]>48)
		paIndex=2;

	_addressBits = paBitsMap[paIndex];


	// 设置TCR，TTBR0,TTBR1均使用4KB映射
	auto tcr = RegTCR_EL1::read();
	// disable address tag
	tcr.TBI0 = tcr.TBI1 = 0;
	// set output address size,same with PARange
	// IPS has 3bits, the extra bit is alwasy RES0
	tcr.IPS = paIndex; // 设置输出的虚拟地址的范围，包括0级页表。
	// set input address size, TxSZ is the most significant bits
	tcr.T0SZ = tcr.T1SZ = 64 - _addressBits;//设置输入的物理地址的范围
	tcr.A1 = 0; //ASID is in ttbr0
	tcr.TG0 = 0b00;//TTBR0 4KB
	tcr.TG1 = 0b10;//TTBR1 4KB
	tcr.SH0 = tcr.SH1 = 0b11;//inner shareable
	tcr.IRGN0 = tcr.IRGN1 = 0b01;//inner cacheable
	tcr.ORGN0 = tcr.ORGN1 =0b01;// outer cacheable
	tcr.EPD0 = tcr.EPD1 = 0; // not disable page walk on TLB miss
	tcr.write();
	asm_isb();

	return 0;
}
void VirtualManager::enableMMU(void* jmpFunc,void *newSp)
{
	kout << INFO << "VirtualManager enableMMU, jmpFunc = " << jmpFunc << ","
			     << "newSp = " << newSp
				 << "\n";
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
	asm_isb();

	kout << "successfully enabled MMU \n";
	kout << "jump to selected address/function\n";
	// 跳转到预定的函数处
	__asm__ __volatile__(
			"mov sp,%0 \n\t"
			"br  %1 \n\t"
			::"r"(newSp),
			  "r"(jmpFunc)
			  );
}

void VirtualManager::enableTTBR0(bool enable)
{
	if(!enable)
		asm_tlbi_aside1(RegTTBR0_EL1::read().ASID);
	auto tcr=RegTCR_EL1::read();
	tcr.EPD0 = (!enable); // 表示禁用，产生Translation Fault中断而不是进行转换表遍历
	tcr.write();
	asm_tlbi_vmallel1(); // FIXME 更细粒度的控制

}

void VirtualManager::updateTTBR0(const Descriptor4KBL0 *l0Table)
{
	RegTTBR0_EL1::make(reinterpret_cast<size_t>(l0Table)).write();
	asm_tlbi_vmallel1();//FIXME 更细粒度控制
}
void VirtualManager::updateTTBR1(const Descriptor4KBL0 *l0Table)
{

	RegTTBR1_EL1::make(reinterpret_cast<size_t>(l0Table)).write();
	asm_tlbi_vmallel1();//FIXME 更细粒度控制
}

void*  VirtualManager::translateVAToPA(const void * va)const
{
	if(va)
	{
		auto p = asm_at(reinterpret_cast<uint64_t>(va));
		if(!p.S0.F)// 使用static_cast表明至少有64位
			return reinterpret_cast<void*>((static_cast<uint64_t>(p.S0.PA51_48) << 48)|
					(static_cast<uint64_t>(p.S0.PA47_12) << 12)|getBits(reinterpret_cast<uint64_t>(va),0,11));
	}
	return nullptr;
}
uint64_t  VirtualManager::translateVAToPA(uint64_t va)const
{
	return reinterpret_cast<uint64_t>(translateVAToPA(reinterpret_cast<void*>(va)));
}
