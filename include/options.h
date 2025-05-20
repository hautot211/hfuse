#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>


#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct _hfuse_options_s {
    char* image_path;

} hfuse_options_t;

int hfuse_opt_proc(void* data, const char* arg, int key, struct fuse_args* outargs);


#endif