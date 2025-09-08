#ifndef __NTG_LABEL_H__
#define __NTG_LABEL_H__

#include <stdint.h>
#include <assert.h>

#include "shared/ntg_xy.h"

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

#endif // __NTG_LABEL_H__
