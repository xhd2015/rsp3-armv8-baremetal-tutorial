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

int main(int argc,char **argv)
{
	// FIXME 为了使输出一致，系统应当在使用out之前刷新输出
	//        如果这种责任转移到用户态，在应当在每次使用系统输出前，
	//        手动刷新输出。（容易出错）
	kout << "argv[0] = " << argv[0] << ".\n";
	kout << "ls:\n";
	kout.flush();
	svc_call<SvcFunc::ls>();

	kout << "pwd:\n";kout.flush();
	svc_call<SvcFunc::pwd>();

	kout << "cat:\n";kout.flush();
	Vector<String> path;
	path.emplaceBack("kern.log");
	VectorRef<String> pathRef(path);
	svc_call<SvcFunc::cat>(reinterpret_cast<size_t>(&pathRef));

	return 0;
}



