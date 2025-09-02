#include <assert.h>

#include "object/ntg_label.h"
#include "object/_ntg_label.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"
#include "uconv/uconv.h"

/* -------------------------------------------------------------------------- */

struct ntg_label_opts ntg_label_opts_default()
{
    return (struct ntg_label_opts) {
        .orientation = NTG_ORIENTATION_HORIZONTAL,
        .primary_alignment = NTG_TEXT_ALIGNMENT_1,
        .secondary_aligmnent = NTG_ALIGNMENT_1,
        .indent = 0,
        .wrap_mode = NTG_TEXT_WRAP_NOWRAP
    };
}

static void __init_default_values(ntg_label* label)
{
    label->_text = (struct ntg_str) {0};
    label->_gfx = NT_GFX_DEFAULT;
    label->_opts = ntg_label_opts_default();
}

void __ntg_label_init__(ntg_label* label, ntg_orientation orientation)
{
    assert(label != NULL);

    __ntg_object_init__(NTG_OBJECT(label),
            NTG_OBJECT_WIDGET,
            _ntg_label_measure_fn,
            NULL, NULL,
            _ntg_label_arrange_drawing_fn);

    __init_default_values(label);

    label->_opts.orientation = orientation;
}

void __ntg_label_deinit__(ntg_label* label)
{
    assert(label != NULL);

    __ntg_object_deinit__(NTG_OBJECT(label));

    if(label->_text.data != NULL)
        free(label->_text.data);

    __init_default_values(label);
}

void ntg_label_set_text(ntg_label* label, struct ntg_str_view text)
{
    char* new_text_data = (char*)realloc(label->_text.data, text.len + 1);
    assert(new_text_data != NULL);

    memcpy(new_text_data, text.data, text.len);

    label->_text.data = new_text_data;
    label->_text.len = text.len;
}

void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx)
{
    assert(label != NULL);

    label->_gfx = gfx;
}

void ntg_label_set_opts(ntg_label* label, struct ntg_label_opts opts)
{
    assert(label != NULL);

    label->_opts = opts;
}

static struct ntg_measure_result __measure_nowrap(ntg_label* label,
        ntg_orientation orientation, size_t for_size)
{
    assert(label != NULL);

    if(label->_text.len == 0) return (struct ntg_measure_result) {0};

    struct ntg_str_view _text = {
        .data = label->_text.data,
        .len = label->_text.len
    };
    struct ntg_str_split_result rows = ntg_str_split(_text, '\n');

    if(orientation == label->_opts.orientation)
    {
        size_t i;
        size_t max_row_len = 0;
        size_t it_adj_row_len;
        for(i = 0; i < rows.count; i++)
        {
            it_adj_row_len = rows.views[i].len + label->_opts.indent;
            max_row_len = _max2_size(max_row_len, it_adj_row_len);
        }

        return (struct ntg_measure_result) {
            .min_size = max_row_len,
            .natural_size = max_row_len,
            .max_size = max_row_len
        };
    }
    else
    {
        return (struct ntg_measure_result) {
            .min_size = rows.count,
            .natural_size = rows.count,
            .max_size = rows.count
        };
    }
}

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(_label != NULL);
    ntg_label* label = NTG_LABEL(_label);

    if(label->_text.data == NULL) return (struct ntg_measure_result) {0};
    size_t len = label->_text.len;
    if(len == 0) return (struct ntg_measure_result) {0};

    struct ntg_measure_result result = {0};

    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            result = __measure_nowrap(label, orientation, for_size);
            break;
        case NTG_TEXT_WRAP_WRAP:
            assert(0);
            // result = __measure_wrap();
            break;
        case NTG_TEXT_WRAP_WORD_WRAP:
            assert(0);
            // result = __measure_word_wrap();
            break;
        default: assert(0);
    }

    return result;;
}

/* Ignores orientation */
static void __arrange_content_nowrap(const ntg_label_rows_utf32* rows,
        size_t indent, ntg_text_alignment alignment, ntg_label_content* out_content)
{

    size_t i, j;
    uint32_t *it_char;
    struct ntg_str_utf32* it_row;
    uint32_t* it_content_row;
    for(i = 0; i < rows->_count; i++)
    {
        it_row = &(rows->_data[i]);

        for(j = 0; j < it_row->count; j++)
        {
            assert(ntg_xy_is_greater(out_content->_size, ntg_xy(j, i)));

            it_char = ntg_label_content_at(out_content, ntg_xy(j, i));
            (*it_char) = it_row->data[j];
        }

        it_content_row = ntg_label_content_at(out_content, ntg_xy(0, i));
        ntg_text_row_apply_alignment_and_indent(it_content_row, indent,
                j, out_content->_size.x, alignment);
    }
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);
    ntg_label* label = NTG_LABEL(_label);

    if(label->_text.data == NULL) return;

    struct ntg_str_view view = { 
        .data = label->_text.data,
        .len = label->_text.len
    };
    struct ntg_str_split_result split_res = ntg_str_split(view, '\n');
    if(split_res.views == NULL)
    {
        // TODO
        assert(0);
    }
    ntg_label_rows_utf32 rows; // rows converted to utf32
    __ntg_label_rows_utf32_init__(&rows, split_res.views, split_res.count);
    free(split_res.views);

    struct ntg_xy arrange_size = 
        (label->_opts.orientation == NTG_ORIENTATION_HORIZONTAL) ?
        size : ntg_xy_transpose(size);

    ntg_label_content _content; // content matrix
    __ntg_label_content_init__(&_content, arrange_size);

    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            __arrange_content_nowrap(&rows, label->_opts.indent,
                    label->_opts.primary_alignment, &_content);
            break;
        case NTG_TEXT_WRAP_WRAP:
            assert(0);
            // result = __arrange_drawing_wrap();
            break;
        case NTG_TEXT_WRAP_WORD_WRAP:
            assert(0);
            // result = __arrange_drawing_word_wrap();
            break;
        default: assert(0);
    }

    size_t i, j;
    ntg_cell* it_cell;
    uint32_t* it_char;
    struct ntg_xy it_xy, it_content_xy;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_xy = ntg_xy(j, i);
            it_content_xy = 
            (label->_opts.orientation == NTG_ORIENTATION_HORIZONTAL) ?
            it_xy : ntg_xy_transpose(it_xy);

            it_cell = ntg_object_drawing_at_(out_drawing, it_xy);
            it_char = ntg_label_content_at(&_content, it_content_xy);

            (*it_cell) = ntg_cell_full((*it_char), label->_gfx);
        }
    }

    __ntg_label_rows_utf32_deinit__(&rows);
    __ntg_label_content_deinit__(&_content);
}
