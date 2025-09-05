#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "shared/ntg_string.h"

size_t _ntg_str_count(const char* str, size_t len, const char* sep, size_t data_size)
{
    assert(str != NULL);
<<<<<<< HEAD
    assert(sep != NULL);
    assert(data_size > 0);

    if(len == 0) return 0;
=======
    assert(len > 0);
    assert(sep != NULL);
    assert(data_size > 0);
>>>>>>> b6d69658450d94b3a0d4bdd0f1b136e41c7ae630

    size_t i;
    const char* it_char;
    size_t sum = 0;
    for(i = 0; i < len; i++)
    {
        it_char = str + (i * data_size);

<<<<<<< HEAD
        if(memcmp(it_char, sep, data_size) == 0)
            sum++;
=======
        if(memcmp(it_char, sep, data_size) == 0) sum++;
>>>>>>> b6d69658450d94b3a0d4bdd0f1b136e41c7ae630
    }

    return sum;
}

<<<<<<< HEAD
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
=======
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
>>>>>>> b6d69658450d94b3a0d4bdd0f1b136e41c7ae630
}
