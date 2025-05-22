#include "hfs_helpers.h"

#include "hfuse_context.h"
#include <stdlib.h>
#include <string.h>
#include <hfs/apple.h>
#include <hfs/hfs.h>
#include <hfs/libhfs.h>

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
    char* const mac_path = malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(mac_path, path);

    for(size_t i = 0; i < strlen(mac_path); i++) {
        if (mac_path[i] == '/')
            mac_path[i] = ':';
    }
    
    return mac_path;
}


bool is_directory(const hfsdirent* const directory_entity) {
    return directory_entity->flags & HFS_ISDIR;
}

bool is_symlink(const hfsdirent* const directory_entity) {
    return false;
}


unsigned long fork_size(hfsfile* const file) {
    unsigned long current = file->pos;
    hfs_seek(file, 0, HFS_SEEK_SET);
    unsigned long begin = file->pos;
    hfs_seek(file, 0, HFS_SEEK_END);
    unsigned long end = file->pos;

    hfs_seek(file, current, HFS_SEEK_SET);
    return end - begin;
}