#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "hfs_helpers.h"


void test_trim_virtual_dir() {
    if (true) {
        const char* const path = "/path/to/some/file";
        const char* const concrete_path = trim_virtual_dir(path, "some");
        assert(strcmp(concrete_path, "path/to/file"));
        free((void*) concrete_path);
    }
    
    // path already concrete
    if (true) {
        const char* const path = "/path/to/some/file";
        const char* const concrete_path = trim_virtual_dir(path, "nosubstr");
        assert(strcmp(concrete_path, path) == 0);
        free((void*) concrete_path);
    }

    // concrete path is /path/to/some
    if (true) {
        const char* const path = "/path/to/some/file";
        const char* const concrete_path = trim_virtual_dir(path, "file");
        assert(strcmp(concrete_path, "/path/to/some") == 0);
        free((void*) concrete_path);
    }

    if(true)
    {
        const char* const path = "/.rsrc/";
        const char* const concrete_path = trim_virtual_dir(path, ".rsrc");
        assert(strcmp(concrete_path, "/") == 0);
        free((void*) concrete_path);
    }

    if(true)
    {
        const char* const path = "/.rsrc";
        const char* const concrete_path = trim_virtual_dir(path, ".rsrc");
        assert(strcmp(concrete_path, "/") == 0);
        free((void*) concrete_path);
    }
}

void test_get_dir_type() {
    assert(get_dir_type("/path/to/something") == fkData);
    assert(get_dir_type("/") == fkData);

    assert(get_dir_type("/path/to/something/.rsrc") == fkRsrc);
    assert(get_dir_type("/path/to/something/.rsrc/test") == fkRsrc);
}

void test_get_parent_directory() {
    {
        const char* const path = "/";
        assert(get_parent_directory(path, NULL) == NULL);
    }

    {
        const char* const path = "/test";
        const char* const parent_dir = get_parent_directory(path, NULL);
        assert(strcmp(parent_dir, "/") == 0);
        free((void*) parent_dir);
    }

    {
        const char* const path = "/path/to/.rsrc/file";
        const char* const parent_dir = get_parent_directory(path, NULL);
        assert(strcmp(parent_dir, "/path/to/.rsrc") == 0);
        free((void*) parent_dir);
    }
}

int main() {
    //test_trim_virtual_dir();
    //test_get_dir_type();
    return 0;
}