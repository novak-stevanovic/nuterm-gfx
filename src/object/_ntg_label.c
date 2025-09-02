#include "object/_ntg_label.h"
#include "base/ntg_cell.h"
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

void __ntg_label_rows_utf32_init__(ntg_label_rows_utf32* rows_utf32,
        struct ntg_str_view* rows_utf8, size_t count)
{
    assert(rows_utf32 != NULL);
    assert(rows_utf8 != NULL);

    if(count == 0) rows_utf32->_data = NULL;
    else
    {
        rows_utf32->_data = (struct ntg_str_utf32*)malloc(count *
                sizeof(struct ntg_str_utf32));
        assert(rows_utf32->_data != NULL);

        uint32_t* _utf32_buff;
        size_t cap;
        size_t _width;
        size_t i;
        uc_status_t _status;
        for(i = 0; i < count; i++)
        {
            cap = rows_utf8[i].len;

            _utf32_buff = malloc(cap * sizeof(uint32_t));
            assert(_utf32_buff != NULL);

            uc_utf8_to_utf32((uint8_t*)rows_utf8[i].data, rows_utf8[i].len,
                    _utf32_buff, cap, 0, &_width, &_status);
            assert(_status == UC_SUCCESS);

            rows_utf32->_data[i] = (struct ntg_str_utf32) {
                .data = _utf32_buff,
                .count = _width
            };
        }
    }

    rows_utf32->_count = count;
}

void __ntg_label_rows_utf32_deinit__(ntg_label_rows_utf32* rows_utf32)
{
    assert(rows_utf32 != NULL);

    if(rows_utf32->_data != NULL)
    {
        size_t i;

        struct ntg_str_utf32* it_str;
        for(i = 0; i < rows_utf32->_count; i++)
        {
            it_str = &(rows_utf32->_data[i]);

            if(it_str->data != NULL)
                free(it_str->data);

            it_str->count = 0;
        }
    }

    rows_utf32->_count = 0;
}
