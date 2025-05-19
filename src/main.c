#include <stdio.h>
#include <stdlib.h>
#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>

#include "hfuse_context.h"

extern const struct fuse_operations hfuse_operations;

int main(int argc, char *argv[]) {

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    

    const char* const image_path = "/home/guillaume/Documents/fuse-test/samples/MacOS-9.0.4-fresh.rom";
    const char* const mountpoint = "/home/guillaume/Documents/fuse-test/hfuse-profiles/hfuse-c/mnt";

    printf("Creating context\n");
    hfuse_context_t* const context = hfuse_init_context(mountpoint, image_path);
    fuse_main(args.argc, args.argv, &hfuse_operations, (void*) context);
    return 0;
}
