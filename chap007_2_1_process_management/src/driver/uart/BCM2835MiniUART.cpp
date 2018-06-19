/*
 * BCM2835MiniUART.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */

#include <driver/uart/BCM2835MiniUART.h>

int BCM2835MiniUART::init()
{
	enable(true);
	dataSize(8);

	baudrate(270);    // 115200 baud
	enable(true);

	enableReceive(true);//reset to true
	enableSend(true); // reset to true
	return 0;
}


