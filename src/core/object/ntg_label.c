#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <shared/_uconv.h>

#include "core/object/ntg_label.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_types.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"
#include "shared/sarena.h"

/* UGLY CODE - TODO: rewrite sometime */

#define DEFAULT_SIZE 5

typedef struct label_content
{
    uint32_t* __data;
    struct ntg_xy _size;
} label_content;

static void _label_content_init_(
        label_content* content,
        struct ntg_xy size,
        sarena* arena);

static inline uint32_t* label_content_at(label_content* content,
        struct ntg_xy position)
{
    return &(content->__data[position.y * content->_size.x + position.x]);
}

static void get_wrap_rows_nowrap(
        struct ntg_strv_utf32 row,
        size_t for_size,
        struct ntg_strv_utf32** out_wrap_rows,
        size_t* out_wrap_row_count,
        sarena* arena);
static void get_wrap_rows_wrap(
        struct ntg_strv_utf32 row,
        size_t for_size,
        struct ntg_strv_utf32** out_wrap_rows,
        size_t* out_wrap_row_count,
        sarena* arena);
static void get_wrap_rows_wwrap(
        struct ntg_strv_utf32 row,
        size_t for_size,
        struct ntg_strv_utf32** out_wwrap_rows,
        size_t* out_wwrap_row_count,
        sarena* arena);

static struct ntg_object_measure measure_nowrap_fn(
        const struct ntg_strv_utf32* rows,
        size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);
static struct ntg_object_measure measure_wrap_fn(
        const struct ntg_strv_utf32* rows,
        size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);
static struct ntg_object_measure measure_wwrap_fn(
        const struct ntg_strv_utf32* rows,
        size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

static void trim_text(ntg_label* label)
{
    assert(label != NULL);
    if((label->__text.len == 0) || (label->__text.data == NULL)) return;

    sarena* arena = sarena_create(10000);
    assert(arena != NULL);

    struct ntg_str_split_out words = ntg_str_split(
            ntg_str_to_view(label->__text), ' ', arena);

    size_t space_needed = 0;

    size_t i;
    for(i = 0; i < words.count; i++)
    {
        if(words.views[i].len > 0)
        {
            memmove(label->__text.data + space_needed,
                    words.views[i].data,
                    words.views[i].len);

            label->__text.data[space_needed + words.views[i].len] = ' ';
            
            space_needed += (1 + words.views[i].len);
        }
    }

    if((space_needed > 0) && (label->__text.data[space_needed - 1] == ' '))
        space_needed--;

    label->__text.data = realloc(label->__text.data, space_needed);
    assert(label->__text.data != NULL);
    label->__text.len = space_needed;

    sarena_destroy(arena);
}

/* -------------------------------------------------------------------------- */

static void init_default_values(ntg_label* label)
{
    label->__text = (struct ntg_str) {0};
    label->__opts = ntg_label_opts_default();
}

struct ntg_label_opts ntg_label_opts_default()
{
    return (struct ntg_label_opts) {
        .orientation = NTG_ORIENTATION_H,
        .gfx = NT_GFX_DEFAULT,
        .palignment = NTG_TEXT_ALIGNMENT_1,
        .salignment = NTG_ALIGNMENT_1,
        .wrap = NTG_TEXT_WRAP_NOWRAP,
        .autotrim = true,
        .indent = false
    };
}

void _ntg_label_init_(
        ntg_label* label,
        struct ntg_label_opts opts,
        struct ntg_object_event_ops event_ops,
        ntg_object_deinit_fn deinit_fn,
        void* data,
        ntg_object_container* container)
{
    assert(label != NULL);

    struct ntg_object_layout_ops layout_ops = {
        .layout_init_fn = NULL,
        .layout_deinit_fn = NULL,
        .measure_fn = _ntg_label_measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = _ntg_label_draw_fn
    };

    _ntg_object_init_((ntg_object*)label,
            NTG_OBJECT_LABEL,
            layout_ops,
            event_ops,
            (deinit_fn != NULL) ? deinit_fn : _ntg_label_deinit_fn,
            data,
            container);

    label->__opts = opts;
}

struct ntg_label_opts ntg_label_get_opts(const ntg_label* label)
{
    assert(label != NULL);

    return label->__opts;
}

void ntg_label_set_opts(ntg_label* label, struct ntg_label_opts opts)
{
    assert(label != NULL);

    label->__opts = opts;
}

void ntg_label_set_text(ntg_label* label, struct ntg_strv text)
{
    assert(label != NULL);

    if((text.len == 0) || (text.data == NULL))
    {
        label->__text = (struct ntg_str) {0};
        return;
    }

    assert(text.len < 1000000);

    char* text_copy = (char*)malloc(text.len);
    assert(text_copy != NULL);

    memmove(text_copy, text.data, text.len);

    label->__text = (struct ntg_str) {
        .data = text_copy,
        .len = text.len
    };

    if(label->__opts.autotrim) trim_text(label);
}

struct ntg_strv ntg_label_get_text(const ntg_label* label)
{
    assert(label != NULL);

    if((label->__text.len == 0) || (label->__text.data == NULL))
    {
        return (struct ntg_strv) {
            .data = "",
            .len = 0
        };
    }
    else
    {
        return (struct ntg_strv) {
            .data = label->__text.data,
            .len = label->__text.len
        };
    }
}

void _ntg_label_deinit_fn(ntg_object* object)
{
    assert(object != NULL);

    ntg_label* label = (ntg_label*)object;

    _ntg_object_deinit_fn((ntg_object*)label);

    if(label->__text.data != NULL)
        free(label->__text.data);

    init_default_values(label);
}

struct ntg_object_measure _ntg_label_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena)
{
    assert(object != NULL);

    const ntg_label* label = (const ntg_label*)object;
    
    if(label->__text.len == 0) return (struct ntg_object_measure) {0};

    /* Get UTF-32 text */
    size_t utf32_cap = label->__text.len;
    uint32_t* text_utf32 = (uint32_t*)sarena_malloc(arena,
            sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    uc_status _status;
    uc_utf8_to_utf32((uint8_t*)label->__text.data, utf32_cap, text_utf32,
            label->__text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    struct ntg_strv_utf32 view = {
        .data = text_utf32,
        .count = _width
    };

    /* Split by rows */
    struct ntg_str_utf32_split_out rows = ntg_str_utf32_split(view, '\n', arena);
    assert(rows.views != NULL);

    if(rows.count == 0) return (struct ntg_object_measure) {0};

    struct ntg_object_measure result;
    switch(label->__opts.wrap)
    {
        case NTG_TEXT_WRAP_NOWRAP:
            result = measure_nowrap_fn(rows.views, rows.count, 
                    label->__opts.orientation, label->__opts.indent,
                    orientation, for_size, arena);
            break;
        case NTG_TEXT_WRAP_WRAP:
            result = measure_wrap_fn(rows.views, rows.count,
                    label->__opts.orientation, label->__opts.indent, orientation,
                    for_size, arena);
            break;
        case NTG_TEXT_WRAP_WORD_WRAP:
            result = measure_wwrap_fn(rows.views, rows.count,
                    label->__opts.orientation, label->__opts.indent, orientation,
                    for_size, arena);
            break;

        default: assert(0);
    }

    /* Clean up */
    // free(rows.views);
    // free(text_utf32);

    return result;
}

void _ntg_label_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena)
{
    assert(object != NULL);

    const ntg_label* label = (const ntg_label*)object;

    if((label->__text.len == 0) || (label->__text.data == NULL)) return;
    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    /* Init content matrix */
    struct ntg_xy content_size = 
        (label->__opts.orientation == NTG_ORIENTATION_H) ?
        size :
        ntg_xy_transpose(size);

    label_content _content;
    _label_content_init_(&_content, content_size, arena);

    /* Get UTF-32 text */
    size_t utf32_cap = label->__text.len;
    uint32_t* text_utf32 = (uint32_t*)sarena_malloc(arena,
            sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    uc_status _status;
    uc_utf8_to_utf32((uint8_t*)label->__text.data, utf32_cap, text_utf32,
            label->__text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    struct ntg_strv_utf32 view = {
        .data = text_utf32,
        .count = _width
    };

    /* Split by rows */
    struct ntg_str_utf32_split_out rows = ntg_str_utf32_split(view, '\n', arena);
    assert(rows.views != NULL);
    if(rows.count == 0) return;

    size_t capped_indent = _min2_size(label->__opts.indent, content_size.x);

    /* Create content matrix */
    size_t i, j, k;
    size_t content_i = 0, content_j = 0;
    /* align variables */
    size_t it_row_align_indent, it_row_effective_indent;
    /* wrap variables */
    struct ntg_strv_utf32* _it_wrap_rows;
    size_t _it_wrap_rows_count;
    uint32_t* it_content_cell;
    /* justify variables */
    size_t it_wrap_row_content_space, it_wrap_row_extra_space,
           it_wrap_row_space_count, it_wrap_row_space_counter;
    for(i = 0; i < rows.count; i++)
    {
        _it_wrap_rows = NULL;
        _it_wrap_rows_count = 0;
        switch(label->__opts.wrap)
        {
            case NTG_TEXT_WRAP_NOWRAP:
               get_wrap_rows_nowrap(rows.views[i], content_size.x, &_it_wrap_rows,
                       &_it_wrap_rows_count, arena); 
                break;
            case NTG_TEXT_WRAP_WRAP:
               get_wrap_rows_wrap(rows.views[i], content_size.x, &_it_wrap_rows,
                       &_it_wrap_rows_count, arena); 
                break;
            case NTG_TEXT_WRAP_WORD_WRAP:
               get_wrap_rows_wwrap(rows.views[i], content_size.x, &_it_wrap_rows,
                       &_it_wrap_rows_count, arena); 
                break;
            default: assert(0);
        }

        for(j = 0; j < _it_wrap_rows_count; j++)
        {
            if(content_i >= content_size.y) break;

            /* Avoid overflow in switch statement */
            _it_wrap_rows[j].count = _min2_size(_it_wrap_rows[j].count, content_size.x);

            switch(label->__opts.palignment)
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
                    it_row_align_indent = 0;
                    break;
                default: assert(0);
            }

            /* If true row, add capped indent */
            it_row_effective_indent = (j == 0) ?
                _max2_size(capped_indent, it_row_align_indent) :
                it_row_align_indent;
            content_j = it_row_effective_indent;

            it_wrap_row_space_counter = 0;
            it_wrap_row_space_count = ntg_str_utf32_count(_it_wrap_rows[j], ' ');
            it_wrap_row_content_space = _it_wrap_rows[j].count + it_row_effective_indent;
            it_wrap_row_extra_space = _ssub_size(content_size.x, it_wrap_row_content_space);
            for(k = 0; k < _it_wrap_rows[j].count; k++)
            {
                if(_it_wrap_rows[j].data[k] == ' ')
                {
                    if(label->__opts.palignment == NTG_TEXT_ALIGNMENT_JUSTIFY)
                    {
                        size_t space_justified_count = (it_wrap_row_extra_space / it_wrap_row_space_count) +
                            (it_wrap_row_space_counter < (it_wrap_row_extra_space % it_wrap_row_space_count)); 

                        content_j += space_justified_count;
                    }
                    it_wrap_row_space_counter++;
                }
                if(content_j >= content_size.x) break; // if indent is too big

                it_content_cell = label_content_at(&_content,
                        ntg_xy(content_j, content_i));

                (*it_content_cell) = _it_wrap_rows[j].data[k]; 

                content_j++;
            }

            content_i++;

        }

        // if(_it_wrap_rows != NULL) free(_it_wrap_rows);
    }

    /* Transpose the content matrix if needed */
    ntg_cell* it_cell;
    uint32_t* it_content;
    if(label->__opts.orientation == NTG_ORIENTATION_H)
    {
        for(i = 0; i < content_size.y; i++)
        {
            for(j = 0; j < content_size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
                it_content = label_content_at(&_content, ntg_xy(j, i));

                (*it_cell) = ntg_cell_full((*it_content), label->__opts.gfx);
            }
        }
    }
    else // NTG_ORIENTATION_VERTICAL
    {
        for(i = 0; i < content_size.y; i++)
        {
            for(j = 0; j < content_size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(i, j));
                it_content = label_content_at(&_content, ntg_xy(j, i));

                (*it_cell) = ntg_cell_full((*it_content), label->__opts.gfx);
            }
        }
    }

    /* Cleanup is not needed due to arena */
}

/* -------------------------------------------------------------------------- */

static void _label_content_init_(
        label_content* content,
        struct ntg_xy size,
        sarena* arena)
{
    assert(content != NULL);

    content->__data = NULL;
    content->_size = ntg_xy(0, 0);

    size_t count = size.x * size.y;

    content->__data = (uint32_t*)sarena_malloc(arena, sizeof(uint32_t) * count);
    assert(content->__data != NULL);
    size_t i;
    for(i = 0; i < count; i++)
        content->__data[i] = NTG_CELL_EMPTY;

    content->_size = size;
}

static struct ntg_object_measure measure_nowrap_fn(
        const struct ntg_strv_utf32* rows,
        size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena)
{
    assert(rows != NULL);

    if(label_orientation == orientation)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count + indent);
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .natural_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        return (struct ntg_object_measure) {
            .min_size = _max2_size(rows->count, DEFAULT_SIZE),
            .natural_size = _max2_size(rows->count, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
}

static struct ntg_object_measure measure_wrap_fn(
        const struct ntg_strv_utf32* rows, size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation, size_t for_size,
        sarena* arena)
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

        return (struct ntg_object_measure) {
            .min_size = DEFAULT_SIZE,
            .natural_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        size_t row_counter = 0;
        struct ntg_strv_utf32* it_row_wrap_rows;
        size_t it_row_wrap_row_count;
        for(i = 0; i < row_count; i++)
        {
            get_wrap_rows_wrap(rows[i], for_size, &it_row_wrap_rows,
                    &it_row_wrap_row_count, arena);

            row_counter += it_row_wrap_row_count;

            // free(it_row_wrap_rows);
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .natural_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
}

static struct ntg_object_measure measure_wwrap_fn(
        const struct ntg_strv_utf32* rows,
        size_t row_count,
        ntg_orientation label_orientation,
        size_t indent,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena)
{
    assert(rows != NULL);

    size_t i, j;
    struct ntg_str_utf32_split_out it_words;
    if(label_orientation == orientation)
    {
        size_t max_row_len = 0;
        size_t max_word_len = 0;
        size_t j_word_adj_indent;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].count == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].count + indent);

            it_words = ntg_str_utf32_split(rows[i], ' ', arena);
            assert(it_words.views != NULL);

            for(j = 0; j < it_words.count; j++)
            {
                // if first word in row, count indent
                j_word_adj_indent = (j == 0) ? indent : 0;

                max_word_len = _max2_size(max_word_len,
                        it_words.views[j].count + j_word_adj_indent);
            }

            // free(it_words.views);
            it_words.views = NULL;
            it_words.count = 0;
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(max_word_len, DEFAULT_SIZE),
            .natural_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
    else
    {
        size_t row_counter = 0;
        struct ntg_strv_utf32* it_row_wrap_rows;
        size_t it_row_wrap_row_count;
        for(i = 0; i < row_count; i++)
        {
            get_wrap_rows_wwrap(rows[i], for_size, &it_row_wrap_rows,
                    &it_row_wrap_row_count, arena);

            row_counter += it_row_wrap_row_count;

            // free(it_row_wrap_rows);
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .natural_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX
        };
    }
}

static void get_wrap_rows_nowrap(
        struct ntg_strv_utf32 row,
        size_t for_size,
        struct ntg_strv_utf32** out_wrap_rows,
        size_t* out_wrap_row_count, sarena* arena)
{
    assert(out_wrap_rows != NULL);
    assert(out_wrap_row_count != NULL);
    assert(for_size != 0);


    if((row.count == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = (struct ntg_strv_utf32*)sarena_malloc(
                arena, sizeof(struct ntg_strv_utf32));
        assert(out_wrap_rows != NULL);
        (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
            .data = row.data,
            .count = 0
        };
        (*out_wrap_row_count) = 1;
        return; 
    }

    (*out_wrap_rows) = (struct ntg_strv_utf32*)sarena_malloc(
            arena, sizeof(struct ntg_strv_utf32));
    assert(out_wrap_rows != NULL);
    (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
        .data = row.data,
        .count = _min2_size(for_size, row.count)
    };
    (*out_wrap_row_count) = 1;
}

static void get_wrap_rows_wrap(
        struct ntg_strv_utf32 row,
        size_t for_size,
        struct ntg_strv_utf32** out_wrap_rows,
        size_t* out_wrap_row_count, sarena* arena)
{
    assert(out_wrap_rows != NULL);
    assert(out_wrap_row_count != NULL);
    assert(for_size != 0);

    if((row.count == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = (struct ntg_strv_utf32*)sarena_malloc(arena,
                sizeof(struct ntg_strv_utf32));
        assert(out_wrap_rows != NULL);
        (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
            .data = row.data,
            .count = 0
        };
        (*out_wrap_row_count) = 1;
        return; 
    }

    size_t wrap_row_count = ceil((1.0 * row.count) / for_size);
    struct ntg_strv_utf32* wrap_rows = (struct ntg_strv_utf32*)sarena_malloc(arena,
            wrap_row_count * sizeof(struct ntg_strv_utf32));
    assert(wrap_rows != NULL);

    size_t i;
    size_t it_start = 0, it_end;
    for(i = 0; i < wrap_row_count; i++)
    {
        it_end = it_start + _min2_size(for_size, row.count - it_start);

        wrap_rows[i] = (struct ntg_strv_utf32) {
            .data = &(row.data[it_start]),
            .count = it_end - it_start
        };

        it_start = it_end;
    }

    (*out_wrap_rows) = wrap_rows;
    (*out_wrap_row_count) = wrap_row_count;
}

static void get_wrap_rows_wwrap(
        struct ntg_strv_utf32 row,
        size_t for_size,
        struct ntg_strv_utf32** out_wwrap_rows,
        size_t* out_wwrap_row_count, sarena* arena)
{
    assert(out_wwrap_rows != NULL);
    assert(out_wwrap_row_count != NULL);
    assert(for_size != 0);

    if((row.count == 0) || (row.data == NULL))
    {
        (*out_wwrap_rows) = (struct ntg_strv_utf32*)sarena_malloc(
                arena, sizeof(struct ntg_strv_utf32));
        assert(out_wwrap_rows != NULL);
        (*out_wwrap_rows)[0] = (struct ntg_strv_utf32) {
            .data = row.data,
            .count = 0
        };
        (*out_wwrap_row_count) = 1;
        return; 
    }

    struct ntg_str_utf32_split_out words = ntg_str_utf32_split(row, ' ', arena);
    size_t wwrap_row_max_count = words.count;

    struct ntg_strv_utf32* wwrap_rows = (struct ntg_strv_utf32*)sarena_malloc(
            arena, wwrap_row_max_count * sizeof(struct ntg_strv_utf32));
    assert(wwrap_rows != NULL);
    size_t i;
    struct ntg_strv_utf32 it_word;
    size_t it_row_len = 0;
    size_t it_row_word_count = 0;
    size_t wwrap_row_counter = 0;
    struct ntg_strv_utf32 it_row_start_word = words.views[0];
    struct ntg_strv_utf32 it_row_end_word;
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

                wwrap_rows[wwrap_row_counter] = (struct ntg_strv_utf32) {
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

                wwrap_rows[wwrap_row_counter] = (struct ntg_strv_utf32) {
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

                    wwrap_rows[wwrap_row_counter] = (struct ntg_strv_utf32) {
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

                wwrap_rows[wwrap_row_counter] = (struct ntg_strv_utf32) {
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

    // free(words.views);

    (*out_wwrap_rows) = wwrap_rows;
    (*out_wwrap_row_count) = wwrap_row_counter;
}
