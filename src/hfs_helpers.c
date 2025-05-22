#include "hfs_helpers.h"

#include "hfuse_context.h"
#include <stdlib.h>
#include <string.h>
#include <hfs/apple.h>

hfsvol* const hfuse_get_context_volume() {
    hfuse_context_t* const context = hfuse_get_context();
    return hfuse_get_volume(context);
}

hfsdir* const hfuse_get_file_info_dir(const struct fuse_file_info* const fi) {
    return (hfsdir*) fi->fh;
}

char* const to_mac_path(const char* const path) {
    char* const mac_path = malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(mac_path, path);

    for(size_t i = 0; i < strlen(mac_path); i++) {
        if (mac_path[i] == '/')
            mac_path[i] = ':';
    }
    
    return mac_path;
}


bool is_directory(const hfsdirent* const directory_entity) {
    return directory_entity->flags & HFS_ISDIR;
}

bool is_symlink(const hfsdirent* const directory_entity) {
    
}
