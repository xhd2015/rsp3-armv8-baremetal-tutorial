/*
 * PL011.cpp
 *
 *  Created on: Mar 20, 2018
 *      Author: 13774
 */

#include <driver/uart/PL011.h>

/**
 * The UART_CR Register is the control register.
NOTE:
To enable transmission, the TXE bit and UARTEN bit must be set to 1.
Similarly, to enable reception, the RXE bit and UARTEN bit, must be set to 1.
NOTE:
Program the control registers as follows:
1. Disable the UART.
2. Wait for the end of transmission or reception of the current character.
3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control
Register, UART_LCRH.
4. Reprogram the Control Register, UART_CR.
5. Enable the UART.

NOTE: The UART_LCRH, UART_IBRD, and UART_FBRD registers must not
be changed:
when the UART is enabled
when completing a transmission or a reception when it has been programmed
to become disabled

波特率计算：
BAUDDIV = (FUARTCLK/(16 * Baudrate))
BAUDDIV由两个寄存器构成，是一个带小数点的数。

所以，根据波特率和FUARTCLK即可计算出BAUDDIV的数。在4000000Hz,115200的情况下，BAUDDIV为2.125
取整为 0x2.0xB

 */
int PL011::init()
{
	enable(false);
	clearIntFlags();
	reg32(UARTIBRD) = 0x2;//配置波特率
	reg32(UARTFBRD) = 0xB;// 115200 baud
	reg32(UARTCR) = bitOnes<8,9>();// 启用8,9位 enable Send/Receive
	// 默认情况下屏蔽所有的中断
	reg32(UARTIMSC) = 0;//lowerMaskBits(11); // 高5位res0
	reg32(UARTLCR_H) = bitOnes<5,6>();// bit5,6必须设置， 此处设置数据的长度为8位。  bit4 选择传输模式：1：FIFO mode 2：1-byte mode
//	enableFIFO(false);//禁用FIFO模式
//	readInterruptLevel(L_1of8); // 配置触发模式，仅当FIFO启用有效
	enable(true);

	return 0;
}
void     PL011::waitInput()const
{
	while(!readReady());
	rawRead();
}


