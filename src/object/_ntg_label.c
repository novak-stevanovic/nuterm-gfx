#include "object/_ntg_label.h"
#include "base/ntg_cell.h"
#include <stdlib.h>
#include <uconv/uconv.h>

void __ntg_label_content_init__(ntg_label_content* content,
        struct ntg_xy size)
{
    assert(content != NULL);

    content->_data = NULL;
    content->_size = ntg_xy(0, 0);

    size_t count = size.x * size.y;

    content->_data = (uint32_t*)malloc(sizeof(uint32_t) * count);
    assert(content->_data != NULL);
    size_t i;
    for(i = 0; i < count; i++)
        content->_data[i] = NTG_CELL_EMPTY;

    content->_size = size;
}

void __ntg_label_content_deinit__(ntg_label_content* content)
{
    assert(content != NULL);
    
    if(content->_data != NULL)
        free(content->_data);
    content->_size = ntg_xy(0, 0);
}
