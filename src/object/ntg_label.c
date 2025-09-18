#include <assert.h>
#include <math.h>
#include <uconv/uconv.h>

#include "object/ntg_label.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"

#define DEFAULT_SIZE 5

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

static void __get_wrap_rows_nowrap(struct ntg_str_utf32_view row, size_t for_size,
        struct ntg_str_utf32_view** out_wrap_rows, size_t* out_wrap_row_count);
static void __get_wrap_rows_wrap(struct ntg_str_utf32_view row, size_t for_size,
        struct ntg_str_utf32_view** out_wrap_rows, size_t* out_wrap_row_count);
static void __get_wrap_rows_wwrap(struct ntg_str_utf32_view row, size_t for_size,
        struct ntg_str_utf32_view** out_wwrap_rows, size_t* out_wwrap_row_count);

static struct ntg_measure_result __measure_nowrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        ntg_orientation label_orientation, size_t indent,
        ntg_orientation orientation, size_t for_size);
static struct ntg_measure_result __measure_wrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        ntg_orientation label_orientation, size_t indent,
        ntg_orientation orientation, size_t for_size);
static struct ntg_measure_result __measure_wwrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        ntg_orientation label_orientation, size_t indent,
        ntg_orientation orientation, size_t for_size);

/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_label* label)
{
    label->__orientation = NTG_ORIENTATION_HORIZONTAL;
    label->__text = (struct ntg_str) {0};
    label->__gfx = NT_GFX_DEFAULT;
    label->__primary_alignment = NTG_TEXT_ALIGNMENT_1;
    label->__secondary_alignment = NTG_ALIGNMENT_1;
    label->__wrap_mode = NTG_TEXT_WRAP_NOWRAP;
    label->__indent = 0;
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

    label->__orientation = orientation;
}

void __ntg_label_deinit__(ntg_label* label)
{
    assert(label != NULL);

    __ntg_object_deinit__(NTG_OBJECT(label));

    if(label->__text.data != NULL)
        free(label->__text.data);

    __init_default_values(label);
}

void ntg_label_set_text(ntg_label* label, struct ntg_str_view text)
{
    assert(label != NULL);

    if((text.len == 0) || (text.data == NULL))
    {
        label->__text = (struct ntg_str) {0};
        return;
    }

    char* text_copy = (char*)malloc(text.len);
    assert(text_copy != NULL);

    memmove(text_copy, text.data, text.len);

    label->__text = (struct ntg_str) {
        .data = text_copy,
        .len = text.len
    };
}

void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx)
{
    assert(label != NULL);

    label->__gfx = gfx;
}

void ntg_label_set_primary_alignment(ntg_label* label, ntg_text_alignment alignment)
{
    assert(label != NULL);

    label->__primary_alignment = alignment;
}

void ntg_label_set_secondary_alignment(ntg_label* label, ntg_alignment alignment)
{
    assert(label != NULL);

    label->__secondary_alignment = alignment;
}

void ntg_label_set_wrap_mode(ntg_label* label, ntg_text_wrap_mode wrap_mode)
{
    assert(label != NULL);

    label->__wrap_mode = wrap_mode;
}

void ntg_label_set_indent(ntg_label* label, size_t indent)
{
    assert(label != NULL);

    label->__indent = indent;
}

struct ntg_str_view ntg_label_get_text(const ntg_label* label)
{
    assert(label != NULL);

    if((label->__text.len == 0) || (label->__text.data == NULL))
    {
        return (struct ntg_str_view) {
            .data = "",
            .len = 0
        };
    }
    else
    {
        return (struct ntg_str_view) {
            .data = label->__text.data,
            .len = label->__text.len
        };
    }
}

struct nt_gfx ntg_label_get_gfx(const ntg_label* label)
{
    assert(label != NULL);

    return label->__gfx;
}

ntg_text_alignment ntg_label_get_primary_alignment(const ntg_label* label)
{
    assert(label != NULL);

    return label->__primary_alignment;
}

ntg_alignment ntg_label_get_secondary_alignment(const ntg_label* label)
{
    assert(label != NULL);

    return label->__secondary_alignment;
}

ntg_text_wrap_mode ntg_label_get_wrap_mode(const ntg_label* label)
{
    assert(label != NULL);

    return label->__wrap_mode;
}

size_t ntg_label_get_indent(const ntg_label* label)
{
    assert(label != NULL);

    return label->__indent;
}

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(_label != NULL);
    ntg_label* label = NTG_LABEL(_label);
    
    if(label->__text.len == 0) return (struct ntg_measure_result) {0};

    /* Get UTF-32 text */
    size_t utf32_cap = label->__text.len;
    uint32_t* text_utf32 = (uint32_t*)malloc(sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    uc_status_t _status;
    uc_utf8_to_utf32((uint8_t*)label->__text.data, utf32_cap, text_utf32,
            label->__text.len, 0, &_width, &_status);
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
    switch(label->__wrap_mode)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            result = __measure_nowrap_fn(rows.views, rows.count, 
                    label->__orientation, label->__indent,
                    orientation, for_size);
            break;
        case NTG_TEXT_WRAP_WRAP:
            result = __measure_wrap_fn(rows.views, rows.count,
                    label->__orientation, label->__indent, orientation,
                    for_size);
            break;
        case NTG_TEXT_WRAP_WORD_WRAP:
            result = __measure_wwrap_fn(rows.views, rows.count,
                    label->__orientation, label->__indent, orientation,
                    for_size);
            break;

        default: assert(0);
    }

    /* Clean up */
    free(rows.views);
    free(text_utf32);

    return result;
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);

    ntg_label* label = NTG_LABEL(_label);

    if((label->__text.len == 0) || (label->__text.data == NULL)) return;
    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    /* Init content matrix */
    struct ntg_xy content_size = 
        (label->__orientation == NTG_ORIENTATION_HORIZONTAL) ?
        size :
        ntg_xy_transpose(size);

    label_content _content;
    __label_content_init__(&_content, content_size);

    /* Get UTF-32 text */
    size_t utf32_cap = label->__text.len;
    uint32_t* text_utf32 = (uint32_t*)malloc(sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    uc_status_t _status;
    uc_utf8_to_utf32((uint8_t*)label->__text.data, utf32_cap, text_utf32,
            label->__text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    struct ntg_str_utf32_view view = {
        .data = text_utf32,
        .count = _width
    };

    /* Split by rows */
    struct ntg_str_utf32_split_result rows = ntg_str_utf32_split(view, '\n');
    assert(rows.views != NULL);
    if(rows.count == 0) return;

    size_t capped_indent = _min2_size(label->__indent, content_size.x);

    /* Create content matrix */
    size_t i, j, k;
    size_t content_i = 0, content_j = 0;
    size_t it_row_align_indent, it_row_effective_indent;
    struct ntg_str_utf32_view* _it_wrap_rows;
    size_t _it_wrap_rows_count;
    uint32_t* it_content_cell;
    for(i = 0; i < rows.count; i++)
    {
        _it_wrap_rows = NULL;
        _it_wrap_rows_count = 0;
        switch(label->__wrap_mode)
        {
            case NTG_TEXT_WRAP_NOWRAP:
               __get_wrap_rows_nowrap(rows.views[i], content_size.x, &_it_wrap_rows,
                       &_it_wrap_rows_count); 
                break;
            case NTG_TEXT_WRAP_WRAP:
               __get_wrap_rows_wrap(rows.views[i], content_size.x, &_it_wrap_rows,
                       &_it_wrap_rows_count); 
                break;
            case NTG_TEXT_WRAP_WORD_WRAP:
               __get_wrap_rows_wwrap(rows.views[i], content_size.x, &_it_wrap_rows,
                       &_it_wrap_rows_count); 
                break;
            default: assert(0);
        }

        for(j = 0; j < _it_wrap_rows_count; j++)
        {
            if(content_i >= content_size.y) break;

            /* Avoid overflow in switch statement */
            _it_wrap_rows[j].count = _min2_size(_it_wrap_rows[j].count, content_size.x);

            switch(label->__primary_alignment)
            {
                case NTG_TEXT_ALIGNMENT_1:
                    it_row_align_indent = 0;
                    break;
                case NTG_TEXT_ALIGNMENT_2:
                    it_row_align_indent = (content_size.x -_it_wrap_rows[j].count) / 2;
                    break;
                case NTG_TEXT_ALIGNMENT_3:
                    it_row_align_indent = (content_size.x -_it_wrap_rows[j].count);
                    break;
                case NTG_TEXT_ALIGNMENT_JUSTIFY:
                    assert(0);
                    // effective_indent = 0;
                    // break;
                default: assert(0);
            }

            /* If true row, add capped indent */
            it_row_effective_indent = (j == 0) ?
                _max2_size(capped_indent, it_row_align_indent) :
                it_row_align_indent;
            
            content_j = it_row_effective_indent;
            for(k = 0; k < _it_wrap_rows[j].count; k++)
            {
                if(content_j >= content_size.x) break; // if indent is too big

                it_content_cell = __label_content_at(&_content,
                        ntg_xy(content_j, content_i));

                (*it_content_cell) = _it_wrap_rows[j].data[k]; 

                content_j++;
            }

            content_i++;

        }

        if(_it_wrap_rows != NULL) free(_it_wrap_rows);
    }

    /* Transpose the content matrix if needed */
    ntg_cell* it_cell;
    uint32_t* it_content;
    if(label->__orientation == NTG_ORIENTATION_HORIZONTAL)
    {
        for(i = 0; i < content_size.y; i++)
        {
            for(j = 0; j < content_size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, i));
                it_content = __label_content_at(&_content, ntg_xy(j, i));

                (*it_cell) = ntg_cell_full((*it_content), label->__gfx);
            }
        }
    }
    else // NTG_ORIENTATION_VERTICAL
    {
        for(i = 0; i < content_size.y; i++)
        {
            for(j = 0; j < content_size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(i, j));
                it_content = __label_content_at(&_content, ntg_xy(j, i));

                (*it_cell) = ntg_cell_full((*it_content), label->__gfx);
            }
        }
    }

    /* Clean up */
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
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation, size_t for_size)
{
    assert(rows != NULL);

    if(label_orientation == orientation)
    {
        size_t i;
        size_t max_row_len = 0;
        struct ntg_str_utf32_view* rows = NULL;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count + indent);
        }

        return (struct ntg_measure_result) {
            .min_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .natural_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        return (struct ntg_measure_result) {
            .min_size = _max2_size(rows->count, DEFAULT_SIZE),
            .natural_size = _max2_size(rows->count, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
}

static struct ntg_measure_result __measure_wrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation, size_t for_size)
{
    assert(rows != NULL);

    size_t i;
    if(label_orientation == orientation)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count + indent);
        }

        return (struct ntg_measure_result) {
            .min_size = DEFAULT_SIZE,
            .natural_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        size_t row_counter = 0;
        struct ntg_str_utf32_view* it_row_wrap_rows;
        size_t it_row_wrap_row_count;
        for(i = 0; i < row_count; i++)
        {
            __get_wrap_rows_wrap(rows[i], for_size, &it_row_wrap_rows,
                    &it_row_wrap_row_count);

            row_counter += it_row_wrap_row_count;

            free(it_row_wrap_rows);
        }

        return (struct ntg_measure_result) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .natural_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
}

static struct ntg_measure_result __measure_wwrap_fn(
        const struct ntg_str_utf32_view* rows, size_t row_count,
        ntg_orientation label_orientation, size_t indent,
        ntg_orientation orientation, size_t for_size)
{
    assert(rows != NULL);

    size_t i, j;
    struct ntg_str_utf32_split_result it_words;
    if(label_orientation == orientation)
    {
        size_t max_row_len = 0;
        size_t max_word_len = 0;
        size_t j_word_adj_indent;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count + indent);

            it_words = ntg_str_utf32_split(rows[i], ' ');
            assert(it_words.views != NULL);

            for(j = 0; j < it_words.count; j++)
            {
                // if first word in row, count indent
                j_word_adj_indent = (j == 0) ? indent : 0;

                max_word_len = _max2_size(max_word_len,
                        it_words.views[j].count + j_word_adj_indent);
            }

            free(it_words.views);
            it_words.views = NULL;
            it_words.count = 0;
        }

        return (struct ntg_measure_result) {
            .min_size = _max2_size(max_word_len, DEFAULT_SIZE),
            .natural_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        size_t row_counter = 0;
        struct ntg_str_utf32_view* it_row_wrap_rows;
        size_t it_row_wrap_row_count;
        for(i = 0; i < row_count; i++)
        {
            __get_wrap_rows_wwrap(rows[i], for_size, &it_row_wrap_rows,
                    &it_row_wrap_row_count);

            row_counter += it_row_wrap_row_count;

            free(it_row_wrap_rows);
        }

        return (struct ntg_measure_result) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .natural_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
}

static void __get_wrap_rows_nowrap(struct ntg_str_utf32_view row, size_t for_size,
        struct ntg_str_utf32_view** out_wrap_rows, size_t* out_wrap_row_count)
{
    assert(out_wrap_rows != NULL);
    assert(out_wrap_row_count != NULL);
    assert(for_size != 0);

    if((row.count == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = (struct ntg_str_utf32_view*)malloc(
                sizeof(struct ntg_str_utf32_view));
        (*out_wrap_rows)[0] = (struct ntg_str_utf32_view) {
            .data = row.data,
            .count = 0
        };
        (*out_wrap_row_count) = 1;
        return; 
    }

    (*out_wrap_rows) = (struct ntg_str_utf32_view*)malloc(
            sizeof(struct ntg_str_utf32_view));
    (*out_wrap_rows)[0] = (struct ntg_str_utf32_view) {
        .data = row.data,
        .count = _min2_size(for_size, row.count)
    };
    (*out_wrap_row_count) = 1;
}

static void __get_wrap_rows_wrap(struct ntg_str_utf32_view row, size_t for_size,
        struct ntg_str_utf32_view** out_wrap_rows, size_t* out_wrap_row_count)
{
    assert(out_wrap_rows != NULL);
    assert(out_wrap_row_count != NULL);
    assert(for_size != 0);

    if((row.count == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = (struct ntg_str_utf32_view*)malloc(
                sizeof(struct ntg_str_utf32_view));
        (*out_wrap_rows)[0] = (struct ntg_str_utf32_view) {
            .data = row.data,
            .count = 0
        };
        (*out_wrap_row_count) = 1;
        return; 
    }

    size_t wrap_row_count = ceil((1.0 * row.count) / for_size);
    struct ntg_str_utf32_view* wrap_rows = (struct ntg_str_utf32_view*)malloc
        (wrap_row_count * sizeof(struct ntg_str_utf32_view));
    assert(wrap_rows != NULL);

    size_t i;
    size_t it_start = 0, it_end;
    for(i = 0; i < wrap_row_count; i++)
    {
        it_end = it_start + _min2_size(for_size, row.count - it_start);

        wrap_rows[i] = (struct ntg_str_utf32_view) {
            .data = &(row.data[it_start]),
            .count = it_end - it_start
        };

        it_start = it_end;
    }

    (*out_wrap_rows) = wrap_rows;
    (*out_wrap_row_count) = wrap_row_count;
}

static void __get_wrap_rows_wwrap(struct ntg_str_utf32_view row, size_t for_size,
        struct ntg_str_utf32_view** out_wwrap_rows, size_t* out_wwrap_row_count)
{
    assert(out_wwrap_rows != NULL);
    assert(out_wwrap_row_count != NULL);
    assert(for_size != 0);

    if((row.count == 0) || (row.data == NULL))
    {
        (*out_wwrap_rows) = (struct ntg_str_utf32_view*)malloc(
                sizeof(struct ntg_str_utf32_view));
        (*out_wwrap_rows)[0] = (struct ntg_str_utf32_view) {
            .data = row.data,
            .count = 0
        };
        (*out_wwrap_row_count) = 1;
        return; 
    }

    struct ntg_str_utf32_split_result words = ntg_str_utf32_split(row, ' ');
    size_t wwrap_row_max_count = words.count;

    struct ntg_str_utf32_view* wwrap_rows = (struct ntg_str_utf32_view*)malloc
        (wwrap_row_max_count * sizeof(struct ntg_str_utf32_view));
    assert(wwrap_rows != NULL);
    size_t i;
    struct ntg_str_utf32_view it_word;
    size_t it_row_len = 0;
    size_t it_row_word_count = 0;
    size_t wwrap_row_counter = 0;
    struct ntg_str_utf32_view it_row_start_word = words.views[0];
    struct ntg_str_utf32_view it_row_end_word;
    size_t effective_space;
    for(i = 0; i < words.count; i++)
    {
        effective_space = (it_row_word_count == 0) ? 0 : 1;
        it_word = words.views[i];
        if((it_row_len + it_word.count + effective_space) <= for_size)
        {
            it_row_len += (effective_space + it_word.count);
            it_row_word_count++;

            if(i == (words.count - 1))
            {
                /* process last row */

                it_row_end_word = words.views[i];

                wwrap_rows[wwrap_row_counter] = (struct ntg_str_utf32_view) {
                    .data = it_row_start_word.data,
                    .count = &(it_row_end_word.data[it_row_end_word.count]) -
                        &(it_row_start_word.data[0])
                };

                wwrap_row_counter++;
                // it_row_start_word = words.views[i];
            }
        }
        else
        {
            /* next row */

            if(it_row_word_count > 0)
            {
                it_row_end_word = words.views[i - 1];

                wwrap_rows[wwrap_row_counter] = (struct ntg_str_utf32_view) {
                    .data = it_row_start_word.data,
                    .count = &(it_row_end_word.data[it_row_end_word.count]) -
                        &(it_row_start_word.data[0])
                };

                it_row_start_word = words.views[i];

                wwrap_row_counter++;
            }

            if(it_word.count < for_size) // can fit in next row
            {
                it_row_len = it_word.count;
                it_row_word_count = 1;

                /* process last row */
                if(i == (words.count - 1))
                {
                    it_row_end_word = words.views[i];

                    wwrap_rows[wwrap_row_counter] = (struct ntg_str_utf32_view) {
                        .data = it_row_start_word.data,
                            .count = &(it_row_end_word.data[it_row_end_word.count]) -
                                &(it_row_start_word.data[0])
                    };

                    // it_row_start_word = words.views[i];

                    wwrap_row_counter++;
                }
            }
            else // can't fit in next row(or can, but just right)
            {
                /* next row, again */

                it_row_end_word = words.views[i];

                wwrap_rows[wwrap_row_counter] = (struct ntg_str_utf32_view) {
                    .data = it_row_start_word.data,
                    .count = &(it_row_end_word.data[for_size]) -
                        &(it_row_start_word.data[0])
                };

                if(i < (words.count - 1))
                    it_row_start_word = words.views[i + 1];

                wwrap_row_counter++;

                it_row_len = 0;
                it_row_word_count = 0;
            }
        }
    }

    free(words.views);

    (*out_wwrap_rows) = wwrap_rows;
    (*out_wwrap_row_count) = wwrap_row_counter;
}
