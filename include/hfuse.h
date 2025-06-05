/*
 * Set of functions that fuse will use to open and read files and directories
 * <https://libfuse.github.io/doxygen/structfuse__operations.html>
 */

#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>


#ifndef HFUSE_H
#define HFUSE_H

/* Fuse operations */

extern const struct fuse_operations hfuse_operations;

void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg);
void hfuse_destroy(void* private_data);

int hfuse_getattr(const char* path, struct stat* stbuf, struct fuse_file_info *fi);

int hfuse_opendir(const char* path, struct fuse_file_info* fi);
int hfuse_releasedir(const char *path, struct fuse_file_info* fi);
int hfuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags);

int hfuse_open(const char* path, struct fuse_file_info* fi);
int hfuse_release(const char* path, struct fuse_file_info* fi);
int hfuse_read(const char *path, char *buffer, size_t length, off_t offset, struct fuse_file_info* fi);

int hfuse_readlink(const char* path, char* buffer, size_t length);


/* Constants */

extern const char* const VDIR_RSRC;
extern const char* const VDIR_FINF;

#endif