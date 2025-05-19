#ifndef HFUSE_H
#define HFUSE_H

#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>

extern const struct fuse_operations hfuse_operations;

void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg);
void hfuse_destroy(void* private_data);

int hfuse_getattr(const char* path, struct stat* stbuf, struct fuse_file_info *fi);
int hfuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags);
int hfuse_opendir(const char *, struct fuse_file_info *);

#endif