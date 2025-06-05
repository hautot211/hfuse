/* 
 * The structure defined here is meant to be used with fuse_file_info.fh .
 */

#include <hfs/libhfs.h>
#include <hfs/apple.h>
#include <hfs/file.h>

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

typedef struct hfuse_handler_s hfuse_handler_t;

typedef enum : unsigned int {
    DIRTYPE_DATA,
    DIRTYPE_RSRC,
    DIRTYPE_FINF,
} dirtype_t;

typedef enum : unsigned int {
    ENTTYPE_FILE,
    ENTTYPE_DIRECTORY,
} enttype_t;

/* Construstors / Destructors */

hfuse_handler_t* const hfuse_handler_init(const char* const path, enttype_t enttype);
void hfuse_handler_destroy(hfuse_handler_t* const h);

/* Getters / Setters */

hfsdir* const hfuse_handler_get_directory(const hfuse_handler_t* const h);
void hfuse_handler_set_directory(hfuse_handler_t* const h, hfsdir* const directory);

hfsfile* const hfuse_handler_get_file(const hfuse_handler_t* const h);
void hfuse_handler_set_file(hfuse_handler_t* const h, hfsfile* const file);

dirtype_t hfuse_handler_get_dirtype(const hfuse_handler_t* const h);
void hfuse_handler_set_dirtype(hfuse_handler_t* const h, const dirtype_t type);

const char* const hfuse_handler_get_macpath(hfuse_handler_t* const h);
void hfuse_handler_set_macpath(hfuse_handler_t* const h, const char* const path);


#endif
