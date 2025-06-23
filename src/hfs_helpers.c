/*
File        : hfs_helpers.c
              ( C source file )

Description : GREYC's C project to mount standard HFS disk images on Linux virtual filesystem

Author   : Guillaume HAUTOT

License     : MIT

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "hfs_helpers.h"

#include "hfuse_context.h"
#include "hfuse.h"
#include "debug_funcs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <hfs/apple.h>
#include <math.h>
#include <hfs/hfs.h>
#include <hfs/libhfs.h>

#define MAX(x, y) (x > y ? x : y)

struct fuse_file_info* const file_info_init(void* handler) {
    struct fuse_file_info* const fi = calloc(1, sizeof(struct fuse_file_info));
    fi->fh = (uint64_t) handler;
    return fi;
}

hfsvol* const hfuse_get_context_volume() {
    hfuse_context_t* const context = hfuse_get_context();
    return hfuse_get_volume(context);
}

hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi) {
    return (hfsdir*) fi->fh;
}
void hfuse_set_file_info_dir(struct fuse_file_info* const fi, const hfsdir* const directory) {
    fi->fh = (uint64_t) directory;
}
hfsfile* const hfuse_get_file_info_file(const struct fuse_file_info* const fi) {
    return (hfsfile*) fi->fh;
}
void hfuse_set_file_info_file(struct fuse_file_info* const fi, const hfsfile* const file) {
    fi->fh = (uint64_t) file;
}

char* const strtr(char* dst, const char* const string, char old, char new) {
  if(dst == NULL) {
    dst = strdup(string);
  } else if(string == dst) {
    dst = strcpy(dst, string);
  }

  const char* string_iter = NULL;
  char* dst_iter = NULL;
  for(string_iter = string, dst_iter = dst; *string_iter != '\0'; string_iter++, dst_iter++) {
    if(*string_iter == old) *dst_iter = new;
  }

  return dst;
}

bool endswith(const char* const str, const char* const suf) {
    size_t str_len = strlen(str);
    size_t suf_len = strlen(suf);

    if(suf_len > str_len) return false;
    for(size_t i = 0; i < suf_len; i++) {
        if(str[str_len - i - 1] != suf[suf_len - i - 1]) return false;
    }
    return true;
}

const char* const last_occurence(const char* const str, const char* const inf) {
    const char* last = NULL;
    const char* curr = str;
    
    while (true) {
        curr = strstr(curr, inf);
        if(curr == NULL) {
            return last;
        }
        last = curr;
        curr++;
    }
}

char* const get_parent_directory(const char* const path, char* buffer) {
    if(strcmp(path, "/") == 0) return NULL;
    size_t path_len = strlen(path);

    const char* const filename = last_occurence(path, "/");
    size_t filename_len = strlen(filename);
    
    size_t buffer_len = MAX(path_len - filename_len, 1);
    if(buffer == NULL) buffer = calloc(sizeof(char), buffer_len + 1);
    memcpy(buffer, path, buffer_len);
    buffer[0] = '/';

    return buffer;
}

char* const trim_virtual_dir(const char* const path, char* vdir) {
    /* Case path is root */
    if(strcmp(path, "/") == 0) {
        strcpy(vdir, "");
        char* const concrete_path = malloc(2 * sizeof(char)); // returned
        strcpy(concrete_path, "/");
        return concrete_path;
    }

    /* Case path is a virtual directory */
    const char* const current_entity = last_occurence(path, "/");
    if(get_dirtype(current_entity + 1) != DIRTYPE_DATA) {
        strcpy(vdir, current_entity + 1);
        char* const concrete_path = get_parent_directory(path, NULL); // returned
        return concrete_path;
    }

    /* Case path is a virtual file */
    const char* const parent_path = get_parent_directory(path, NULL); // freed
    const char* const parent_entity = last_occurence(parent_path, "/");
    size_t path_len = strlen(path);
    if(get_dirtype(parent_entity + 1) != DIRTYPE_DATA) {
        strcpy(vdir, parent_entity + 1);
        size_t parent_entity_len = strlen(parent_entity);
        size_t concrete_path_len = path_len - parent_entity_len;
        char* concrete_path = calloc(concrete_path_len + 1, sizeof(char)); // returned
        concrete_path = get_parent_directory(parent_path, concrete_path);
        if(strcmp(concrete_path, "/") == 0) strcpy(concrete_path, "");
        concrete_path = strcat(concrete_path, current_entity);
        free((void*) parent_path);
        return concrete_path;
    }
    free((void*) parent_path);

    /* Case path is a concrete file or directory*/
    size_t concrete_path_len = path_len;
    char* const concrete_path = malloc((concrete_path_len + 1) * sizeof(char)); // returned
    strncpy(concrete_path, path, concrete_path_len + 1);
    strncpy(vdir, "", 1);

    return concrete_path;
}

char* const escape_chars(const char* const string) {
  const char* string_iter = string;

  /* Count the number of caracters that sould be escaped */
  size_t escape_count = 0;
  while(*string_iter != '\0') {
    if(strchr("/", *string_iter)) escape_count++;
    string_iter++;
  }
  
  /* Allocate the escaped string */
  size_t string_len = string_iter - string;
  size_t escaped_len = string_len + escape_count*3;
  char* const escaped = malloc(sizeof(char) * (escaped_len + 1));

  /* Build the escaped string */
  char* escaped_iter = escaped;
  char test_string[2] = " ";
  string_iter = string;
  for(string_iter = string; *string_iter != '\0'; string_iter++, escaped_iter++) {
    test_string[0] = *string_iter;
    if(strchr("/", *string_iter) != NULL) {
      strcpy(escaped_iter, "%2F");
      escaped_iter += 2;
      continue;
    }
    *escaped_iter = *string_iter;
  }
  *escaped_iter = '\0';

  return escaped;
}

char* const unescape_chars(const char* const string) {
  const char* string_iter = NULL;

  /* Count the number of escaped caracters */
  size_t escaped_count = -1;
  for(string_iter = string - 1; string_iter != NULL; string_iter = strstr(string_iter + 1, "%2F"))
    escaped_count++;

  /* Allocate the unescaped string */
  size_t string_len = strlen(string);
  size_t unescaped_len = string_len - (escaped_count * 2);
  char* const unescaped = malloc(sizeof(char) * (unescaped_len + 1));

  /*Build the unescaped string */
  char* unescaped_iter = unescaped;
  const char* next_escape = strstr(string, "%2F");
  for(string_iter = string; *string_iter != '\0'; string_iter++, unescaped_iter++) {
    if(string_iter == next_escape) {
      *unescaped_iter = '/';
      string_iter += 2;
    } else *unescaped_iter = *string_iter;
    next_escape = strstr(string_iter, "%2F");
  }
  *unescaped_iter = '\0';

  return unescaped;
}

dirtype_t get_dirtype(const char* const path) {
    if(strcmp(path, VDIR_RSRC) == 0) return DIRTYPE_RSRC;
    if(strcmp(path, VDIR_FINF) == 0) return DIRTYPE_FINF;
    return DIRTYPE_DATA;
}


bool is_directory(const hfsdirent* const directory_entity) {
    return directory_entity->flags & HFS_ISDIR;
}

bool is_symlink(const hfsdirent* const directory_entity) {
    /* Read finder info and strcmp with 'hlnk' or something */
    return strcmp(directory_entity->u.file.creator, "hbwr") == 0;
}


unsigned long current_fork_size(hfsfile* const file) {
    unsigned long current = file->pos;
    hfs_seek(file, 0, HFS_SEEK_SET);
    unsigned long begin = file->pos;
    hfs_seek(file, 0, HFS_SEEK_END);
    unsigned long end = file->pos;

    hfs_seek(file, current, HFS_SEEK_SET);
    return end - begin;
}
