#include <fs/vfs.h>
#include <fs/rdfs/rdfs.h>
#include <peripherals/system_timer.h>
#include <utils/memutils.h>

sint64_t vfs_read(file_desc_t fd, char* buf, uint32_t size) {
    file_t file;
    if(get_file_from_fd(fd, &file) < 0) return -1;

    if (file.file_id == 255) {
        uint32_t timer_val = get_timer_val();
        memcpy((uint8_t *) buf, (uint8_t *) &timer_val, sizeof(timer_val));
        return sizeof(timer_val);
    }

    return rdfs_read_file(&file, (uint8_t *) buf, size);
}
