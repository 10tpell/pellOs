#ifndef __RDFS_H__
#define __RDFS_H__

#include <types.h>
#include <fs/stat.h>

/*
========================================== RamDiskFileSystem ================================================

Read only filesystem designed for ramdisks on startup 

**** Features ****
* Incredibly simple
* Read Only
* Easy to find inode from id (indexed by id)

**** Partition layout ****
 --------
| Header |
| files  |
| data   |
 --------

**** Files ****
Files are either directories or actual files, they contain identifying information and point
to the data blocks and specify the size of the data block 

--- Directories ---
Directories point to a datablock which contains a list of files (files/directories). The first entry in the 
file directory is always the root directory (/) of the partition.


============================================================================================================
*/


#define RDFS_MAGIC_NUMBER 0x7DF5

#define RDFS_FILE_TYPE_DIR 1
#define RDFS_FILE_TYPE_FILE 2

#define RDFS_DIR_MAX_ENTRIES 256

/* Header for rdfs partition (will be located at block 0)*/
typedef struct rdfs_partition_header {
    uint16_t magic_number;
    uint64_t partition_size;
    char partition_name[64];
    uint64_t file_directory_size;
    uint64_t data_directory_size;
} rdfs_partition_header_t;

typedef struct rdfs_file_dir_entry {
    uint64_t file_id;
    uint8_t file_type;
    char filename[71];
    uint64_t data_block_offset;
    uint64_t data_entry_size;
} rdfs_file_dir_entry_t;

sint8_t rdfs_init(void* initial_offset);

/*
path: null-terminated string containing filepath
st: pre-allocated stat_t to be populated by function
*/
sint8_t rdfs_getattr(const char* path, stat_t *st);

/*
path: null-terminated string containing filepath
file_list: empty pointer to file_list (will be allocated by function)
list_list: pointer to a uint64_t, will be modified to contain size of list
*/
uint64_t* rdfs_readdir(const char* path, uint64_t* list_size);

extern void* __ramdisk_start;
#endif