/*
File        : file_handler.h
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
 * The structure defined here is meant to be used with fuse_file_info.fh .
 */

#include <hfs/libhfs.h>
#include <hfs/apple.h>
#include <hfs/file.h>

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

typedef struct hfuse_handler_s hfuse_handler_t;

typedef enum {
    DIRTYPE_DATA,
    DIRTYPE_RSRC,
    DIRTYPE_FINF,
} dirtype_t;

typedef enum {
    ENTTYPE_FILE,
    ENTTYPE_DIRECTORY,
} enttype_t;

/* Construstors / Destructors */

hfuse_handler_t* const hfuse_handler_init(const char* const path, enttype_t enttype);
void hfuse_handler_destroy(hfuse_handler_t* const h);


void hfuse_handler_open(hfuse_handler_t* const self);

/* Getters / Setters */

hfsdir* const hfuse_handler_get_directory(const hfuse_handler_t* const h);
void hfuse_handler_set_directory(hfuse_handler_t* const h, hfsdir* const directory);

hfsfile* const hfuse_handler_get_file(const hfuse_handler_t* const h);
void hfuse_handler_set_file(hfuse_handler_t* const h, hfsfile* const file);

dirtype_t hfuse_handler_get_dirtype(const hfuse_handler_t* const h);
void hfuse_handler_set_dirtype(hfuse_handler_t* const h, const dirtype_t type);

const char* const hfuse_handler_get_macpath(hfuse_handler_t* const h);
void hfuse_handler_set_macpath(hfuse_handler_t* const h, const char* const path);


#endif
