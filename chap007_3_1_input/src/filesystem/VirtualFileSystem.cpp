/*
 * VirtualFileSystem.cpp
 *
 *  Created on: Apr 8, 2018
 *      Author: 13774
 */
#include <filesystem/VirutalFileSystem.h>
#include <memory/MemoryManager.h>
#include <io/Output.h>
#include <asm_instructions.h>
#include <cassert>

VirtualFileSystem::VirtualFileSystem()
	:_root( new VirtualFile(""))
{
	assert(_root);
}

VirtualFileSystem::~VirtualFileSystem()
{
	if(_root)
	{
		delete _root;
	}
}

