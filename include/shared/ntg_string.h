#ifndef _NTG_STRING_H_
#define _NTG_STRING_H_

#include <stddef.h>

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

size_t ntg_str_count(struct ntg_str_view str, char sep);

/* `count` represents how many times `sep` occurs in `str`.
 * `out_views` has to have capacity `count` + 1.
 *
 * Calling the function if `str.data` == NULL is undefinded behavior. */
void _ntg_str_split(struct ntg_str_view str, char sep, size_t count,
        struct ntg_str_view* out_views);

struct ntg_str_split_result
{
    struct ntg_str_view* views;
    size_t count;
};

struct ntg_str_split_result ntg_str_split(struct ntg_str_view str, char sep);

#endif // _NTG_STRING_H_
