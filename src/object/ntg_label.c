#include "object/ntg_label.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"
#include <assert.h>

struct ntg_label_opts ntg_label_opts_default()
{
    return (struct ntg_label_opts) {
        .orientation = NTG_ORIENTATION_HORIZONTAL,
        .alignment = NTG_TEXT_ALIGNMENT_1,
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

static void __arrange_drawing_nowrap(ntg_label* label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    struct ntg_oxy _size = ntg_oxy_from_xy(size, label->_opts.orientation);

    size_t i, j;
    size_t k = 0; // text str iterator
    char it_char;
    ntg_cell* it_cell;
    for(i = 0; i < _size.prim_val; i++)
    {
        for(j = 0; j < _size.sec_val; j++)
        {
            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, i));
            it_char = label->_text.data[k];

            if(k < label->_text.len) // still chars left to read
            {
                if(it_char == '\n')
                {
                    size_t l;
                    for(l = j; l < _size.sec_val; l++) // fill the row with spaces
                    {
                        (*it_cell) = ntg_cell_bg(label->_gfx.bg);
                        break;
                    }
                }
                else
                    (*it_cell) = ntg_cell_full(it_char, label->_gfx);

                k++;
            }
            else
                (*it_cell) = ntg_cell_bg(label->_gfx.bg);
        }
    }
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);
    ntg_label* label = NTG_LABEL(_label);

    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            __arrange_drawing_nowrap(label, size, out_drawing);
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
}
