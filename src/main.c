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
    hfuse_fill_context(context);

    return fuse_main(args.argc, args.argv, &hfuse_operations, (void*) context);
}
