#include <stdlib.h>
#include <string.h>
#include <hfs.h>
#include "hfuse_context.h"

struct _hfuse_context_s {
    const char* mountpoint;
    const char* image_path;
    const hfsvol* volume;
    const hfsvolent* volume_entity;
};


const hfuse_context_t const* hfuse_init_context(const char const* mountpoint, const char const* image_path) {
    hfuse_context_t const* context = malloc(sizeof(hfuse_context_t));
    context->mountpoint = strdup(mountpoint);
    context->image_path = strdup(image_path);
    context->volume = hfs_mount(context->mountpoint, 0, HFS_MODE_RDONLY);
    context->volume_entity = malloc(sizeof(hfsvolent)); hfs_vstat(context->volume, context->volume_entity);

    return (const hfuse_context_t const*) context;
}

void hfuse_destroy_context(const hfuse_context_t const* context) {
    free(context->volume_entity);
    free(context->volume);
    free(context->image_path);
    free(context->mountpoint);
    free(context);
}

const char const* hfuse_get_mountpoint(const hfuse_context_t const context) {
    return context->mountpoint;
}

const char const* hfuse_get_image_path(const hfuse_context_t const* context) {
    return context->image_path;
}

const hfsvol const* hfuse_get_volume(const hfuse_context_t const* context) {
    return context->volume;
}

const hfsvolent const* hfuse_get_volumen_entity(const hfuse_context_t const * context) {
    return context->volume_entity;
}
