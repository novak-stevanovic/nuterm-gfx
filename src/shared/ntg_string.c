#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

size_t ntg_str_count(const char* str, size_t len, char sep)
{
    size_t count = 0;

    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] == sep) ++count;
    }

    return count;
}

size_t ntg_str_utf32_count(const uint32_t* str, size_t len, uint32_t sep)
{
    size_t count = 0;

    for (size_t i = 0; i < len; ++i)
    {
        if (str[i] == sep) ++count;
    }

    return count;
}

size_t ntg_str_split(const char* str, size_t len, char sep,
                     const char** out_views, size_t* out_lens,
                     size_t cap)
{
    assert(str);
    assert(out_views);
    assert(out_lens);

    size_t count = 0;
    size_t start = 0;
    size_t i;

    if (cap == 0)
        return 0;

    for (i = 0; i <= len; ++i)
    {
        if (i == len || str[i] == sep)
        {
            if (count >= cap) return count;

            out_views[count] = str + start;
            out_lens[count]  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}

size_t ntg_str_utf32_split(
        const uint32_t* str,
        size_t len,
        uint32_t sep,
        const uint32_t** out_views,
        size_t* out_lens,
        size_t cap)
{
    assert(str);
    assert(out_views);
    assert(out_lens);

    size_t count = 0;
    size_t start = 0;
    size_t i;

    if (cap == 0)
        return 0;

    for (i = 0; i <= len; ++i)
    {
        if (i == len || str[i] == sep)
        {
            if (count >= cap) return count;

            out_views[count] = str + start;
            out_lens[count]  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}
