#include "hfs_helpers.h"

#include "hfuse_context.h"
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
    char* const mac_path = malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(mac_path, path);

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

char* const trim_virtual_dir(const char* const path, const char* const vdir) {
    size_t vdir_len = strlen(vdir);
    size_t path_len = strlen(path);


    // .rsrc => .rsrc/
    size_t actual_vdir_len = (vdir_len + 1);
    char* actual_vdir = calloc(1, sizeof(char) * (actual_vdir_len + 1)); // free
    actual_vdir[0] = '/';
    strcpy(actual_vdir + 1, vdir);

    // /path/to/.rsrc/test => /path/to/test
    // /path/to/.rsrc => /path/to
    // /path/to/.rsrc/ => /path/to/
    const char* vdir_occ = last_occurence(path, actual_vdir);
    if (vdir_occ == NULL) {
        free(actual_vdir);
        char* const newpath = calloc(1, path_len + 1); // returned
        strcpy(newpath, path);
        return newpath;
    }
    size_t vdir_occ_len = strlen(vdir_occ);
    
    const char* const vfile_occ = vdir_occ + actual_vdir_len;
    const size_t vfile_occ_len = strlen(vfile_occ);
    
    size_t newpath_len = MAX(vfile_occ_len, 1) + (vdir_occ - path + 1);
    char* const newpath = calloc(1, newpath_len + 1); // returned
    memcpy(newpath, path, path_len - vdir_occ_len);
    strcat(newpath, vfile_occ);
    newpath[0] = '/';
    return newpath;
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

dir_type_t get_dir_type(const char* const path) {
    if(last_occurence(path, "/.rsrc") != NULL) return fkRsrc;
    if(last_occurence(path, "/.fdat") != NULL) return 0xf0;
    return fkData;
}


bool is_directory(const hfsdirent* const directory_entity) {
    return directory_entity->flags & HFS_ISDIR;
}

bool is_symlink(const hfsdirent* const directory_entity) {
    return false;
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