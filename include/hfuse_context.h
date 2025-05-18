#ifndef HFUSE_CONTEXT_H
#define HFUSE_CONTEXT_H

#include <hfs.h>

typedef struct _hfuse_context_s hfuse_context_t ;

const hfuse_context_t const* hfuse_init_context(const char const* mountpoint);
void hfuse_destroy_context(hfuse_context_t const* context);

const char const* hfuse_get_mountpoint(const hfuse_context_t const* context);
const char const* hfuse_get_image_path(const hfuse_context_t const* context);
const hfsvol const* hfuse_get_volume(const hfuse_context_t const* context);
const hfsvolent const* hfuse_get_volumen_entity(const hfuse_context_t const * context);

#endif
