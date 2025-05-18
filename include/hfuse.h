#ifndef HFUSE_H
#define HFUSE_H

const struct fuse_operations hfs_operations = {
        .init           = hfuse_init,
        .destroy        = hfuse_destroy,
        //.getattr        = hello_getattr,
        //.readdir        = hello_readdir,
        //.open           = hello_open,
        //.read           = hello_read,
};

void *hfuse_init(struct fuse_conn_info *conn, struct fuse_config *cfg);

void fuse_destroy(void* private_data);


#endif