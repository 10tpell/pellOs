#ifndef __FILE_H__
#define __FILE_H__

#include <types.h>

typedef struct file_struct {
    uint64_t file_id;
    uint64_t index;
    uint64_t flags;
} file_t;



#endif