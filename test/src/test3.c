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
