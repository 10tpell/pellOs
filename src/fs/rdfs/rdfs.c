#include <fs/rdfs/rdfs.h>
#include <fs/stat.h>
#include <lib/kmalloc.h>
#include <utils/memutils.h>
#include <utils/printf.h>
#include <fs/file.h>

static rdfs_partition_header_t* rdfs_start = (rdfs_partition_header_t *) 0;
static rdfs_file_dir_entry_t* first_file = (rdfs_file_dir_entry_t *) 0;
static uint64_t numFiles = 0;

sint8_t rdfs_init(void* initial_offset) {
    if (!initial_offset) return -1;

    rdfs_start = (rdfs_partition_header_t *) initial_offset;
    if (rdfs_start->magic_number != RDFS_MAGIC_NUMBER) return -1;

    first_file = (rdfs_file_dir_entry_t *) ((uintptr_t) rdfs_start + sizeof(rdfs_partition_header_t));
    numFiles = rdfs_start->file_directory_size / sizeof(rdfs_file_dir_entry_t);
    return 0;
}

#define DATA_LOCATION(f) ((void *) ((uintptr_t) rdfs_start + rdfs_start->file_directory_size + f->data_block_offset + sizeof(rdfs_partition_header_t)))

uint64_t* rdfs_get_file_list_from_dir_id(uint64_t curr_dir_id, uint64_t* list_size) {
    rdfs_file_dir_entry_t* file = &first_file[curr_dir_id];
    *list_size = file->data_entry_size / sizeof(uint64_t);
    
    uint64_t* file_list = (uint64_t *) kmalloc(*list_size * sizeof(uint64_t));
    uint64_t* file_ids = DATA_LOCATION(file); // Get location of first id in data block
    for (uint64_t i = 0; i < *list_size; i++) {
        file_list[i] = file_ids[i];
    }
    return file_list;
}

sint8_t rdfs_get_file_id_from_list_and_name(uint64_t* file_list, uint64_t size, const char* filename, uint64_t* file_id) {
    for (uint64_t i = 0; i < size; i++) {
        if (strcmp(first_file[file_list[i]].filename, filename) == 0) {
            *file_id = file_list[i];
            return 0;
        }
    }
    return -1;
}

/*
// path = "/.../.../.../filename" (filename could be a dir or file)
get_id_from_path(path)
parse_root()



*/
sint8_t rdfs_file_get_id_from_path(const char* path, uint64_t* id) {
    uint64_t current_dir_id = 0; // start with root node
    uint64_t* file_id_list = (uint64_t *) 0;
    uint64_t file_list_size = 0;

    const char* path_to_parse = &path[1]; // skip root
    while(path_to_parse[0] != 0) {
        /* get next file name */
        uint16_t i = 0;
        while (path_to_parse[i] != '/' && path_to_parse[i] != 0) {
            i++;
        }
        char* next_filename = (char *) kmalloc((i + 1) * sizeof(char));  // + 1 to include null terminator at end
        memcpy((uint8_t *) next_filename, (uint8_t *) path_to_parse, i);
        next_filename[i] = 0;

        if (path_to_parse[i] == '/') {
            path_to_parse = &path_to_parse[i + 1]; // Move path_to_parse to next filename
        } else {
            /* at end of path so set to 0 so that loop breaks at end */
            path_to_parse = &path_to_parse[i];
        }

        /* get list of ids in current directory */
        file_id_list = rdfs_get_file_list_from_dir_id(current_dir_id, &file_list_size);

        if (!file_id_list) return -1;

        /* get the id which corresponds to the next file name */
        if (rdfs_get_file_id_from_list_and_name(file_id_list, file_list_size, next_filename, &current_dir_id) < 0) return -1;

        kfree(file_id_list);
        kfree(next_filename);
    }

    *id = current_dir_id;
    return 0;
}

sint8_t rdfs_getattr(const char* path, stat_t *st) {
    uint64_t fileId = 0;
    if (rdfs_file_get_id_from_path(path, &fileId) < 0) return -1;

    rdfs_file_dir_entry_t* found_file = &first_file[fileId];

    if (found_file->file_type == RDFS_FILE_TYPE_DIR) {
        st->st_mode = S_IFDIR;
    } else if (found_file->file_type == RDFS_FILE_TYPE_FILE) {
        st->st_mode = S_IFREG;
    }
    st->st_size = found_file->data_entry_size;

    return 0;
}

uint8_t rdfs_id_isDir(uint64_t id) {
    rdfs_file_dir_entry_t* file = &first_file[id];
    return (file->file_type == RDFS_FILE_TYPE_DIR);
}

uint64_t* rdfs_readdir(const char* path, uint64_t* list_size) {
    uint64_t fileId = 0;
    if (rdfs_file_get_id_from_path(path, &fileId) < 0) return (uint64_t *) 0;
    if (!rdfs_id_isDir(fileId)) return (uint64_t *) 0;
    uint64_t* file_list = rdfs_get_file_list_from_dir_id(fileId, list_size);

    if (!file_list || !list_size) return (uint64_t *) 0;
    return file_list;
}

sint8_t rdfs_filename_from_id(uint64_t fileid, char* filepath) {
    if(first_file[fileid].file_id != fileid) return -1;
    
    strcpy(filepath, first_file[fileid].filename);
    filepath = first_file[fileid].filename;
    return 0;
}

sint8_t rdfs_open_file_id(uint64_t file_id, uint64_t flags, file_t* file) {
    if (!file) return -1;

    file->file_id = file_id;
    file->index = 0;
    file->flags = flags;
    return 0;
}

sint8_t rdfs_open_file(const char* path, uint64_t flags, file_t* file) {
    if (!file) return -1;

    uint64_t id = 0;
    
    if (rdfs_file_get_id_from_path(path, &id) < 0) return -1;
    return rdfs_open_file_id(id, flags, file);
}

uint64_t rdfs_read_file(file_t* file, uint8_t* buf, uint64_t size) {
    rdfs_file_dir_entry_t* file_dir_entry = &first_file[file->file_id];

    uint8_t* data_ptr = DATA_LOCATION(file_dir_entry);

    uint64_t total_remaining = file_dir_entry->data_entry_size - file->index; 

    if (total_remaining == 0) return 0;

    if (size > total_remaining) {
        size = total_remaining;
    }

    memcpy(buf, data_ptr + file->index, size);
    file->index += size;
    return size;
}