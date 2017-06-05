#include "fs.h"

#include "../Mem/mem.h"

static struct File*  open(struct FileSystem* fs, struct UnicodeString* path, int flags)
{
	return NULL;
}	

bool	fs_initFileSystem(struct FileSystem* fs, struct BlockDevice* bdev)
{
	fs->bdev = bdev;
	
	fs->open = open;

	return true;
}

void	fs_destroyFileSystem(struct FileSystem** fs)
{
	if (! fs || ! *fs)
		return;

	kfree(*fs);

	*fs = NULL;
}


void	fs_destroyFile(struct File** file)
{
	if (! file || ! *file)
		return;

	kfree(*file);

	*file = NULL;
}
