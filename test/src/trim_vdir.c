/*
File        : trim_vdir.c
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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void test_trim_virtual_dir(const char* const path, const char* const expected_concrete_path, const char* const expected_vdir) {
    char* const vdir = malloc(33);
    
    const char* const concrete_path = trim_virtual_dir(path, vdir);
    printf("path : %s\n", path);
    printf("(expected)\t\t concrete_path : '%s', vdir: '%s'\n", expected_concrete_path, expected_vdir);
    printf("(actual)\t\t concrete_path : '%s', vdir: '%s'\n", concrete_path, vdir);

    assert(strcmp(concrete_path, expected_concrete_path) == 0);
    assert(strcmp(vdir, expected_vdir) == 0);
    
    free((void*) concrete_path);
    free((void*) vdir);
    printf("\n");
}

int main() {

    test_trim_virtual_dir("/", "/", "");
    test_trim_virtual_dir("/.rsrc", "/", ".rsrc");
    test_trim_virtual_dir("/.rsrc/file", "/file", ".rsrc");
    test_trim_virtual_dir("/.notvirtual/file", "/.notvirtual/file", "");
    test_trim_virtual_dir("/test", "/test", "");
    test_trim_virtual_dir("/Web Pages/.rsrc/About Personal WebSharing.htm", "/Web Pages/About Personal WebSharing.htm", ".rsrc");
    test_trim_virtual_dir("/Apple Extras/.rsrc/Register with Apple", "/Apple Extras/Register with Apple", ".rsrc");
    test_trim_virtual_dir("/Applications/Security", "/Applications/Security", "");


    return 0;
}
