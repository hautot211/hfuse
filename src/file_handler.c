/*
File        : file_handler.c
              ( C source file )

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

#include "file_handler.h"

#include "hfs_helpers.h"
#include <stdio.h>
#include <hfs/libhfs.h>
#include <hfs/hfs.h>
#include <string.h>
#include <hfs/file.h> // fkData, fkRsrc
#include <stdlib.h>

/* Structures */

struct hfuse_handler_s {
    void* structure;        /* Pointer to 'hfsdir' or 'hfsfile' struct */
    char* macpath;          /* Mac path (separated with ':') */
    dirtype_t dirtype;      /* Type of the current directory (resource fork directory, normal data fork directory, ...) */
    enttype_t enttype;      /* Whether the structure is an 'hfsfile' or 'hfsdir' */
};

// (path, enttype) => (dirtype, either[hfsdir*, hfsfile*], macpath)

enttype_t _hfuse_handler_get_enttype(const hfuse_handler_t* const h) { return h->enttype; }
void _hfuse_handler_set_enttype(hfuse_handler_t* const h, enttype_t enttype) { h->enttype = enttype; }

void* _hfuse_handler_get_structure(const hfuse_handler_t* const self) { return self->structure; }
void _hfuse_handler_set_structure(hfuse_handler_t* const self, void* structure) { self->structure = structure; }

void hfuse_handler_open(hfuse_handler_t* const self) {
    hfsvol* const volume = hfuse_get_context_volume();
    const char* const macpath = hfuse_handler_get_macpath(self);
    enttype_t enttype = _hfuse_handler_get_enttype(self);
    dirtype_t dirtype = hfuse_handler_get_dirtype(self);
    switch(enttype) {
        case ENTTYPE_DIRECTORY:
        hfsdir* directory = hfs_opendir(volume, macpath);
        if(directory != NULL) {
            printf("VOL : %p\n", directory->vol);
            printf("DIRID : %ld\n", directory->dirid);
        }
        hfuse_handler_set_directory(self, directory);
        break;

        case ENTTYPE_FILE:
        hfsfile* file = hfs_open(volume, macpath);
        hfuse_handler_set_file(self, file);
        break;
    }
}

void _hfuse_handler_close(hfuse_handler_t* const self) {
    switch(_hfuse_handler_get_enttype(self)) {
        case ENTTYPE_DIRECTORY:
            hfsdir* const directory = hfuse_handler_get_directory(self);
            hfs_closedir(directory);
            break;

        case ENTTYPE_FILE:
            hfsfile* const file = hfuse_handler_get_file(self);
            hfs_close(file);
            break;
    }
    _hfuse_handler_set_structure(self, NULL);
}

hfuse_handler_t* const hfuse_handler_init(const char* const path, enttype_t enttype) {
    printf("CALLING hfuse_handler_init(%s)\n", path);
    hfuse_handler_t* self = (hfuse_handler_t*) malloc(sizeof(hfuse_handler_t));
    char* vdir = malloc(sizeof(char) * (HFS_MAX_FLEN + 1));
    const char* const concrete_path = trim_virtual_dir(path, vdir);
    char* macpath = strtr(NULL, concrete_path, '/', ':');
    const char* const decoded_path = unescape_chars(macpath);

    dirtype_t dirtype = get_dirtype(vdir);
    
    hfsvol* const volume = hfuse_get_context_volume();

    hfuse_handler_set_macpath(self, decoded_path);
    hfuse_handler_set_dirtype(self, dirtype);
    _hfuse_handler_set_enttype(self, enttype);
    //hfuse_handler_open(self);

    free((void*) concrete_path);
    free((void*) macpath);
    free((void*) vdir);
    return self;
}

// (dirtype, hfsdir*, macpath) => (dirtype, hfsfile*, macpath)

void hfuse_handler_destroy(hfuse_handler_t* const self) {
    _hfuse_handler_close(self);
    const char* const macpath = hfuse_handler_get_macpath(self);
    free((void*) macpath);
    free((void*) self);
}

hfsdir* const hfuse_handler_get_directory(const hfuse_handler_t* const self) {
    return (hfsdir*) _hfuse_handler_get_structure(self);
}
void hfuse_handler_set_directory(hfuse_handler_t* const self, hfsdir* const directory) {
    _hfuse_handler_set_structure(self, (void*) directory);
}

hfsfile* const hfuse_handler_get_file(const hfuse_handler_t* const self) {
    return (hfsfile*) _hfuse_handler_get_structure(self);
}
void hfuse_handler_set_file(hfuse_handler_t* const self, hfsfile* const file) {
    _hfuse_handler_set_structure(self, (void*) file);
}

dirtype_t hfuse_handler_get_dirtype(const hfuse_handler_t* const h) { return h->dirtype; }
void hfuse_handler_set_dirtype(hfuse_handler_t* const h, const dirtype_t type) { h->dirtype = type; }

const char* const hfuse_handler_get_macpath(hfuse_handler_t* const h) { return h->macpath; }
void hfuse_handler_set_macpath(hfuse_handler_t* const h, const char* const path) { h->macpath = (char*) path; }
