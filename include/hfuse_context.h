#include <hfs/hfs.h>

#ifndef HFUSE_CONTEXT_H
#define HFUSE_CONTEXT_H

typedef struct _hfuse_context_s hfuse_context_t;

hfuse_context_t* const hfuse_init_context(const char* const mountpoint, const char* const image_path);
void hfuse_fill_context(hfuse_context_t* const context);
void hfuse_destroy_context(const hfuse_context_t* const context);
hfuse_context_t* const hfuse_get_context();


/* Getters */

const char* const hfuse_get_mountpoint(const hfuse_context_t* const context);
const char* const hfuse_get_image_path(const hfuse_context_t* const context);
hfsvol* const hfuse_get_volume(const hfuse_context_t* const context);
hfsvolent* const hfuse_get_volume_entity(const hfuse_context_t* const context);

#endif
