#include <assert.h>

#include "object/ntg_label.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"
#include "uconv/uconv.h"

typedef struct ntg_label_content
{
    uint32_t* _data;
    struct ntg_xy _size;
} ntg_label_content;

static void __ntg_label_content_init__(ntg_label_content* content,
        struct ntg_xy size)
{
    assert(content != NULL);

    size_t count = size.x * size.y;

    content->_data = (uint32_t*)malloc(sizeof(uint32_t) * count);
    assert(content->_data != NULL);
    size_t i;
    for(i = 0; i < size.x * count; i++)
        content->_data[i] = NTG_CELL_EMPTY;

    content->_size = size;
}

static void __ntg_label_content_deinit__(ntg_label_content* content)
{
    assert(content != NULL);
    
    if(content->_data != NULL)
        free(content->_data);
    content->_size = ntg_xy(0, 0);
}

static inline uint32_t* __ntg_label_content_at(ntg_label_content* content,
        struct ntg_xy position)
{
    return &(content->_data[position.y * content->_size.x + position.x]);
}

/* -------------------------------------------------------------------------- */

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

static void __arrange_content_nowrap(ntg_label* label,
        struct ntg_xy size, ntg_label_content* content)
{

    size_t i, j;
    uint32_t* it_char;
    for(i = 0; i < split_res.count; i++)
    {
        for(j = 0; j < split_res.views[i].len; j++)
        {
            it_char = __ntg_label_content_at(content, ntg_xy(j, i));

            (*it_char) = split_res.views[i].data[j];

            ntg_text_row_apply_alignment_and_indent
        }
    }
}

struct label_row_utf32
{
    uint32_t* data;
    size_t count; // codepoint count
};

void __label_row_utf32_init__(struct label_row_utf32

struct label_rows_utf32
{
    struct label_row_utf32* data;
    size_t count; // row count
};

static void __label_rows_utf32_init__(struct label_rows_utf32* rows,
        struct ntg_str_split_result* split_res)
{

    rows->data = (struct label_row_utf32*)malloc(split_res->count *
            sizeof(struct label_row_utf32));
    assert(rows->data != NULL);

    rows->count = split_res->count;

    size_t i;
    for(i = 0; i < split_res->count; i++)
    {
        rows->data[i].data = (uint32_t*)malloc(sizeof(uint32_t) * split_res->data[i].len);
    }
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);
    ntg_label* label = NTG_LABEL(_label);

    ntg_label_content content;
    __ntg_label_content_init__(&content, size);

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

    size_t i;
    for(i = 0; i < split_res.count; i++)
    {
    }

    free(split_res.views);

    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            __arrange_content_nowrap(size, &content);
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

    __ntg_label_content_deinit__(&content);
}
