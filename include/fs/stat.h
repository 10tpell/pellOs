#ifndef __STAT_H__
#define __STAT_H__

#include <types.h>

#define S_IFDIR ((uint64_t) 1 << 32);
#define S_IFREG ((uint64_t) 2 << 32);

typedef struct stat_struct {
    uint64_t st_mode;
    uint64_t st_size;
} stat_t;

#endif