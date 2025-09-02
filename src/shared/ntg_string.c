#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "shared/ntg_string.h"

size_t ntg_str_count(struct ntg_str_view str, char sep)
{
    if((str.data == NULL) ||(str.len == 0))
        return 0;

    size_t count = 0;
    const char* it_str = str.data;
    size_t it_len = str.len;
    const char* it_result = NULL; 
    size_t diff = 0;
    while(true)
    {
        it_result = memchr(it_str, sep, it_len);
        if(it_result == NULL) break;

        count++;
        diff = it_result - it_str;
        it_str += diff + 1;
        it_len -= diff + 1;
    }

    return count;
}

void _ntg_str_split(struct ntg_str_view str, char sep, size_t count,
        struct ntg_str_view* out_views)
{
    if(str.len == 0) 
    {
        out_views[0] = (struct ntg_str_view) {0};
        return;
    }
    if(count == 0)
    {
        out_views[0] = (struct ntg_str_view) {
            .data = str.data,
            .len = str.len
        };
        return;
    }

    size_t i;
    const char* it_str = str.data;
    size_t it_len = str.len;
    const char* it_result = NULL; 
    size_t diff = 0;
    for(i = 0; i < count; i++)
    {
        it_result = memchr(it_str, sep, it_len);
        diff = it_result - it_str;

        out_views[i] = (struct ntg_str_view) {
            .data = it_str,
            .len = diff
        };

        it_str += (diff + 1);
        it_len -= (diff + 1);
    }

    // TODO: check count calculation
    out_views[count] = (struct ntg_str_view) {
        .data = it_str,
        .len = diff
    };
}

struct ntg_str_split_result ntg_str_split(struct ntg_str_view str, char sep)
{
    size_t count = ntg_str_count(str, sep);

    struct ntg_str_view* views = (struct ntg_str_view*)malloc(
            sizeof(struct ntg_str_view) * (count + 1));
    assert(views != NULL);

    _ntg_str_split(str, sep, count, views);

    return (struct ntg_str_split_result) {
        .views = views,
        .count = count + 1
    };
}
