#include <fs/vfs.h>
#include <fs/rdfs/rdfs.h>
#include <utils/memutils.h>

#include <scheduler/scheduler.h>

file_desc_t vfs_open(const char* path, uint64_t flags) {
    file_t file;

    if (!get_current_task()->fd_table.file) init_fd_table();

    if(strcmp(path, "/dev/timer") == 0) {
        file.file_id = 255;
        file.flags = 0;
        file.index = 0; 
        return create_fd(file);   
    }

    if (rdfs_open_file(path, flags, &file) < 0) return 0;

    return create_fd(file);
}

void vfs_close(file_desc_t fd) {
    delete_fd(fd);
}
