/*
File        : hfuse_context.c
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "hfuse_context.h"
#include "hfuse.h"

struct _hfuse_context_s {
    const char* image_path;
    const hfsvol* volume;
    const hfsvolent* volume_entity;
};


hfuse_context_t* const hfuse_new_context() {
    hfuse_context_t* const context = calloc(1, sizeof(hfuse_context_t));
    return context;
}

hfuse_context_t * const hfuse_init_context(const char* const image_path) {
    hfuse_context_t* const context = hfuse_new_context();
    hfuse_set_image_path(context, image_path);
    return context;
}

void hfuse_fill_context(hfuse_context_t* const context) {
    const int nparts = hfs_nparts(context->image_path);
    context->volume = NULL;
    if(nparts == -1) {
        printf("No partitions found, trying to read disk image as a partition");
        context->volume = hfs_mount(context->image_path, 0, HFS_MODE_RDONLY);
    }
    else {
        for(int curr_part = 1; curr_part <= nparts && context->volume == NULL; curr_part++) {
            printf("Trying to read partition n°%d\n", curr_part);
            context->volume = hfs_mount(context->image_path, curr_part, HFS_MODE_RDONLY);
        }
    }
    hfsvolent* const volume_entity = malloc(sizeof(hfsvolent));
    int hfs_vstat_result = hfs_vstat((hfsvol* const) context->volume, volume_entity);
    context->volume_entity = volume_entity;
}

void hfuse_destroy_context(const hfuse_context_t* const context) {
    hfs_umountall(); // destroys context->volume
    free((void*) context->volume_entity);
    free((void*) context->image_path);
    free((void*) context);
}

hfuse_context_t* const hfuse_get_context() {
    return (hfuse_context_t* const) fuse_get_context()->private_data;
}

const char const* hfuse_get_image_path(const hfuse_context_t* const context) {
    return context->image_path;
}

void hfuse_set_image_path(hfuse_context_t* const context, const char* const image_path) {
    context->image_path = strdup(image_path);
}

hfsvol* const hfuse_get_volume(const hfuse_context_t* const context) {
    return (hfsvol* const) context->volume;
}

void hfuse_set_volume(hfuse_context_t* const context, hfsvol* const volume) {
    context->volume = volume;
}

hfsvolent* const hfuse_get_volume_entity(const hfuse_context_t* const context) {
    return (hfsvolent* const) context->volume_entity;
}

void hfuse_set_volume_entity(hfuse_context_t* const context, hfsvolent* const volume_entity) {
    context->volume_entity = volume_entity;
}
