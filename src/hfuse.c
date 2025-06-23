/*
File        : hfuse.c
              ( C source file )

Description : GREYC's C project to mount standard HFS disk images on Linux virtual filesystem

Author   : Guillaume HAUTOT

License     : MIT

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "hfuse.h"
#include "hfs_helpers.h"
#include "hfuse_context.h"
#include "file_handler.h"
#include "debug_funcs.h"

#include <hfs/libhfs.h>
#include <hfs/hfs.h>
#include <hfs/file.h>

#include <alloca.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

const struct fuse_operations hfuse_operations = {
        .init           = hfuse_init,
        .destroy        = hfuse_destroy,
        
        .getattr        = hfuse_getattr,
        
        .opendir        = hfuse_opendir,
        .releasedir     = hfuse_releasedir,
        .readdir        = hfuse_readdir,
        
        .open           = hfuse_open,
        .release        = hfuse_release,
        .read           = hfuse_read,

        .readlink       = hfuse_readlink,
};


void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    printf("CALL hfuse_init\n");
    cfg->kernel_cache = 0;
    hfuse_context_t* const context = hfuse_get_context();
    hfuse_fill_context(context);
    // printf("RETURN hfuse_init\n");
    return (void*) context;
}


void hfuse_destroy(void* private_data) {
    // printf("CALL hfuse_destroy\n");
    const hfuse_context_t const* context = (const hfuse_context_t const*) private_data;
    hfuse_destroy_context(context);
    // printf("RETURN hfuse_destroy\n");
}


int hfuse_getattr(const char *path, struct stat * stbuf, struct fuse_file_info *fi) {
    printf("CALL hfuse_getattr(%s)\n", path);
    memset(stbuf, 0, sizeof(struct stat));
    
    /* 5 cases
    - hfs file
    - hfs folder
    - hfs symlink
    - virtual folder (.rsrc, .fdat, ...)
    - virtual files (.rsrc/foo, .fdat/bar)
    */
    
    hfsvol* const volume = hfuse_get_context_volume();
    hfuse_handler_t* const handler = hfuse_handler_init(path, ENTTYPE_DIRECTORY);    
    
    dirtype_t dirtype = hfuse_handler_get_dirtype(handler);
    const char* const mac_path = hfuse_handler_get_macpath(handler);

    hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));
    printf("after malloc : ERRNO : %d\n", errno);
    int hfs_stat_res = hfs_stat(volume, mac_path, directory_entity);
    printf("after hfs_stat : ERRNO : %d\n", errno);
    printf("volume : %p, mac_path : %p, directory_entity : %p\n", volume, mac_path, directory_entity);

    dbg();
    
    if(hfs_stat_res == -1) {
        printf("Freeing directory_entity : %p\n", directory_entity);
        printf("ERRNO : %d\n", errno);
        free((void*) directory_entity);
        return -ENOENT;
    }
    dbg();
    
    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_mode = 0;
    
    if(is_directory(directory_entity)) {
        stbuf->st_mode |= S_IFDIR;
        stbuf->st_mode |= 0750;
        stbuf->st_nlink = 2;
        stbuf->st_size = 4096;
         printf("\t%s is a directory\n", path);
    } else if(is_symlink(directory_entity) && dirtype == DIRTYPE_DATA){
        stbuf->st_mode |= S_IFLNK;
        stbuf->st_mode |= 0440;
        printf("\t%s is a symlink\n", path);
    } else {
        printf("\t%s is a regular file\n", path);
        stbuf->st_mode |= S_IFREG;
        stbuf->st_mode |= 0440;
        stbuf->st_nlink = 1;

        hfsfile* const file = hfs_open(volume, mac_path);
        hfs_setfork(file, (dirtype == DIRTYPE_DATA) ? fkData : fkRsrc);
        printf("\t\tFORK : %s\n", (dirtype == DIRTYPE_DATA) ? "DATA" : "RESOURCE");
        stbuf->st_size = current_fork_size(file);
    }

    dbg();
    printf("ERRNO : %d\n\n", errno);
    // TODO complete stat struct

    free((void*) directory_entity);
    dbg();
    printf("ERRNO : %d\n\n", errno);
    return 0;
}

int hfuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    // printf("CALL hfuse_readdir(path = %s, offset = %ld)\n", path, offset);

    hfuse_handler_t* const handler = (hfuse_handler_t* const) fi->fh;
    dirtype_t dirtype = hfuse_handler_get_dirtype(handler);
    off_t next_offset = offset + 1;
    
    switch(offset) {
        case 0:
            filler(buf, ".", NULL, next_offset, 0);
            return 0;
        case 1:
            filler(buf, "..", NULL, next_offset, 0);
            return 0;
        case 2:
            if(dirtype == DIRTYPE_RSRC) break;
            filler(buf, VDIR_RSRC, NULL, next_offset, 0);
            return 0;
    }
    
    hfsdir* const directory =  hfuse_handler_get_directory(handler);
    hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));
    int hfs_readdir_result = 0;
    switch(dirtype) {
        case DIRTYPE_DATA:
            hfs_readdir_result = hfs_readdir(directory, directory_entity);
            if (hfs_readdir_result == -1) {
                if (errno == ENOENT) {
                    free((void*) directory_entity);
                    printf("NO MORE\n");
                    errno = 0;
                    return 0;
                }
                else {
                    free((void*) directory_entity);
                    printf("FAILURE\n");
                    return -1;
                }
            }
            break;
            
        case DIRTYPE_RSRC:
            bool ignore = true;
            do {
                hfs_readdir_result = hfs_readdir(directory, directory_entity);
                ignore = is_directory(directory_entity);
                if (hfs_readdir_result == -1) {
                    free(directory_entity);
                    if (errno == ENOENT) {
                        errno = 0;
                        return 0;
                    }
                    else {
                        return -1;
                    }
                }
            } while(ignore);
            break;
        }
    const char* const subentity = escape_chars(directory_entity->name);
    
    filler(buf, subentity, NULL, next_offset, 0);
    free((void*) subentity);
    free((void*) directory_entity);
    //printf("RETURN hfuse_readdir\n");
    return 0;
}

int hfuse_opendir(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_opendir(%s)\n", path);

    hfuse_handler_t* const handler = hfuse_handler_init(path, ENTTYPE_DIRECTORY);
    fi->fh = (uint64_t) handler;
    hfuse_handler_open(handler);

    return 0;
}

int hfuse_releasedir(const char *path, struct fuse_file_info* fi) {
    // printf("CALL hfuse_releasedir(%s)\n", path);

    hfuse_handler_t* const handler = (hfuse_handler_t* const) fi->fh;
    hfuse_handler_destroy(handler);
    fi->fh = (uint64_t) NULL;

    // printf("RETURN hfuse_releasedir\n");
    return 0;
}

int hfuse_open(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_open(%s)\n", path);

    hfuse_handler_t* const handler = hfuse_handler_init(path, ENTTYPE_FILE);
    hfuse_handler_open(handler);
    hfsfile* const file = hfuse_handler_get_file(handler);
    dirtype_t dirtype = hfuse_handler_get_dirtype(handler);
    fi->fh = (uint64_t) handler;
    
    switch(dirtype) {
        case DIRTYPE_DATA:
        hfs_setfork(file, fkData);
        break;
        case DIRTYPE_RSRC:
        hfs_setfork(file, fkRsrc);
        break;
    }

    return 0;
}

int hfuse_release(const char* path, struct fuse_file_info* fi) {
    // printf("CALL hfuse_release(%s)\n", path);

    hfuse_handler_t* const handler = (hfuse_handler_t* const) fi->fh;
    hfuse_handler_destroy(handler);
    fi->fh = (uint64_t) NULL;

    // printf("RETURN hfuse_release\n");
    return 0;
}

int hfuse_read(const char *path, char *buffer, size_t length, off_t offset, struct fuse_file_info* fi) {
    // printf("CALL hfuse_read('%s', %p, %ld, %ld, %p)\n", path, buffer, length, offset, fi);
    hfuse_handler_t* const handler = (hfuse_handler_t* const) fi->fh;
    
    hfsfile* const file = hfuse_handler_get_file(handler);
    hfs_seek(file, offset, HFS_SEEK_SET);
    long total_read = hfs_read(file, buffer, length);

    return total_read;
}

int hfuse_readlink(const char* path, char* buffer, size_t length) {
    printf("CALL hfuse_readlink(%s, %p, %ld)\n", path, buffer, length);
    char* vdir = malloc(HFS_MAX_FLEN + 1);
    char* concrete_path = trim_virtual_dir(path, vdir);
    char* mac_path = strtr(NULL, concrete_path, '/', ':');

    hfsvol* const volume = hfuse_get_context_volume();
    hfsdir* const directory = hfs_opendir(volume, mac_path);
    hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));
    hfs_stat(volume, mac_path, directory_entity);

    strcpy(buffer, "");
    free((void*) mac_path);
    free((void*) concrete_path);
    free((void*) vdir);
    return 0;
}


const char* const VDIR_RSRC = ".rsrc";
const char* const VDIR_FINF = ".finf";
