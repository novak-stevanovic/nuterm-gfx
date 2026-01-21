#ifndef NTG_STRING_H
#define NTG_STRING_H

#include "shared/ntg_typedef.h"
#include <stdint.h>
#include <string.h>

struct ntg_str
{
    char* data;
    size_t len;
};

struct ntg_str_utf32
{
    uint32_t* data;
    size_t len;
};

struct ntg_strv
{
    const char* data;
    size_t len;
};

struct ntg_strv_utf32
{
    const uint32_t* data;
    size_t len;
};

static inline struct ntg_strv 
ntg_strv(const char* str, size_t len)
{
    return (struct ntg_strv) {
        .data = str,
        .len = len
    };
}

static inline struct ntg_strv 
ntg_strv_cstr(const char* cstr)
{
    return (struct ntg_strv) {
        .data = cstr,
        .len = strlen(cstr)
    };
}

static inline struct ntg_strv
ntg_strv_str(struct ntg_str str)
{
    return (struct ntg_strv) {
        .data = str.data,
        .len = str.len
    };
}

static inline struct ntg_strv_utf32
ntg_strv_utf32(const uint32_t* str, size_t len)
{
    return (struct ntg_strv_utf32) {
        .data = str,
        .len = len
    };
}

static inline struct ntg_strv_utf32
ntg_strv_str_utf32(struct ntg_str_utf32 str)
{
    return (struct ntg_strv_utf32) {
        .data = str.data,
        .len = str.len
    };
}

size_t ntg_str_count(struct ntg_strv str, char sep);
size_t ntg_str_utf32_count(struct ntg_strv_utf32 str, uint32_t sep);

size_t ntg_str_split(struct ntg_strv str, char sep,
                     struct ntg_strv* out, size_t cap);
size_t ntg_str_utf32_split(struct ntg_strv_utf32 str, uint32_t sep,
                           struct ntg_strv_utf32* out, size_t cap);

#endif // NTG_STRING_H
