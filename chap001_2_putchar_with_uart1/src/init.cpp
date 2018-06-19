#include <cstdio>
extern "C" void init();
__asm__(
	".section .text.boot \n\t"
	"mrs     x0, mpidr_el1 \n\t"
	"and     x0, x0, #0b11 \n\t"
	"cmp     x0,#0 \n\t"
	"b.eq      2f \n\t"
	"1:  		\n\t"
	"b       1b	\n\t"
	"2: \n\t"
	"ldr    x0, =__stack_top\n\t"
	"mov    sp,x0 \n\t"
	"b      init \n\t"
);
__attribute__((section(".text.boot")))
void init()
{
	while(true)
		putchar('A');
}

