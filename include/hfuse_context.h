/*
File        : hfuse_context.h
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

#include <hfs/hfs.h>

#ifndef HFUSE_CONTEXT_H
#define HFUSE_CONTEXT_H

typedef struct _hfuse_context_s hfuse_context_t;

/* Constructors / Destructors */

hfuse_context_t* const hfuse_new_context();
hfuse_context_t* const hfuse_init_context(const char* const image_path);
void hfuse_fill_context(hfuse_context_t* const context);
void hfuse_destroy_context(const hfuse_context_t* const context);


/* Getters / Setters */

hfuse_context_t* const hfuse_get_context();

const char* const hfuse_get_image_path(const hfuse_context_t* const context);
void hfuse_set_image_path(hfuse_context_t* const context, const char* const image_path);

hfsvol* const hfuse_get_volume(const hfuse_context_t* const context);
void hfuse_set_volume(hfuse_context_t* const context, hfsvol* const volume);

hfsvolent* const hfuse_get_volume_entity(const hfuse_context_t* const context);
void hfuse_set_volume_entity(hfuse_context_t* const context, hfsvolent* const volume_entity);

#endif
