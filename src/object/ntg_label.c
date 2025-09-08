#include <assert.h>
#include <math.h>
#include <uconv/uconv.h>

#include "object/ntg_label.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"

static struct ntg_measure_result __measure_nowrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        struct ntg_label_opts label_opts,
        ntg_orientation orientation, size_t for_size);
static struct ntg_measure_result __measure_wrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        struct ntg_label_opts label_opts,
        ntg_orientation orientation, size_t for_size);
static struct ntg_measure_result __measure_wwrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        struct ntg_label_opts label_opts,
        ntg_orientation orientation, size_t for_size);

/* -------------------------------------------------------------------------- */

struct ntg_label_opts ntg_label_opts_default()
{
    return (struct ntg_label_opts) {
        .orientation = NTG_ORIENTATION_HORIZONTAL,
        .primary_alignment = NTG_TEXT_ALIGNMENT_1,
        .secondary_alignment = NTG_ALIGNMENT_1,
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

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(_label != NULL);
    ntg_label* label = NTG_LABEL(_label);
    
    if(label->_text.len == 0) return (struct ntg_measure_result) {0};

    size_t _width;
    uc_status_t _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, label->_text.len, NULL,
            0, 0, &_width, &_status);

    assert(_status == UC_SUCCESS);
    if(_width == 0) return (struct ntg_measure_result) {0};

    uint32_t* text_utf32 = (uint32_t*)malloc(sizeof(uint32_t) * _width);
    assert(text_utf32 != NULL);

    uc_utf8_to_utf32((uint8_t*)label->_text.data, label->_text.len, text_utf32,
            _width, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    struct ntg_str_utf32_view view = {
        .data = text_utf32,
        .count = _width
    };

    struct ntg_str_utf32_split_result rows = ntg_str_utf32_split(view, '\n');
    assert(rows.views != NULL);

    if(rows.count == 0) return (struct ntg_measure_result) {0};

    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            return __measure_nowrap_fn(rows.views, rows.count, label->_opts,
                    orientation, for_size);
        case NTG_TEXT_WRAP_WRAP:
            return __measure_wrap_fn(rows.views, rows.count, label->_opts,
                    orientation, for_size);
        case NTG_TEXT_WRAP_WORD_WRAP:
            return __measure_wwrap_fn(rows.views, rows.count, label->_opts,
                    orientation, for_size);
        default: assert(0);
    }

    free(rows.views);
    rows.views = NULL;
    rows.count = 0;
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);

    if(ntg_xy_are_equal(ntg_xy_size(size), ntg_xy(0, 0))) return;

    assert(0);
}

/* -------------------------------------------------------------------------- */

static struct ntg_measure_result __measure_nowrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        struct ntg_label_opts label_opts,
        ntg_orientation orientation, size_t for_size)
{
    assert(rows != NULL);
    assert(label_opts.wrap_mode == NTG_TEXT_WRAP_NOWRAP);

    if(label_opts.orientation == orientation)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count +
                    label_opts.indent);
        }

        return (struct ntg_measure_result) {
            .min_size = max_row_len,
            .natural_size = max_row_len,
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        return (struct ntg_measure_result) {
            .min_size = rows->count,
            .natural_size = rows->count,
            .max_size = NTG_SIZE_MAX
        };
    }
}

static struct ntg_measure_result __measure_wrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        struct ntg_label_opts label_opts,
        ntg_orientation orientation, size_t for_size)
{
    assert(rows != NULL);
    assert(label_opts.wrap_mode == NTG_TEXT_WRAP_WRAP);

    size_t i;
    if(label_opts.orientation == orientation)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count +
                    label_opts.indent);
        }

        return (struct ntg_measure_result) {
            .min_size = 1,
            .natural_size = max_row_len,
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        size_t row_counter = 0;
        size_t it_wrap_rows_needed_for_row;
        for(i = 0; i < row_count; i++)
        {
            it_wrap_rows_needed_for_row = ceil(
                    (1.0 * (rows[i].count + label_opts.indent)) /
                    for_size);

            row_counter += it_wrap_rows_needed_for_row;
        }

        return (struct ntg_measure_result) {
            .min_size = row_counter,
            .natural_size = row_counter,
            .max_size = NTG_SIZE_MAX
        };
    }
}

static struct ntg_measure_result __measure_wwrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        struct ntg_label_opts label_opts,
        ntg_orientation orientation, size_t for_size)
{
    assert(rows != NULL);
    assert(label_opts.wrap_mode == NTG_TEXT_WRAP_WORD_WRAP);

    size_t i, j;
    struct ntg_str_utf32_split_result it_words;
    if(label_opts.orientation == orientation)
    {
        size_t max_row_len = 0;
        size_t max_word_len = 0;
        size_t j_word_adj_indent;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count +
                    label_opts.indent);

            it_words = ntg_str_utf32_split(rows[i], ' ');
            assert(it_words.views != NULL);

            for(j = 0; j < it_words.count; j++)
            {
                // if first word in row, count indent
                j_word_adj_indent = (j == 0) ? label_opts.indent : 0;

                max_word_len = _max2_size(max_word_len,
                        it_words.views[i].count + j_word_adj_indent);
            }

            free(it_words.views);
            it_words.views = NULL;
            it_words.count = 0;
        }

        return (struct ntg_measure_result) {
            .min_size = max_word_len,
            .natural_size = max_row_len,
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        assert(0);
    }
}
