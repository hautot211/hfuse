/* 
 * The two structures defined here (file_handler_t and directory_handler_t) are meant to be used with
 * fuse_file_info.fh .
 */

#include <hfs/libhfs.h>
#include <hfs/apple.h>
#include <hfs/file.h>

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

typedef struct file_handler_s file_handler_t;
typedef struct directory_handler_s directory_handler_t;

typedef int dir_type_t;
enum {
//  fkData  = 0x00,              /* Defined in 'hfs/file.h' */
//  fkRsrc  = 0xff,
    hfsFdat = 0x0f,
};

directory_handler_t* const hfuse_directory_handler_init(hfsdir* const directory, dir_type_t type);

hfsdir* const hfuse_directory_handler_get(const directory_handler_t* const dh);
void hfuse_directory_handler_set(directory_handler_t* const dh, hfsdir* const directory);

dir_type_t hfuse_directory_handler_get_type(const directory_handler_t* const dh);
void hfuse_directory_handler_set_type(directory_handler_t* const dh, const dir_type_t);



file_handler_t* const hfuse_file_handler_init(hfsfile* const file);

hfsfile* const hfuse_file_handler_get(file_handler_t* const fh);
void hfuse_file_handler_set(file_handler_t* const fh, hfsfile* const file);



#endif
