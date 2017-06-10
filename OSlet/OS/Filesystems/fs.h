#pragma once

#include "../Devices/Block/block.h"
#include "../Lib/klib.h"

enum
{
	SEEK_SET = 0,
	SEEK_CUR,
	SEEK_END
};	

enum 
{
	FILEMODE_READABLE	= 1,
	FILEMODE_WRITEABLE	= 2
};

enum
{
	// TODO: more POSIX flags

	O_RDONLY	=	0x0000,
	O_WRONLY	=	0x0001,
	O_RDWR		=	0x0002,
	O_ACCMODE	=	0x0003,

	O_CREAT		=	0x0100,
	O_EXCL		=	0x0200,
	O_NOCTTY	=	0x0400,
	O_TRUNC		=	0x0800,
	O_APPEND	=	0x1000,
	O_NONBLOCK	=	0x2000,

};

struct FileSystem;

struct File
{

	unsigned int mode;
	unsigned long long int size;

	off_t	offset;	

	struct FileSystem* fs;

	size_t (*read)(struct File* file, void* buf, size_t count);
	off_t  (*lseek)(struct File* file, off_t offset, int whence);

	void	(*close)(struct File* file);

};

struct FileSystem
{

	struct BlockDevice* bdev;

	struct File* (*open)(struct FileSystem* fs, struct UnicodeString* path, int flags);	

};

bool	fs_initFileSystem(struct FileSystem* fs, struct BlockDevice* bdev);
void	fs_destroyFileSystem(struct FileSystem** fs);

void	fs_destroyFile(struct File** file);

#include "fat.h"
