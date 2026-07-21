#ifndef NTG_STR_H
#define NTG_STR_H

#include <stddef.h>
#include <stdint.h>

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

size_t ntg_str_count(struct ntg_str_view str, char sep);

size_t ntg_str32_count(struct ntg_str32_view str, uint32_t sep);

size_t ntg_str_split(struct ntg_str_view str, char sep,
        struct ntg_str_view* out_views, size_t cap);

size_t ntg_str32_split(struct ntg_str32_view str, uint32_t sep,
        struct ntg_str32_view* out_views, size_t cap);

#endif // NTG_STR_H
