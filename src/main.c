/*
File        : main.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>

#include "debug_funcs.h"
#include "hfuse_context.h"

extern const struct fuse_operations hfuse_operations;

static int hfuse_opt_proc(void *data, const char* arg, int key, struct fuse_args* outargs) {
    hfuse_context_t* context = data;
    if(key == FUSE_OPT_KEY_NONOPT && hfuse_get_image_path(context) == NULL) {
        hfuse_set_image_path(context, arg);
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if(argc <= 2) {
        fprintf(stderr, "Usage: %s <image> <mountpoint> [fuse args...]\n", argv[0]);
        return 1;
    }
    
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    hfuse_context_t* const context = hfuse_new_context();
    fuse_opt_parse(&args, context, NULL, hfuse_opt_proc);

    /* Disactivae multithreading */
    fuse_opt_add_arg(&args, "-s");
    hfuse_fill_context(context);

    return fuse_main(args.argc, args.argv, &hfuse_operations, (void*) context);
}
