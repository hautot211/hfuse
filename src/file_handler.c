#include "file_handler.h"

#include <stdio.h>
#include <hfs/libhfs.h>
#include <hfs/hfs.h>
#include <hfs/file.h> // fkData, fkRsrc
#include <stdlib.h>

/* Structures */

struct directory_handler_s {
    hfsdir* directory;
    dir_type_t type;
};

struct file_handler_s {
    hfsfile* file;
};


directory_handler_t* const hfuse_directory_handler_init(hfsdir* const directory, dir_type_t type) {
    directory_handler_t* const dh = malloc(sizeof(directory_handler_t));
    hfuse_directory_handler_set(dh, directory);
    hfuse_directory_handler_set_type(dh, type);
    return dh;
}

hfsdir* const hfuse_directory_handler_get(const directory_handler_t* const dh) {
    return dh->directory;
}
void hfuse_directory_handler_set(directory_handler_t* const dh, hfsdir* const directory) {
    dh->directory = directory;
}

dir_type_t hfuse_directory_handler_get_type(const directory_handler_t* const dh) {
    return dh->type;
}
void hfuse_directory_handler_set_type(directory_handler_t* const dh, const dir_type_t type) {
    dh->type = type;
}



file_handler_t* const hfuse_file_handler_init(hfsfile* const file) {
    file_handler_t* const fh = malloc(sizeof(file_handler_t));
    hfuse_file_handler_set(fh, file);
    return fh;
}

hfsfile* const hfuse_file_handler_get(file_handler_t* const fh) {
    return fh->file;
}
void hfuse_file_handler_set(file_handler_t* const fh, hfsfile* const file) {
    fh->file = file;
}

