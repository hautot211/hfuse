#include <hfs/hfs.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>
#include <stdbool.h>


#ifndef HFS_HELPERS_H
#define HFS_HELPERS_H

hfsvol* const hfuse_get_context_volume();

// Functions to transfer and transmute data using fuse_file_info struct
hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi);
void hfuse_set_file_info_dir(struct fuse_file_info* const fi, const hfsdir* const directory);
hfsfile* const hfuse_get_file_info_file(const struct fuse_file_info* const fi);
void hfuse_set_file_info_file(struct fuse_file_info* const fi, const hfsfile* const file);

// Path helpers
char* const to_mac_path(const char* const path);

// Stat helpers
bool is_directory(const hfsdirent* const directory_entity);
bool is_symlink(const hfsdirent* const directory_entity);

// Files and Forks helpers
unsigned long fork_size(hfsfile* const file);

#endif