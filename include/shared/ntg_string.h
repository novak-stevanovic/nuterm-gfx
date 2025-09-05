#ifndef _NTG_STRING_H_
#define _NTG_STRING_H_

#include <stddef.h>
#include <stdint.h>

struct ntg_str_view
{
    const char* data;
    size_t len;
};

struct ntg_str
{
    char* data;
    size_t len;
};

struct ntg_str_utf32
{
    uint32_t* data;
    size_t count; // codepoint count
};

struct ntg_str_split_result
{
    const uint32_t* data;
    size_t count;
};

size_t _ntg_str_count(const char* str, size_t len, const char* sep, size_t data_size);

void _ntg_str_split(const char* str, size_t len, const char* sep, size_t sep_count,
        const char** out_strs, size_t* out_lens, size_t data_size);

struct ntg_str_utf32_split_result
{
    struct ntg_str_utf32_view* views;
    size_t count;
};

size_t ntg_str_utf32_count(struct ntg_str_utf32_view str, uint32_t sep);

struct ntg_str_utf32_split_result ntg_str_utf32_split(struct ntg_str_utf32_view str,
        char sep);

#endif // _NTG_STRING_H_
