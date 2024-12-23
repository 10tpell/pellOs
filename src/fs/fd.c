#include <fs/file.h>
#include <fs/fd.h>
#include <lib/kmalloc.h>
#include <scheduler/scheduler.h>
#include <utils/memutils.h>

uint8_t is_valid_fd(file_desc_t fd) {
    if (fd < 0) return 0;
    
    task_struct* task = get_current_task();
    return (task->fd_table.file[fd].flags > 0);
} 

sint8_t get_file_from_fd(file_desc_t fd, file_t* file) {
    task_struct* task = get_current_task();
    if (!is_valid_fd(fd)) return -1;

    *file = task->fd_table.file[fd];
    return 0;
}

void init_fd_table() {
    task_struct* task = get_current_task();

    file_desc_table_t* fd_table = &task->fd_table;

    // check that the fd_table does not already exist! (this function is called during context switch so will be called more than one)
    // TODO: find a better time to call this function
    if (!fd_table->file) {
        fd_table->bitmap = (uint8_t *) kmalloc(sizeof(uint8_t) * FILE_DESC_TABLE_INIT_SIZE/8);
        memzero_u8(fd_table->bitmap, sizeof(uint8_t) * FILE_DESC_TABLE_INIT_SIZE/8);

        fd_table->file = (file_t *) kmalloc(sizeof(file_t) * FILE_DESC_TABLE_INIT_SIZE);
        memzero_u8((uint8_t *) fd_table->file, sizeof(fd_table->file) * FILE_DESC_TABLE_INIT_SIZE);
    }
} 

void destroy_fd_table() {
    task_struct* task = get_current_task();
    file_desc_table_t* fd_table = &task->fd_table;

    kfree(fd_table->bitmap);
    kfree(fd_table->file);
}

file_desc_t create_fd(file_t file) {
    task_struct* task = get_current_task();

    file_desc_table_t* fd_table = &task->fd_table;
    file_desc_t fd = -1;

    for (uint8_t i = 0; i < FILE_DESC_TABLE_INIT_SIZE/8; i++) {
        if (fd_table->bitmap[i] == (uint8_t) -1) continue;

        uint8_t byte = fd_table->bitmap[i];
        for (uint8_t j = 0; j < 8; j++) {
            if ((byte & 1) == 0) {
                fd_table->bitmap[i] |= 1 << j;
                fd = j + (i * 8);
                fd_table->file[fd] = file;
                fd_table->file[fd].flags = 1;
                return fd;
            }
            byte = byte >> 1;
        }
    }

    return fd; 
}

void delete_fd(file_desc_t fd) {
    task_struct* task = get_current_task();
    file_desc_table_t* fd_table = &task->fd_table;

    fd_table->bitmap[(fd/8)] &= ~(1 << (fd % 8));
    fd_table->file[fd].file_id = 0;
    fd_table->file[fd].index = 0;
    fd_table->file[fd].flags = 0;
}
