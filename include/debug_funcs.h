#include <stdio.h>
#include <string.h>

#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H


char* const format_bits(char* const dst, void* flags, size_t bytesize);

extern int beacon;

void dbg();

void reset_dbg();

void dbg_string(const char* const name, const char* const value);

void dbg_args(const char* const* args);

#endif