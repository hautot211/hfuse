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

const struct fuse_operations hfuse_operations = {
        .init           = hfuse_init,
        .destroy        = hfuse_destroy,
        .getattr        = hfuse_getattr,
        .readdir        = hfuse_readdir,
        .opendir        = hfuse_opendir,
        .releasedir     = hfuse_releasedir,
        .open           = hfuse_open,
        .flush          = hfuse_flush,
        .read           = hfuse_read,
};


void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    // printf("CALL hfuse_init\n");
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
    // printf("CALL hfuse_getattr(%s)\n", path);
    memset(stbuf, 0, sizeof(struct stat));

    /* 5 cases
         - hfs file
         - hfs folder
         - hfs symlink
         - virtual folder (.rsrc, .fdat, ...)
         - virtual files (.rsrc/foo, .fdat/bar)
     */

    /* VIRTUAL FOLDERS */
    if(endswith(path, "/.rsrc")) {
        stbuf->st_uid = getuid();
        stbuf->st_gid = getgid();
        stbuf->st_nlink = 1;
        stbuf->st_size = 4096;
        stbuf->st_mode = S_IFDIR | 0550;
        // printf("\t%s is a virtual resource fork directory\n", path);
        return 0;
    }

    /* VIRTUAL FILES */

    const char* const mac_path = to_mac_path(path);
    hfsvol* const volume = hfuse_get_context_volume();

    hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));
    int hfs_stat_res = hfs_stat(volume, mac_path, directory_entity);
    


    if(hfs_stat_res != 0) {
        free((void*) directory_entity);
        free((void*) mac_path);
        return -ENOENT;
    }
    
    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_size = 4096;
    
    if(is_directory(directory_entity)) {
        stbuf->st_mode |= S_IFDIR;
        stbuf->st_mode |= 0550;
        stbuf->st_nlink = 2;
        // printf("\t%s is a directory\n", path);
    } else if(is_symlink(directory_entity)){
        stbuf->st_mode |= S_IFLNK;
        stbuf->st_mode |= 0440;
        // printf("\t%s is a symlink\n", path);
    } else {
        // printf("\t%s is a regular file\n", path);
        stbuf->st_mode |= S_IFREG;
        stbuf->st_mode |= 0440;
        stbuf->st_nlink = 1;
        hfsfile* const file = hfs_open(volume, mac_path);
        stbuf->st_size = current_fork_size(file);
    }

    // TODO complete stat struct

    free((void*) directory_entity);
    free((void*) mac_path);
    // printf("RETURN hfuse_getattr\n");
    return 0;
}

int hfuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    printf("CALL hfuse_readdir(path = %s, offset = %d)\n", path, offset);
 
    directory_handler_t* const directory_handler = (directory_handler_t* const) fi->fh;

    off_t next_offset = offset + 1;
    switch(offset) {
        case 0:
            filler(buf, ".", NULL, next_offset, 0);
            return 0;
        case 1:
            filler(buf, "..", NULL, next_offset, 0);
            return 0;
        case 2:
            if(hfuse_directory_handler_get_type(directory_handler) == fkRsrc) break;
            filler(buf, ".rsrc", NULL, next_offset, 0);
            return 0;
        
        //default:
        //    return 0;
    }
    hfsdir* const directory = hfuse_directory_handler_get(directory_handler);
    dir_type_t type = hfuse_directory_handler_get_type(directory_handler);

    hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));

    int hfs_readdir_result = hfs_readdir(directory, directory_entity);
    if (hfs_readdir_result == -1) {
        free(directory_entity);
        if (errno == ENOENT) {
            //printf("No more entities\n");
        }
        else {
            //printf("Unknown error...\n");
        }
        return -1;
    }
    char* const subpath = malloc(sizeof(char) * (HFS_MAX_FLEN + 1));
    strcpy(subpath, directory_entity->name);
    
    filler(buf, subpath, NULL, next_offset, 0);
    free(subpath);
    free(directory_entity);

    // printf("RETURN hfuse_readdir\n");
    return 0;
}

int hfuse_opendir(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_opendir(%s)\n", path);

    dir_type_t type = endswith(path, "/.rsrc") ? fkRsrc : fkData;
    printf("path : %s (%d)\n", path, strlen(path));
    const char* const actual_path = trim_virtual_dir(path, ".rsrc");
    (actual_path == NULL) ? printf("%p", actual_path) : printf("%p  actual_path : %s (%d)\n", actual_path, actual_path, strlen(actual_path));

    const char* const mac_path = to_mac_path(actual_path);

    hfsvol* const volume = hfuse_get_context_volume();
    hfsdir* const directory = hfs_opendir(volume, mac_path);
    
    directory_handler_t* directory_handler = hfuse_directory_handler_init(directory, type);
    fi->fh = (uint64_t) directory_handler;
    
    free((void*) mac_path);
    free((void*) actual_path);
    // printf("RETURN hfuse_opendir\n");
    return 0;
}

int hfuse_releasedir(const char *path, struct fuse_file_info* fi) {
    printf("CALL hfuse_releasedir(%s)\n", path);

    directory_handler_t* directory_handler = (directory_handler_t* const) fi->fh;
    hfsdir* const directory = hfuse_directory_handler_get(directory_handler);
    
    hfs_closedir(directory);
    free((void*) directory_handler);
    fi->fh = (uint64_t) NULL;

    // printf("RETURN hfuse_releasedir\n");
    return 0;
}

int hfuse_open(const char* path, struct fuse_file_info* fi) {
    // printf("CALL hfuse_open(%s)\n", path);

    const char* concrete_path = trim_virtual_dir(path, ".rsrc");

    const char* const mac_path = to_mac_path(path);
    hfsvol* const volume = hfuse_get_context_volume();
    hfsfile* const file = hfs_open(volume, mac_path);
    //hfs_setfork(file, hfsfork);
    hfuse_set_file_info_file(fi, file);

    free((void*) mac_path);
    // printf("RETURN hfuse_open\n");
    return 0;
}

int hfuse_flush(const char* path, struct fuse_file_info* fi) {
    // printf("CALL hfuse_flush(%s)\n", path);

    hfsfile* const file = hfuse_get_file_info_file(fi);
    hfs_close(file);

    // printf("RETURN hfuse_flush\n");
    return 0;
}

int hfuse_read(const char *path, char *buffer, size_t length, off_t offset, struct fuse_file_info* fi) {
    // printf("CALL hfuse_read\n");

    hfsfile* const file = hfuse_get_file_info_file(fi);
    hfs_seek(file, offset, HFS_SEEK_SET);
    long total_read = hfs_read(file, buffer, length);

    // printf("RETURN hfuse_read\n");
    return total_read;
}
