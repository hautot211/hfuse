#include "hfuse.h"
#include "hfs_helpers.h"
#include "hfuse_context.h"
#include "debug_funcs.h"

#include <hfs/libhfs.h>
#include <hfs/hfs.h>

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
    printf("CALL hfuse_init\n");
    cfg->kernel_cache = 0;
    hfuse_context_t* const context = hfuse_get_context();
    hfuse_fill_context(context);
    // printf("context : %p\n", context);
    printf("RETURN hfuse_init\n");
    return (void*) context;
}


void hfuse_destroy(void* private_data) {
    printf("CALL hfuse_destroy\n");
    const hfuse_context_t const* context = (const hfuse_context_t const*) private_data;
    hfuse_destroy_context(context);
    printf("RETURN hfuse_destroy\n");
}


int hfuse_getattr(const char *path, struct stat * stbuf, struct fuse_file_info *fi) {
    printf("CALL hfuse_getattr(%s)\n", path);
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    const char* const mac_path = to_mac_path(path);
    // printf("mac_path : %s\n", mac_path);

    hfsvol* const volume = hfuse_get_context_volume();

    hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));
    int hfs_stat_res = hfs_stat(volume, mac_path, directory_entity);
    


    if(hfs_stat_res != 0) {
        printf("no such thing as %s\n", path);
        // printf("ERROR hfuse_getattr(%s)\n", path);
        return -ENOENT;
    }

    char* const flag_str = calloc(33, sizeof(char));
    //printf("flags : %s\n", format_bits(flag_str, (void*) &directory_entity->flags, sizeof(int)));
    memset(flag_str, 0, 32);
    //printf("fdflags : %s\n", format_bits(flag_str, (void*) &directory_entity->fdflags, sizeof(short)));
    free((void*) flag_str);
    //printf("\"%s\" is %s\n", mac_path, (directory_entity->flags & HFS_ISDIR ? "a directory" : "NOT a directory"));
    
    
    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_nlink = 2;
    stbuf->st_size = 10;
    
    if(is_directory(directory_entity)) {
        stbuf->st_mode |= S_IFDIR;
        printf("%s is a directory\n", path);
    } else if(is_symlink(directory_entity)){
        stbuf->st_mode |= S_IFLNK;
        printf("%s is a symlink\n", path);
    } else {
        printf("%s is a regular file\n", path);
        stbuf->st_mode |= S_IFREG;
        hfsfile* const file = hfs_open(volume, mac_path);
        stbuf->st_size = fork_size(file);
    }

    stbuf->st_mode |= 0440;
    // TODO complete stat struct

    free((void*) directory_entity);
    free((void*) mac_path);
    printf("RETURN hfuse_getattr\n");
    return 0;
}

int hfuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    printf("CALL hfuse_readdir(path = %s, offset = %d)\n", path, offset);
 
    hfsdir* const directory = hfuse_get_file_info_dir(fi);

    off_t next_offset = offset + 1;
    switch(offset) {
        case 0:
            filler(buf, ".", NULL, next_offset, 0);
            break;
        case 1:
            filler(buf, "..", NULL, next_offset, 0);
        default:
            hfsdirent* const directory_entity = malloc(sizeof(hfsdirent));
            int hfs_readdir_result = hfs_readdir(directory, directory_entity);
            if (hfs_readdir_result == -1) {
                free(directory_entity);
                return -1;
            }
            char* const subpath = malloc(sizeof(char) * (HFS_MAX_FLEN + 1));
            strcpy(subpath, directory_entity->name);

            filler(buf, subpath, NULL, next_offset, 0);
            free(subpath);
    }


    printf("RETURN hfuse_readdir\n");
    return 0;
}

int hfuse_opendir(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_opendir(%s)\n", path);

    const char* const mac_path = to_mac_path(path);
    printf("mac_path : \"%s\"\n", mac_path);

    hfsvol* const volume = hfuse_get_context_volume();
    hfsdir* const directory = hfs_opendir(volume, mac_path);
    hfuse_set_file_info_dir(fi, directory);
    
    free((void*) mac_path);
    printf("RETURN hfuse_opendir\n");
    return 0;
}

int hfuse_releasedir(const char *path, struct fuse_file_info* fi) {
    printf("CALL hfuse_releasedir(%s)\n", path);

    hfsdir* const directory = hfuse_get_file_info_dir(fi);
    hfs_closedir(directory);

    printf("RETURN hfuse_releasedir\n");
    return 0;
}

int hfuse_open(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_open(%s)\n", path);
    
    const char* const mac_path = to_mac_path(path);
    hfsvol* const volume = hfuse_get_context_volume();
    hfsfile* const file = hfs_open(volume, mac_path);
    hfuse_set_file_info_file(fi, file);

    printf("hfsfile at %p\n", file);
    
    free((void*) mac_path);
    printf("RETURN hfuse_open\n");
    return 0;
}

int hfuse_flush(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_flush(%s)\n", path);

    hfsfile* const file = hfuse_get_file_info_file(fi);
    hfs_close(file);

    printf("RETURN hfuse_flush\n");
    return 0;
}

int hfuse_read(const char *path, char *buffer, size_t length, off_t offset, struct fuse_file_info* fi) {
    printf("CALL hfuse_read\n");

    hfsfile* const file = hfuse_get_file_info_file(fi);
    dbg();
    hfs_seek(file, offset, HFS_SEEK_SET);
    dbg();
    long total_read = hfs_read(file, buffer, length);
    dbg();

    printf("total_read : %d\n", total_read);

    printf("RETURN hfuse_read\n");
    return total_read;
}
