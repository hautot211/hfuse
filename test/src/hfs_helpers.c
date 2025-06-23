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

void test_get_dirtype() {
    assert(get_dirtype("/path/to/something") == fkData);
    assert(get_dirtype("/") == fkData);

    assert(get_dirtype("/path/to/something/.rsrc") == fkRsrc);
    assert(get_dirtype("/path/to/something/.rsrc/test") == fkRsrc);
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
    //test_get_dirtype();
    return 0;
}
