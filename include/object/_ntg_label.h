#ifndef __NTG_LABEL_H__
#define __NTG_LABEL_H__

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "shared/ntg_xy.h"
#include "shared/ntg_string.h"

typedef struct ntg_label_content
{
    uint32_t* _data;
    struct ntg_xy _size;
} ntg_label_content;

void __ntg_label_content_init__(ntg_label_content* content,
        struct ntg_xy size);

void __ntg_label_content_deinit__(ntg_label_content* content);

static inline uint32_t* ntg_label_content_at(ntg_label_content* content,
        struct ntg_xy position)
{
    return &(content->_data[position.y * content->_size.x + position.x]);
}

typedef struct ntg_label_rows_utf32
{
    struct ntg_str_utf32* _data;
    size_t _count; // row count
} ntg_label_rows_utf32;

void __ntg_label_rows_utf32_init__(ntg_label_rows_utf32* rows_utf32,
        struct ntg_str_view* rows_utf8, size_t count);
void __ntg_label_rows_utf32_deinit__(ntg_label_rows_utf32* rows_utf32);

#endif // __NTG_LABEL_H__
