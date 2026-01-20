#include "ntg.h"
#include <assert.h>

size_t ntg_str_count(struct ntg_strv str, char sep)
{
    size_t count = 0;

    for (size_t i = 0; i < str.len; ++i) {
        if (str.data[i] == sep)
            ++count;
    }

    return count;
}

size_t ntg_str_utf32_count(struct ntg_strv_utf32 str, uint32_t sep)
{
    size_t count = 0;

    for (size_t i = 0; i < str.len; ++i) {
        if (str.data[i] == sep)
            ++count;
    }

    return count;
}

size_t ntg_str_split(struct ntg_strv str, char sep,
                     struct ntg_strv* out, size_t cap)
{
    size_t count = 0;
    size_t start = 0;

    if (cap == 0)
        return 0;

    for (size_t i = 0; i <= str.len; ++i) {
        if (i == str.len || str.data[i] == sep) {
            if (count >= cap)
                return count;

            out[count].data = str.data + start;
            out[count].len  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}

size_t ntg_str_utf32_split(struct ntg_strv_utf32 str, uint32_t sep,
                           struct ntg_strv_utf32* out, size_t cap)
{
    size_t count = 0;
    size_t start = 0;

    if (cap == 0)
        return 0;

    for (size_t i = 0; i <= str.len; ++i) {
        if (i == str.len || str.data[i] == sep) {
            if (count >= cap)
                return count;

            out[count].data = str.data + start;
            out[count].len  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}
