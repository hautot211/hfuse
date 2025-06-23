/*
File        : debug_funcs.c
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

#include "debug_funcs.h"
#include <stdint.h>

char* const format_bits(char* const dst, void* flags, size_t bytesize) {
    const uint8_t* flags_frag = NULL;

    for(size_t byte = 0; byte < bytesize; byte++) {
        flags_frag = (const uint8_t*) flags + byte;
        for(size_t i = 0; i < 8; i++) {
            dst[byte*8 + i] = *flags_frag & (uint8_t) (1 << i) ? '1' : '0';
        }
    }

    return dst;
}


int beacon = 0;

void dbg() {
    printf("DBG %d\n", beacon++);
}

void reset_dbg() {
    beacon = 0;
}

void dbg_string(const char* const name, const char* const value) {
    if(value == NULL) {
        printf("%s : \"%p\"\n", name, value);
    }
    else {
        printf("%s : \"%s\" (%d)\n", name, value, strlen(value));
    }
}

void dbg_args(const char* const* args) {
    for(; *args != NULL; args++) {
        printf("%s ", *args);
    }
    printf("\n");
}
