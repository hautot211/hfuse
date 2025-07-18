/*
File        : hfs_helpers.h
              ( C header file )

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
 * Change all occurences of character 'old' to 'new' in 'string'.
 * Can be used to change create macintosh path by changing '/' to ':'
 * If 'dst' is NULL, it is mallocd.
 * Returns the addresss of dst.
 * Mutation can be done inplace.
 */
char* const strtr(char* dst, const char* const string, char old, char new);

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
 * Escape Linux directory separator (/). It is the only character that libfuse needs to be escaped.
 */
char* const escape_chars(const char* const string);

/*
 * Opposite function of the last function
 */
char* const unescape_chars(const char* const string);

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
