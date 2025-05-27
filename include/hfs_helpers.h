#ifndef HFS_HELPERS_H
#define HFS_HELPERS_H

#include <hfs/hfs.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>
#include <stdbool.h>
#include "file_handler.h"


hfsvol* const hfuse_get_context_volume();

// fuse_file_info helpers

/* Create a fuse_file_info from an handler
 * handler must be either a file_handler_t* or a directory_handler_t*
 */
struct fuse_file_info* const file_info_init(void* handler);

hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi);
void hfuse_set_file_info_dir(struct fuse_file_info* const fi, const hfsdir* const directory);
hfsfile* const hfuse_get_file_info_file(const struct fuse_file_info* const fi);
void hfuse_set_file_info_file(struct fuse_file_info* const fi, const hfsfile* const file);

// Path helpers
/* /foo/bar (utf8) => :foo:bar (mac roman)*/
char* const to_mac_path(const char* const path);
/* "/path/to/smthng/.rsrc" endswith ".rsrc" */
bool endswith(const char* const str, const char* const suf);
/* /path/to/.rsrc/smthng => /path/to/smthng */
char* const trim_virtual_dir(const char* const path, const char* const vdir_name);
/* /path/to/not.a.rsrc/.rsrc/smthng with inf = ".rsrc/" => .rsrc/smthng */
const char* const last_occurence(const char* const str, const char* const inf);

dir_type_t get_dir_type(const char* const path);

// Stat helpers
bool is_directory(const hfsdirent* const directory_entity);
bool is_symlink(const hfsdirent* const directory_entity);

// Files and Forks helpers
unsigned long current_fork_size(hfsfile* const file);


// fuse_operations helpers

//hfuse_readdir_all(void* buf, fuse_fill_dir_t filler, off_t offset);

#endif
