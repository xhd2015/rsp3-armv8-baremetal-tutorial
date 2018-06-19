/*
 * main.cpp
 *
 *  Created on: Jun 16, 2018
 *      Author: 13774
 */

#include <runtime_def.h>
#include <io/Output.h>
#include <exception/svc_call.h>
#include <io/Input.h>

int main()
{
	kout << "please input:";
	kout.flush();

	auto s = kin.readline();
	kout << s;
	kout.flush();

	svc_call<SvcFunc::ps>();
	return 0;
}



