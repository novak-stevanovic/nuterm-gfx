#ifndef NTG_STRING_H
#define NTG_STRING_H

#include "shared/ntg_shared.h"
#include <stddef.h>
#include <stdint.h>

size_t ntg_str_count(const char* str, size_t len, char sep);
size_t ntg_str_utf32_count(const uint32_t* str, size_t len, uint32_t sep);

size_t ntg_str_split(
        const char* str,
        size_t len,
        char sep,
        const char** out_views,
        size_t* out_lens,
        size_t cap);

size_t ntg_str_utf32_split(
        const uint32_t* str,
        size_t len,
        uint32_t sep,
        const uint32_t** out_views,
        size_t* out_lens,
        size_t cap);

#endif // NTG_STRING_H
