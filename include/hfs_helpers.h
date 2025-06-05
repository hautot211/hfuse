/*
 * Helper functions for libhfs
 */

#include <hfs/hfs.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>
#include <stdbool.h>
#include "file_handler.h"


#ifndef HFS_HELPERS_H
#define HFS_HELPERS_H

/*
 * 'Entities' refers to name of a directory or file (not the absolute path)
 * 'hfsdirent' refers to a data structure from libhfs that is common to files and directories
 */

/*
 * Return the volume inside the current fuse context.
 */
hfsvol* const hfuse_get_context_volume();

// fuse_file_info helpers

/*
 * Create a fuse_file_info from an handler
 * handler must be either a file_handler_t* or a directory_handler_t*
 */
struct fuse_file_info* const file_info_init(void* handler);

// Getters and setters for fuse_file_info

hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi);
void hfuse_set_file_info_dir(struct fuse_file_info* const fi, const hfsdir* const directory);
hfsfile* const hfuse_get_file_info_file(const struct fuse_file_info* const fi);
void hfuse_set_file_info_file(struct fuse_file_info* const fi, const hfsfile* const file);

// Path helpers

/*
 * Create a path separated by ':' from a path separated by '/' (required for usage of libhfs).
 * A new string is created, path is not modified inplace
 */
char* const to_mac_path(const char* const path);

/*
 * Tests if 'suf' is a suffix of 'str'
 */
bool endswith(const char* const str, const char* const suf);

/*
 * Returns the address of the last occurence of 'inf' in 'str'
 */
const char* const last_occurence(const char* const str, const char* const inf);

/*
 * Takes a path and write the parent directory of it to the buffer.
 * If buffer is NULL, a new buffer of size HFS_MAX_FLEN + 1 is allocated on heap.
 * Returns the adresse of the buffer, or NULL if path has no parent directories.
 */
char* const get_parent_directory(const char* const path, char* const buffer);

/*
 * Take a path, and return the effective path of the object inside HFS.
 * The name of the virtual directory is written in vdir (vdir requires to be at least HFS_MAX_FLEN + 1 in size)
 * vdir is set to empty string if path does not point to a virtual file or directory.
 */
char* const trim_virtual_dir(const char* const path, char* vdir);

/*
 * Return the dir type of an entity
 */
dirtype_t get_dirtype(const char* const entity_name);


// Stat helpers

/*
 * Test if a hfsdirent is a file or directory
 */
bool is_directory(const hfsdirent* const directory_entity);
/*
 * Tests if a hfsdirent is a symlink or not
 */
bool is_symlink(const hfsdirent* const directory_entity);

// Files and Forks helpers

/*
 * Returns the size of the fork of the current file.
 */
unsigned long current_fork_size(hfsfile* const file);


#endif
