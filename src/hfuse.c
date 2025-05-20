#include "hfuse.h"
#include "hfs_helpers.h"
#include "hfuse_context.h"
#include <hfs.h>

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
        //.open           = hello_open,
        //.read           = hello_read,
};

void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    printf("CALL hfuse_init\n");
    cfg->kernel_cache = 0;
    hfuse_context_t* const context = hfuse_get_context();
    hfuse_fill_context(context);
    printf("context : %p\n", context);
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

    hfuse_context_t* const context = hfuse_get_context();
    hfsdirent* const dir_entity = malloc(sizeof(hfsdirent));
    int hfs_stat_res = hfs_stat(hfuse_get_volume(context), mac_path, dir_entity);
    if(hfs_stat_res != 0) {
        //printf("error for hfs_stat\n");
        printf("RETURN hfuse_getattr\n");
        return -ENOENT;
    }
    

    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_mode = S_IFDIR |0440;
    stbuf->st_nlink = 2;
    // TODO complete stat struct

    free((void*)mac_path);
    printf("RETURN hfuse_getattr\n");
    return 0;
}

int hfuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    printf("CALL hfuse_readdir(%s)\n", path);
    if (strcmp(path, "/") != 0)
                return -ENOENT;
 
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    printf("RETURN hfuse_readdir\n");
    return 0;
}

int hfuse_opendir(const char* path, struct fuse_file_info* fi) {
    printf("CALL hfuse_opendir(%s)\n", path);

    const char* const mac_path = to_mac_path(path);
    printf("mac_path : \"%s\"\n", mac_path);

    hfsvol* const volume = hfuse_get_context_volume();
    printf("volume : %p\n", volume);
    hfsdir* const directory = hfs_opendir(volume, mac_path);
    printf("directory %p\n", directory);


    fi->fh = (uint64_t) directory;
    
    free((void*) mac_path);
    printf("RETURN hfuse_opendir\n");
    return 0;
}

int hfuse_releasedir(const char *path, struct fuse_file_info* fi) {
    printf("CALL hfuse_releasedir(%s)\n", path);

    hfsdir* const directory = hfuse_get_file_info_dir(fi);
    hfs_closedir(directory);
    printf("destroying %p\n", directory);

    printf("RETURN hfuse_releasedir\n");
    return 0;
}
