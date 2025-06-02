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
