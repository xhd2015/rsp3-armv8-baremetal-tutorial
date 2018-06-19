/*
 * FileType.h
 *
 *  Created on: Jun 13, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_FILESYSTEM_FILETYPE_H_
#define INCLUDE_FILESYSTEM_FILETYPE_H_
#include <cstdint>
#include <cstddef>

enum FileType:uint16_t {
	F_NON_EXISTS,
	F_FILE,
	F_DIRECTORY,
	F_DEVICE,
	F_SUB_FILE_SYSTEM
};
enum SubFileSystemType : uint16_t {
	FS_NONE,FS_RAMFS, FS_ROOTFS,FS_FAT12,FS_FAT16,FS_FAT32,
};


#endif /* INCLUDE_FILESYSTEM_FILETYPE_H_ */
