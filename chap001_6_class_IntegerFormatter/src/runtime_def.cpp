/*
 * runtime_def.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: 13774
 */
#include <runtime_def.h>
#include <driver/uart/BCM2835MiniUART.h>
#include <driver/gpio/GPIO.h>
#include <generic/IntegerFormatter_forward.h> //formatBuf

GPIO gpio(0);

BCM2835MiniUART uart1(0);

char   formatBuf[FORMAT_BUF_SIZE];



