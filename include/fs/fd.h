#ifndef __FD_H__
#define __FD_H__

#include <fs/file.h>

#define FILE_DESC_TABLE_INIT_SIZE 127

typedef sint16_t file_desc_t;

typedef struct file_desc_table_struct {
    uint8_t* bitmap;
    file_t* file;
} file_desc_table_t;

void destroy_fd_table();
sint8_t get_file_from_fd(file_desc_t fd, file_t* file);
void init_fd_table();
file_desc_t create_fd(file_t file);
uint8_t is_valid_fd(file_desc_t fd);
void delete_fd(file_desc_t fd);

#endif