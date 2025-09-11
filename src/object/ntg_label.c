#include <assert.h>
#include <math.h>
#include <uconv/uconv.h>

#include "object/ntg_label.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"

typedef struct label_content
{
    uint32_t* __data;
    struct ntg_xy _size;
} label_content;

static void __label_content_init__(label_content* content,
        struct ntg_xy size);
static void __label_content_deinit__(label_content* content);

static inline uint32_t* __label_content_at(label_content* content,
        struct ntg_xy position)
{
    return &(content->__data[position.y * content->_size.x + position.x]);
}

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

static void __arrange_content_nowrap_fn(const struct ntg_str_utf32_view* rows,
        size_t row_count, ntg_text_alignment primary_alignment, size_t indent,
        label_content* out_content);
static void __arrange_content_wrap_fn(const struct ntg_str_utf32_view* rows,
        size_t row_count, ntg_text_alignment primary_alignment, size_t indent,
        label_content* out_content);
static void __arrange_content_wwrap_fn(const struct ntg_str_utf32_view* rows,
        size_t row_count, ntg_text_alignment primary_alignment, size_t indent,
        label_content* out_content);

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

    /* Get UTF-32 text */
    size_t utf32_cap = label->_text.len;
    uint32_t* text_utf32 = (uint32_t*)malloc(sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    uc_status_t _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, utf32_cap, text_utf32,
            label->_text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    struct ntg_str_utf32_view view = {
        .data = text_utf32,
        .count = _width
    };

    /* Split by rows */
    struct ntg_str_utf32_split_result rows = ntg_str_utf32_split(view, '\n');
    assert(rows.views != NULL);

    if(rows.count == 0) return (struct ntg_measure_result) {0};

    struct ntg_measure_result result;
    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            result = __measure_nowrap_fn(rows.views, rows.count, label->_opts,
                    orientation, for_size);
            break;
        case NTG_TEXT_WRAP_WRAP:
            result = __measure_wrap_fn(rows.views, rows.count, label->_opts,
                    orientation, for_size);
            break;
        case NTG_TEXT_WRAP_WORD_WRAP:
            result = __measure_wwrap_fn(rows.views, rows.count, label->_opts,
                    orientation, for_size);
            break;

        default: assert(0);
    }

    free(rows.views);
    free(text_utf32);

    return result;
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);

    ntg_label* label = NTG_LABEL(_label);

    if((label->_text.len == 0) || (label->_text.data == NULL)) return;
    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    /* Init content matrix */
    label_content _content;
    __label_content_init__(&_content, size);

    /* Get UTF-32 text */
    size_t utf32_cap = label->_text.len;
    uint32_t* text_utf32 = (uint32_t*)malloc(sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    uc_status_t _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, utf32_cap, text_utf32,
            label->_text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    struct ntg_str_utf32_view view = {
        .data = text_utf32,
        .count = _width
    };

    /* Split by rows */
    struct ntg_str_utf32_split_result rows = ntg_str_utf32_split(view, '\n');
    assert(rows.views != NULL);
    if(rows.count == 0) return;

    switch(label->_opts.wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            __arrange_content_nowrap_fn(rows.views, rows.count,
                    label->_opts.primary_alignment,
                    label->_opts.indent, &_content);
            break;
        case NTG_TEXT_WRAP_WRAP:
            __arrange_content_wrap_fn(rows.views, rows.count,
                    label->_opts.primary_alignment,
                    label->_opts.indent, &_content);
            break;
        case NTG_TEXT_WRAP_WORD_WRAP:
            __arrange_content_wwrap_fn(rows.views, rows.count,
                    label->_opts.primary_alignment,
                    label->_opts.indent, &_content);
            break;
    }

    size_t i, j;
    ntg_cell* it_cell;
    uint32_t* it_content;
    if(label->_opts.orientation == NTG_ORIENTATION_HORIZONTAL)
    {
        for(i = 0; i < size.y; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, i));
                it_content = __label_content_at(&_content, ntg_xy(j, i));

                (*it_cell) = ntg_cell_full((*it_content), label->_gfx);
            }
        }
    }
    else // NTG_ORIENTATION_VERTICAL
    {
        assert(0);
    }

    __label_content_deinit__(&_content);
    free(text_utf32);
    free(rows.views);
}

/* -------------------------------------------------------------------------- */

static void __label_content_init__(label_content* content,
        struct ntg_xy size)
{
    assert(content != NULL);

    content->__data = NULL;
    content->_size = ntg_xy(0, 0);

    size_t count = size.x * size.y;

    content->__data = (uint32_t*)malloc(sizeof(uint32_t) * count);
    assert(content->__data != NULL);
    size_t i;
    for(i = 0; i < count; i++)
        content->__data[i] = NTG_CELL_EMPTY;

    content->_size = size;
}

static void __label_content_deinit__(label_content* content)
{
    assert(content != NULL);
    
    if(content->__data != NULL)
        free(content->__data);
    content->_size = ntg_xy(0, 0);
}

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

static size_t __wrap_rows_for_row(struct ntg_str_utf32_view row, size_t indent,
        size_t for_size)
{
    return ceil((1.0 * (row.count + indent)) / for_size);
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
        for(i = 0; i < row_count; i++)
        {
            row_counter += __wrap_rows_for_row(rows[i], label_opts.indent,
                    for_size);
        }

        return (struct ntg_measure_result) {
            .min_size = row_counter,
            .natural_size = row_counter,
            .max_size = NTG_SIZE_MAX
        };
    }
}

static size_t __wwrap_rows_for_row(struct ntg_str_utf32_view row, size_t indent,
        size_t for_size)
{
    assert(for_size != 0);

    if(row.count == 0) return 1;

    struct ntg_str_utf32_split_result words = ntg_str_utf32_split(row, ' ');
    assert(words.views != NULL);

    size_t wwrap_rows = 0;
    size_t it_wwrap_row_len = 0;
    size_t it_effective_indent;
    size_t it_effective_space;
    struct ntg_str_utf32_view* it_word;

    size_t i;
    for(i = 0; i < words.count; i++)
    {
        it_word = &(words.views[i]);
        it_effective_indent = (i == 0) ? indent : 0;
        it_effective_space = (i == 0) ? 0 : 1;

        if((it_wwrap_row_len + it_word->count + it_effective_indent +
                    it_effective_space) <= for_size)
        {
            it_wwrap_row_len += it_word->count + it_effective_indent +
                it_effective_space;

            if(i == (words.count - 1)) wwrap_rows++;
        }
        else
        {
            wwrap_rows++;
            if(it_word->count + it_effective_indent < for_size)
            {
                it_wwrap_row_len = it_word->count + it_effective_indent;
            }
            else
            {
                wwrap_rows++;
                it_wwrap_row_len = 0;
            }
        }
    }

    free(words.views);
    words.views = NULL;
    words.count = 0;

    return wwrap_rows;
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
                        it_words.views[j].count + j_word_adj_indent);
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
        size_t row_counter = 0;
        for(i = 0; i < row_count; i++)
        {
            row_counter += __wwrap_rows_for_row(rows[i], label_opts.indent,
                    for_size);
        }

        return (struct ntg_measure_result) {
            .min_size = row_counter,
            .natural_size = row_counter,
            .max_size = NTG_SIZE_MAX
        };
    }
}

static void __arrange_content_nowrap_fn(const struct ntg_str_utf32_view* rows,
        size_t row_count, ntg_text_alignment primary_alignment, size_t indent,
        label_content* out_content)
{
    assert(rows != NULL);
    assert(row_count > 0);
    assert(out_content != NULL);

    struct ntg_xy content_size = out_content->_size;
    indent = _min2_size(indent, content_size.x);

    size_t i, j;
    size_t content_i = 0, content_j = indent;
    struct ntg_str_utf32_view it_row;
    uint32_t it_char;
    uint32_t* it_content_char;

    for(i = 0; i < row_count; i++)
    {
        if(i >= content_size.y) break;
        if(content_i >= content_size.y) break;

        it_row = rows[i];

        for(j = 0; j < it_row.count; j++)
        {
            if(j >= content_size.x) break;
            if(content_j >= content_size.x) break;

            it_char = it_row.data[j];
            it_content_char = __label_content_at(out_content,
                    ntg_xy(content_j, content_i));

            if(it_char == '\n')
            {
                content_i++;
                content_j = indent;
            }
            else
            {
               (*it_content_char) = it_char; 
               content_j++;
            }
        }
        content_i++;
        content_j = indent;
    }
}

static void __arrange_content_wrap_fn(const struct ntg_str_utf32_view* rows,
        size_t row_count, ntg_text_alignment primary_alignment, size_t indent,
        label_content* out_content)
{
    assert(rows != NULL);
    assert(row_count > 0);
    assert(out_content != NULL);

    assert(0);
}

static void __arrange_content_wwrap_fn(const struct ntg_str_utf32_view* rows,
        size_t row_count, ntg_text_alignment primary_alignment, size_t indent,
        label_content* out_content)
{
    assert(rows != NULL);
    assert(row_count > 0);
    assert(out_content != NULL);

    assert(0);
}
