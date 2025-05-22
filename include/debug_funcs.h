#include <stdio.h>

#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H


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


static int beacon = 0;

void dbg() {
    printf("DBG %d\n", beacon++);
}

void reset_dbg() {
    beacon = 0;
}

#endif