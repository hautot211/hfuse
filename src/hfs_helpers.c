#include "hfs_helpers.h"

#include "hfuse_context.h"
#include "hfuse.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <hfs/apple.h>
#include <math.h>
#include <hfs/hfs.h>
#include <hfs/libhfs.h>

#define MAX(x, y) (x > y ? x : y)

struct fuse_file_info* const file_info_init(void* handler) {
    struct fuse_file_info* const fi = calloc(1, sizeof(struct fuse_file_info));
    fi->fh = (uint64_t) handler;
    return fi;
}

hfsvol* const hfuse_get_context_volume() {
    hfuse_context_t* const context = hfuse_get_context();
    return hfuse_get_volume(context);
}

hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi) {
    return (hfsdir*) fi->fh;
}
void hfuse_set_file_info_dir(struct fuse_file_info* const fi, const hfsdir* const directory) {
    fi->fh = (uint64_t) directory;
}
hfsfile* const hfuse_get_file_info_file(const struct fuse_file_info* const fi) {
    return (hfsfile*) fi->fh;
}
void hfuse_set_file_info_file(struct fuse_file_info* const fi, const hfsfile* const file) {
    fi->fh = (uint64_t) file;
}

char* const to_mac_path(const char* const path) {
    size_t to_allocate = (strlen(path) + 1);
    char* const mac_path = calloc(sizeof(char), to_allocate);
    strncpy(mac_path, path, strlen(path) + 1);

    for(size_t i = 0; i < strlen(mac_path); i++) {
        if (mac_path[i] == '/')
            mac_path[i] = ':';
    }
    
    return mac_path;
}

bool endswith(const char* const str, const char* const suf) {
    size_t str_len = strlen(str);
    size_t suf_len = strlen(suf);

    if(suf_len > str_len) return false;
    for(size_t i = 0; i < suf_len; i++) {
        if(str[str_len - i - 1] != suf[suf_len - i - 1]) return false;
    }
    return true;
}

const char* const last_occurence(const char* const str, const char* const inf) {
    const char* last = NULL;
    const char* curr = str;
    
    while (true) {
        curr = strstr(curr, inf);
        if(curr == NULL) {
            return last;
        }
        last = curr;
        curr++;
    }
}

char* const get_parent_directory(const char* const path, char* buffer) {
    if(strcmp(path, "/") == 0) return NULL;
    size_t path_len = strlen(path);

    const char* const filename = last_occurence(path, "/");
    size_t filename_len = strlen(filename);
    
    size_t buffer_len = MAX(path_len - filename_len, 1);
    if(buffer == NULL) buffer = calloc(sizeof(char), buffer_len + 1);
    memcpy(buffer, path, buffer_len);
    buffer[0] = '/';

    return buffer;
}

char* const trim_virtual_dir(const char* const path, char* vdir) {
    /* Case path is root */
    if(strcmp(path, "/") == 0) {
        strcpy(vdir, "");
        char* const concrete_path = malloc(2 * sizeof(char)); // returned
        strcpy(concrete_path, "/");
        return concrete_path;
    }

    /* Case path is a virtual directory */
    const char* const current_entity = last_occurence(path, "/");
    if(get_dirtype(current_entity + 1) != DIRTYPE_DATA) {
        strcpy(vdir, current_entity + 1);
        char* const concrete_path = get_parent_directory(path, NULL); // returned
        return concrete_path;
    }

    /* Case path is a virtual file */
    const char* const parent_path = get_parent_directory(path, NULL); // freed
    const char* const parent_entity = last_occurence(parent_path, "/");
    size_t path_len = strlen(path);
    if(get_dirtype(parent_entity + 1) != DIRTYPE_DATA) {
        strcpy(vdir, parent_entity + 1);
        size_t parent_entity_len = strlen(parent_entity);
        size_t concrete_path_len = path_len - parent_entity_len;
        char* concrete_path = calloc(concrete_path_len + 1, sizeof(char)); // returned
        concrete_path = get_parent_directory(parent_path, concrete_path);
        if(strcmp(concrete_path, "/") == 0) strcpy(concrete_path, "");
        concrete_path = strcat(concrete_path, current_entity);
        free((void*) parent_path);
        return concrete_path;
    }
    free((void*) parent_path);

    /* Case path is a concrete file or directory*/
    size_t concrete_path_len = path_len;
    char* const concrete_path = malloc((concrete_path_len + 1) * sizeof(char)); // returned
    strncpy(concrete_path, path, concrete_path_len + 1);
    strncpy(vdir, "", 1);

    return concrete_path;
}

dirtype_t get_dirtype(const char* const path) {
    if(strcmp(path, VDIR_RSRC) == 0) return DIRTYPE_RSRC;
    if(strcmp(path, VDIR_FINF) == 0) return DIRTYPE_FINF;
    return DIRTYPE_DATA;
}


bool is_directory(const hfsdirent* const directory_entity) {
    return directory_entity->flags & HFS_ISDIR;
}

bool is_symlink(const hfsdirent* const directory_entity) {
    /* Read finder info and strcmp with 'hlnk' or something */
    return strcmp(directory_entity->u.file.creator, "hbwr") == 0;
}


unsigned long current_fork_size(hfsfile* const file) {
    unsigned long current = file->pos;
    hfs_seek(file, 0, HFS_SEEK_SET);
    unsigned long begin = file->pos;
    hfs_seek(file, 0, HFS_SEEK_END);
    unsigned long end = file->pos;

    hfs_seek(file, current, HFS_SEEK_SET);
    return end - begin;
}