#ifndef __FS_H__
#define __FS_H__

#include <types.h>
#include <fs/fd.h>

sint64_t vfs_read(file_desc_t fd, char* buf, uint32_t size);
file_desc_t vfs_open(const char* path, uint64_t flags);
void vfs_close(file_desc_t fd);

#endif