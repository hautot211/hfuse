/*
File        : escape_chars.c
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
#include "debug_funcs.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void test_escape_chars(const char* const string, const char* const expected) {
  const char* const actual = escape_chars(string);
  printf("expected : %s (%ld)\n", expected, strlen(expected));
  printf("actual : %s (%ld)\n", actual, strlen(actual));
  assert(strcmp(expected, actual) == 0);
  free((void*) actual);
  printf("\n");
}

void test_unescape_chars(const char* const string, const char* const expected) {
  const char* const actual = unescape_chars(string);
  printf("expected : %s (%ld)\n", expected, strlen(expected));
  printf("actual : %s (%ld)\n", actual, strlen(actual));
  assert(strcmp(expected, actual) == 0);
  free((void*) actual);
  printf("\n");
}

int main() {
  test_escape_chars("", "");
  test_escape_chars("mytest", "mytest");
  test_escape_chars("/mytest/", "%2Fmytest%2F");
  test_escape_chars("Log File (3/18/25 4.11 PM)", "Log File (3%2F18%2F25 4.11 PM)");

  test_unescape_chars("", "");
  test_unescape_chars("mytest", "mytest");
  test_unescape_chars("%2Fmytest%2F", "/mytest/");
  test_unescape_chars("mnt/Installer Logs/Mac OS 9/Log File (3%2F18%2F25 4.11 PM)", "mnt/Installer Logs/Mac OS 9/Log File (3/18/25 4.11 PM)");
  
  return EXIT_SUCCESS;
}
