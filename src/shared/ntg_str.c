#include "ntg.h"
#include <ctype.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

GENC_VECTOR_DEFINE(ntg_charvec, char, 1.5, )

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

size_t ntg_str_count(const char* text, size_t len, char sep)
{
    size_t count = 0;
    size_t pos = 0;

    while(pos < len)
    {
        const char* found = memchr(text + pos, sep, len - pos);
        if(!found) break;

        ++count;
        pos = (size_t)(found - text) + 1;
    }

    return count;
}

size_t ntg_str32_count(const uint32_t* text, size_t len, uint32_t sep)
{
    size_t count = 0;

    for(size_t i = 0; i < len; ++i)
    {
        if(text[i] == sep)
            ++count;
    }

    return count;
}

size_t ntg_str_split(const char* text, size_t len, char sep,
                     struct ntg_str_view* out_views, size_t cap)
{
    if(cap == 0)
        return 0;

    size_t count = 0;
    size_t start = 0;

    for(size_t i = 0; i <= len; ++i)
    {
        if(i == len || text[i] == sep)
        {
            if(count >= cap)
                return count;

            out_views[count].data = text + start;
            out_views[count].len = i - start;

            ++count;
            start = i + 1;
        }
    }

    return count;
}

size_t ntg_str32_split(const uint32_t* text, size_t len, uint32_t sep,
                       struct ntg_str32_view* out_views, size_t cap)
{
    if(cap == 0)
        return 0;

    size_t count = 0;
    size_t start = 0;

    for(size_t i = 0; i <= len; ++i)
    {
        if(i == len || text[i] == sep)
        {
            if(count >= cap)
                return count;

            out_views[count].data = text + start;
            out_views[count].len = i - start;

            ++count;
            start = i + 1;
        }
    }

    return count;
}

static inline bool is_ws(char c)
{
    return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\v') || (c == '\f');
}

// TODO
size_t ntg_str_rm_ws(char* text, size_t len)
{
    if(!text) return SIZE_MAX;

    size_t read_idx = 0;
    size_t write_idx = 0;

    while(read_idx < len)
    {
        while((read_idx < len) && (is_ws(text[read_idx])))
            read_idx++;

        if(read_idx == len)
            break;

        if(write_idx > 0)
        {
            text[write_idx] = ' ';
            write_idx++;
        }

        while((read_idx < len) && (!is_ws(text[read_idx])))
        {
            text[write_idx] = text[read_idx];
            write_idx++;
            read_idx++;
        }
    }

    text[write_idx] = '\0';

    return write_idx;
}
