#include "hfuse_context.h"
#include "hfuse.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct _hfuse_context_s {
    const char* mountpoint;
    const char* image_path;
    const hfsvol* volume;
    const hfsvolent* volume_entity;
};


hfuse_context_t * const hfuse_init_context(const char* const mountpoint, const char* const image_path) {
    hfuse_context_t * const context = malloc(sizeof(hfuse_context_t));
    context->mountpoint = strdup(mountpoint);
    context->image_path = strdup(image_path);
    return context;
    //return (hfuse_context_t const*) context;
}

void hfuse_fill_context(hfuse_context_t* const context) {

    context->volume = hfs_mount(context->image_path, 0, HFS_MODE_RDONLY);
    printf("FINISHED\n");

    hfsvolent* const volume_entity = malloc(sizeof(hfsvolent));
    hfs_vstat((hfsvol* const) context->volume, volume_entity);
    context->volume_entity = volume_entity;
}

void hfuse_destroy_context(const hfuse_context_t* const context) {
    hfs_umountall();
    free((void*) context->volume_entity);
    free((void*) context->image_path);
    free((void*) context->mountpoint);
    free((void*) context);
}

hfuse_context_t* const hfuse_get_context() {
    return (hfuse_context_t* const) fuse_get_context()->private_data;
}

const char const* hfuse_get_mountpoint(const hfuse_context_t* const context) {
    return context->mountpoint;
}

const char const* hfuse_get_image_path(const hfuse_context_t* const context) {
    return context->image_path;
}

hfsvol* const hfuse_get_volume(hfuse_context_t* const context) {
    return context->volume;
}

hfsvolent* const hfuse_get_volumen_entity(hfuse_context_t* const context) {
    return context->volume_entity;
}
