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
    test_trim_virtual_dir("/Web Pages/.rsrc/About Personal WebSharing.htm", "/Web Pages/About Personal Websharing.htm", ".rsrc");


    return 0;
}