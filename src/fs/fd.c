#include <fs/file.h>
#include <fs/fd.h>
#include <lib/kmalloc.h>
#include <scheduler/scheduler.h>

uint8_t is_valid_fd(file_desc_t fd) {
    if (fd < 0) return 0;
    
    task_struct* task = get_current_task();
    return (fd <= (sint16_t) task->fd_table.count);
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
        fd_table->count = 0;
        fd_table->file = (file_t *) kmalloc(sizeof(file_t) * FILE_DESC_TABLE_INIT_SIZE);
    }
} 

file_desc_t create_fd(file_t file) {
    task_struct* task = get_current_task();

    file_desc_table_t* fd_table = &task->fd_table;
    
    file_desc_t fd = fd_table->count;
    fd_table->file[fd] = file;

    fd_table->count++;
    return fd; 
}
