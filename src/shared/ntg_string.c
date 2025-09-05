#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "shared/ntg_string.h"

size_t _ntg_str_count(const char* str, size_t len, const char* sep, size_t data_size)
{
    assert(str != NULL);
    assert(sep != NULL);
    assert(data_size > 0);

    if(len == 0) return 0;

    size_t i;
    const char* it_char;
    size_t sum = 0;
    for(i = 0; i < len; i++)
    {
        it_char = str + (i * data_size);

        if(memcmp(it_char, sep, data_size) == 0)
            sum++;
    }

    return sum;
}

void _ntg_str_split(const char* str, size_t len, const char* sep, size_t sep_count,
        const char** out_strs, size_t* out_lens, size_t data_size)
{
    assert(str != NULL);
    assert(sep != NULL);
    assert(out_strs != NULL);
    assert(out_lens != NULL);
    assert(data_size > 0);

    if(len == 0)
    {
        assert(sep_count == 0);

        out_strs[0] = str;
        out_lens[0] = len;
        return;
    }

    if(sep_count == 0)
    {
        out_strs[0] = str;
        out_lens[0] = len;
        return;
    }
    
    size_t i;
    const char* it_char;
    size_t it_str_start = 0;
    size_t sep_counter = 0;
    // size_t it_str_count = 0;
    for(i = 0; i < len; i++)
    {
        it_char = str + (i * data_size);

        /* Found separator or end of string */
        if((memcmp(it_char, sep, data_size) == 0))
        {
            out_strs[sep_counter] = &(str[it_str_start * data_size]);
            out_lens[sep_counter] = i - it_str_start;

            it_str_start = i + 1;
            sep_counter++;
        }
    }

    out_strs[sep_count] = &(str[it_str_start * data_size]);
    out_lens[i] = i - it_str_start;
}

size_t ntg_str_count(struct ntg_str_view str, char sep)
{
    return _ntg_str_count(str.data, str.len, &sep, 1);
}

size_t ntg_str_utf32_count(struct ntg_str_utf32_view str, uint32_t sep)
{
    return _ntg_str_count((char*)str.data, str.count, (char*)(&sep),
            sizeof(uint32_t));
}

struct ntg_str_split_result ntg_str_split(struct ntg_str_view str, char sep)
{
    size_t sep_count = ntg_str_count(str, sep);
    size_t str_count = sep_count + 1;

    const char** _strs = (const char**)malloc(sizeof(char*) * str_count);
    assert(_strs != NULL);
    size_t* _lens = (size_t*)malloc(sizeof(size_t) * str_count);
    assert(_lens != NULL);

    _ntg_str_split(str.data, str.len, &sep, sep_count, _strs, _lens, sizeof(char));

    struct ntg_str_split_result result;
    result.views = (struct ntg_str_view*)malloc(sizeof(struct ntg_str_view) *
            str_count);
    result.count = str_count;
    assert(result.views != NULL);

    size_t i;
    for(i = 0; i < (sep_count + 1); i++)
    {
        result.views[i].data = _strs[i];
        result.views[i].len = _lens[i];
    }

    return result;
}

struct ntg_str_utf32_split_result ntg_str_utf32_split(struct ntg_str_utf32_view str,
        uint32_t sep)
{
    size_t sep_count = ntg_str_utf32_count(str, sep);
    size_t str_count = sep_count + 1;

    const uint32_t** _strs = (const uint32_t**)malloc(sizeof(uint32_t*) * str_count);
    assert(_strs != NULL);
    size_t* _counts = (size_t*)malloc(sizeof(size_t) * str_count);
    assert(_counts != NULL);

    _ntg_str_split((const char*)str.data, str.count, (char*)(&sep), sep_count,
            (const char**)_strs, _counts, sizeof(uint32_t));

    struct ntg_str_utf32_split_result result;
    result.views = (struct ntg_str_utf32_view*)malloc(
            sizeof(struct ntg_str_utf32_view) * str_count);
    result.count = str_count;
    assert(result.views != NULL);

    size_t i;
    for(i = 0; i < (sep_count + 1); i++)
    {
        result.views[i].data = _strs[i];
        result.views[i].count = _counts[i];
    }

    return result;
}
