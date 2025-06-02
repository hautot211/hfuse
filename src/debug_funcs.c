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