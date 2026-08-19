#include "ntg.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* COUNT */
/* ------------------------------------------------------ */

size_t ntg_str_count(struct ntg_str_view str, char sep)
{
    size_t count = 0;

    for (size_t i = 0; i < str.len; ++i)
    {
        if (str.data[i] == sep) ++count;
    }

    return count;
}

size_t ntg_str32_count(struct ntg_str32_view str, uint32_t sep)
{
    size_t count = 0;

    for (size_t i = 0; i < str.len; ++i)
    {
        if (str.data[i] == sep) ++count;
    }

    return count;
}

/* ------------------------------------------------------ */
/* SPLIT */
/* ------------------------------------------------------ */

size_t ntg_str_split(struct ntg_str_view str, char sep,
        struct ntg_str_view* out_views, size_t cap)
{
    size_t count = 0;
    size_t start = 0;
    size_t i;

    if (cap == 0)
        return 0;

    for (i = 0; i <= str.len; ++i)
    {
        if (i == str.len || str.data[i] == sep)
        {
            if (count >= cap) return count;

            out_views[count].data = str.data + start;
            out_views[count].len  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}

size_t ntg_str32_split(struct ntg_str32_view str, uint32_t sep,
        struct ntg_str32_view* out_views, size_t cap)
{
    size_t count = 0;
    size_t start = 0;
    size_t i;

    if (cap == 0)
        return 0;

    for (i = 0; i <= str.len; ++i)
    {
        if (i == str.len || str.data[i] == sep)
        {
            if (count >= cap) return count;

            out_views[count].data = str.data + start;
            out_views[count].len  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}
