/*
File        : hfuse.h
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
