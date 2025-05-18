#include "hfuse.h"
#include "hfuse_context.h"

void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    cfg->kernel_cache = 0;
    return (void*) hfuse_init_context();
}


void fuse_destroy(void* private_data) {
    const hfuse_context_t const* context = (const hfuse_context_t const*) private_data;
    hfsue_destroy_context(context); 
}