#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "shared/ntg_string.h"

size_t _ntg_str_count(const char* str, size_t len, const char* sep, size_t data_size)
{
    assert(str != NULL);
    assert(len > 0);
    assert(sep != NULL);
    assert(data_size > 0);

    size_t i;
    const char* it_char;
    size_t sum = 0;
    for(i = 0; i < len; i++)
    {
        it_char = str + (i * data_size);

        if(memcmp(it_char, sep, data_size) == 0) sum++;
    }

    return sum;
}

void _ntg_str_split(const char* str, size_t len, const char* sep, size_t data_size,
        size_t sep_count, const char** out_strs, size_t* out_lens)
{
    assert(str != NULL);
    assert(len > 0);
    assert(sep != NULL);
    assert(data_size > 0);
    assert(out_strs != NULL);
    assert(out_lens != NULL);

    size_t i;
    size_t it_start = 0;
    const char* it_char;
    size_t it_len;
    size_t sep_counter = 0;
    for(i = 0; i < len; i++)
    {
        it_char = str + (i * data_size);

        if(memcmp(it_char, sep, data_size) == 0) // found sep
        {
            it_len = i - it_start;

            out_strs[i] = &(str[it_start * data_size]);
            out_lens[i] = it_len;

            sep_counter++;

            if(sep_counter == sep_count)
            {
                out_strs[sep_count - 1] = &(str[(it_start + it_len) * data_size]);
                out_lens[sep_count - 1] = len - (it_start + it_len);

                break;
            }

            it_start = i + 1;
        }
    }
}
