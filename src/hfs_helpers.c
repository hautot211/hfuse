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
    printf("ENTERING to_mac_path\n");
    printf("path : %s (%d)\n", path, strlen(path));
    size_t to_allocate = (strlen(path) + 16);
    printf("to_allocate = %d\n", (to_allocate));
    char* const mac_path = calloc(sizeof(char), to_allocate);
    printf("allocation OK\n");
    strncpy(mac_path, path, strlen(path) + 1);
    printf("mac_path: %s (%d)\n", mac_path, strlen(mac_path));

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

/*
char* const trim_virtual_dir(const char* const path, const char* const vdir) {
    size_t vdir_len = strlen(vdir);
    size_t path_len = strlen(path);


    // .rsrc => /.rsrc
    size_t actual_vdir_len = (vdir_len + 1);
    char* actual_vdir = calloc(1, sizeof(char) * (actual_vdir_len + 1)); // freed
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
    free(actual_vdir);
    return newpath;
}*/

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
    if(get_dir_type(current_entity + 1) != fkData) {
        strcpy(vdir, current_entity + 1);
        char* const concrete_path = get_parent_directory(path, NULL); // returned
        return concrete_path;
    }

    /* Case path is a virtual file */
    const char* const parent_path = get_parent_directory(path, NULL); // freed
    const char* const parent_entity = last_occurence(parent_path, "/");
    size_t path_len = strlen(path);
    if(get_dir_type(parent_entity + 1) != fkData) {
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

    /* Case path is a concrete file or directory*/
    size_t concrete_path_len = path_len;
    char* const concrete_path = malloc((concrete_path_len + 1) * sizeof(char)); // returned
    strncpy(concrete_path, path, path_len);
    strncpy(vdir, "", 1);

    return concrete_path;
}

dir_type_t get_dir_type(const char* const path) {
    if(strcmp(path, ".rsrc") == 0) return fkRsrc;
    if(strcmp(path, ".fdat") == 0) return 0xf0;
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