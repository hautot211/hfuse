#include <hfs.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>


#ifndef HFS_HELPERS_H
#define HFS_HELPERS_H

hfsvol* const hfuse_get_context_volume();
hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi);

char* const to_mac_path(const char* const path);


#endif