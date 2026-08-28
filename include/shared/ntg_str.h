#ifndef NTG_STR_H
#define NTG_STR_H

#include <stddef.h>
#include <stdint.h>
#include "shared/ntg_shared.h"
#include "thirdparty/genc.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

GENC_VECTOR_DECLARE(ntg_charvec, char, NTG_API)

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_str
{
    char* data;
    size_t len;
};

struct ntg_str32
{
    uint32_t* data;
    size_t len;
};

struct ntg_str_view
{
    const char* data;
    size_t len;
};

struct ntg_str32_view
{
    const uint32_t* data;
    size_t len;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static inline struct ntg_str_view
ntg_str_get_view(struct ntg_str string, size_t offset)
{
    if(offset > string.len) offset = string.len;

    return (struct ntg_str_view){
        .data = string.data + offset,
        .len  = string.len - offset
    };
}

static inline struct ntg_str32_view
ntg_str32_get_view(struct ntg_str32 string, size_t offset)
{
    if(offset > string.len) offset = string.len;

    return (struct ntg_str32_view){
        .data = string.data + offset,
        .len  = string.len - offset
    };
}

NTG_API size_t
ntg_str_count(const char* text, size_t len, char sep);

NTG_API size_t
ntg_str32_count(const uint32_t* text, size_t len, uint32_t sep);

NTG_API size_t
ntg_str_split(const char* text, size_t len, char sep,
              struct ntg_str_view* out_views, size_t cap);

NTG_API size_t
ntg_str32_split(const uint32_t* text, size_t len, uint32_t sep,
                struct ntg_str32_view* out_views, size_t cap);

/* SIZE_MAX on failure */
NTG_API size_t
ntg_str_rm_ws(char* text, size_t len);

#endif // NTG_STR_H
