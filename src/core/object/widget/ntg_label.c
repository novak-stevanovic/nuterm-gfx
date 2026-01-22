#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

/* UGLY CODE - TODO: rewrite sometime */

#define DEFAULT_SIZE 1

static struct ntg_object_measure 
measure_nowrap_fn(const ntg_label* label,
                  const struct ntg_strv_utf32* rows,
                  size_t row_count, ntg_orient orient,
                  size_t for_size, sarena* arena);
static struct ntg_object_measure 
measure_wrap_fn(const ntg_label* label,
                const struct ntg_strv_utf32* rows,
                size_t row_count, ntg_orient orient,
                size_t for_size, sarena* arena);
static struct ntg_object_measure 
measure_wwrap_fn(const ntg_label* label,
                 const struct ntg_strv_utf32* rows,
                 size_t row_count, ntg_orient orient,
                 size_t for_size, sarena* arena);

static size_t 
get_wrap_rows_nowrap(const struct ntg_strv_utf32 row,
                     size_t for_size,
                     struct ntg_strv_utf32** out_wrap_rows,
                     sarena* arena);
static size_t 
get_wrap_rows_wrap(const struct ntg_strv_utf32 row,
                   size_t for_size,
                   struct ntg_strv_utf32** out_wrap_rows,
                   sarena* arena);
static size_t 
get_wrap_rows_wwrap(const struct ntg_strv_utf32 row,
                    size_t for_size,
                    struct ntg_strv_utf32** out_wwrap_rows,
                    sarena* arena);

static void trim_text(struct ntg_str* text);

/* -------------------------------------------------------------------------- */

static void init_default(ntg_label* label)
{
    label->_text = (struct ntg_str) {0};
    label->_opts = ntg_label_opts_def();
    label->__utf32_text = (struct ntg_str_utf32) {0};
    label->__utf32_row_count = 0;
    label->__utf32_rows = NULL;
}

struct ntg_label_opts ntg_label_opts_def()
{
    return (struct ntg_label_opts) {
        .orient = NTG_ORIENT_H,
        .gfx = NT_GFX_DEFAULT,
        .palign = NTG_LABEL_ALIGN_1,
        .salign = NTG_ALIGN_1,
        .wrap = NTG_LABEL_WRAP_NONE,
        .autotrim = true,
        .indent = false
    };
}

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_label* ntg_label_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_LABEL,
        .deinit_fn = (ntg_entity_deinit_fn)ntg_label_deinit,
        .system = system
    };

    ntg_label* new = (ntg_label*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_label_init(ntg_label* label)
{
    assert(label != NULL);

    struct ntg_widget_layout_ops widget_data = {
        .measure_fn = _ntg_label_measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = _ntg_label_draw_fn,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_widget_hooks hooks = {0};

    ntg_widget_init((ntg_widget*)label, widget_data, hooks);

    init_default(label);

    ntg_label_set_text(label, ntg_strv_cstr(""));
}

struct ntg_label_opts ntg_label_get_opts(const ntg_label* label)
{
    assert(label != NULL);

    return label->_opts;
}

void ntg_label_set_opts(ntg_label* label, struct ntg_label_opts opts)
{
    assert(label != NULL);

    label->_opts = opts;
    ntg_widget_set_background((ntg_widget*)label, ntg_vcell_bg(opts.gfx.bg));

    ntg_entity_raise_event_((ntg_entity*)label, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_label_set_text(ntg_label* label, struct ntg_strv text)
{
    assert(label != NULL);

    text.len = _min2_size(text.len, (NTG_SIZE_MAX * NTG_SIZE_MAX));

    char* new_text = realloc(label->_text.data, text.len + 1);
    assert(new_text != NULL);
    label->_text.data = new_text;
    label->_text.len = text.len;
    label->_text.data[label->_text.len] = 0;

    memmove(label->_text.data, text.data, text.len);

    if(label->_opts.autotrim) trim_text(&label->_text);

    if(label->_text.len == 0)
    {
        if(label->__utf32_text.data)
            free(label->__utf32_text.data);
        if(label->__utf32_rows)
            free(label->__utf32_rows);

        label->__utf32_text.data = NULL;
        label->__utf32_text.len = 0;
        label->__utf32_rows = NULL;

        return;
    }

    // UTF-32 cache

    size_t utf32_cap = label->_text.len;
    uint32_t* new_utf32_text = realloc(label->__utf32_text.data,
                                       sizeof(uint32_t) * utf32_cap);
    assert(new_utf32_text != NULL);

    size_t _width;
    int _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, utf32_cap, new_utf32_text,
                     label->_text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    label->__utf32_text.data = new_utf32_text;
    label->__utf32_text.len = _width;
    uint32_t* shrunk_utf32_text = realloc(label->__utf32_text.data,
                                          sizeof(uint32_t) * _width);
    assert(shrunk_utf32_text != NULL);

    label->__utf32_text.data = shrunk_utf32_text;

    struct ntg_strv_utf32 view = ntg_strv_str_utf32(label->__utf32_text);
    size_t row_count = ntg_str_utf32_count(view, '\n') + 1;
    struct ntg_strv_utf32* new_rows = realloc(label->__utf32_rows, row_count *
                                              sizeof(struct ntg_strv_utf32));
    assert(new_rows != NULL);
    ntg_str_utf32_split(view, '\n', new_rows, row_count);

    label->__utf32_rows = new_rows;
    label->__utf32_row_count = row_count;

    ntg_entity_raise_event_((ntg_entity*)label, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_label_deinit(ntg_label* label)
{
    if(label->_text.data != NULL)
        free(label->_text.data);

    if(label->__utf32_text.data)
        free(label->__utf32_text.data);

    if(label->__utf32_rows)
        free(label->__utf32_rows);

    init_default(label);

    ntg_widget_deinit((ntg_widget*)label);
}

struct ntg_object_measure _ntg_label_measure_fn(
        const ntg_widget* _label,
        void* _layout_data,
        ntg_orient orient,
        bool constrained,
        sarena* arena)
{
    const ntg_label* label = (const ntg_label*)_label;
    size_t for_size = ntg_widget_get_cont_for_size(_label, orient, constrained);
    
    if(label->_text.len == 0) return (struct ntg_object_measure) {0};

    /* Get UTF-32 text */
    size_t utf32_cap = label->_text.len;
    uint32_t* text_utf32 = sarena_malloc(arena, sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    int _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, utf32_cap, text_utf32,
                     label->_text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    size_t row_count = label->__utf32_row_count;
    struct ntg_strv_utf32* rows = label->__utf32_rows;
    if(row_count == 0) return (struct ntg_object_measure) {0};

    struct ntg_object_measure result;
    switch(label->_opts.wrap)
    {
        case NTG_LABEL_WRAP_NONE:
            result = measure_nowrap_fn(label, rows, row_count, orient,
                                       for_size, arena);
            break;
        case NTG_LABEL_WRAP_CHAR:
            result = measure_wrap_fn(label, rows, row_count, orient,
                                     for_size, arena);
            break;
        case NTG_LABEL_WRAP_WORD:
            result = measure_wwrap_fn(label, rows, row_count, orient,
                                      for_size, arena);
            break;

        default: assert(0);
    }

    return result;
}

void _ntg_label_draw_fn(
        const ntg_widget* _label,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena)
{
    const ntg_label* label = (const ntg_label*)_label;
    struct ntg_xy size = ntg_widget_get_cont_size(_label);

    if((label->_text.len == 0) || (label->_text.data == NULL)) return;
    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    /* Init cont matrix */
    struct ntg_xy cont_size = 
        (label->_opts.orient == NTG_ORIENT_H) ?
        size :
        ntg_xy_transpose(size);

    size_t i, j, k;
    size_t cont_size_prod = cont_size.x * cont_size.y;
    uint32_t* cont_buff = sarena_malloc(arena, sizeof(uint32_t) * cont_size_prod);
    for(i = 0; i < cont_size_prod; i++) cont_buff[i] = NTG_CELL_EMPTY;

    /* Get UTF-32 text */
    size_t utf32_cap = label->_text.len;
    uint32_t* text_utf32 = sarena_malloc(arena, sizeof(uint32_t) * utf32_cap);
    assert(text_utf32 != NULL);

    size_t _width;
    int _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, utf32_cap, text_utf32,
            label->_text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    size_t row_count = label->__utf32_row_count;
    struct ntg_strv_utf32* rows = label->__utf32_rows;

    size_t capped_indent = _min2_size(label->_opts.indent, cont_size.x);

    /* Create cont matrix */
    size_t cont_i = 0, cont_j = 0;
    /* align variables */
    size_t it_row_align_indent, it_row_effective_indent;
    /* wrap variables */
    struct ntg_strv_utf32* _it_wrap_rows;
    size_t _it_wrap_rows_count;
    uint32_t* it_cont;
    /* justify variables */
    size_t it_wrap_row_cont_space, it_wrap_row_extra_space,
           it_wrap_row_space_count, it_wrap_row_space_counter;
    for(i = 0; i < row_count; i++)
    {
        _it_wrap_rows = NULL;
        _it_wrap_rows_count = 0;
        switch(label->_opts.wrap)
        {
            case NTG_LABEL_WRAP_NONE:
               _it_wrap_rows_count = get_wrap_rows_nowrap(rows[i], cont_size.x,
                                                          &_it_wrap_rows, arena); 
                break;
            case NTG_LABEL_WRAP_CHAR:
               _it_wrap_rows_count = get_wrap_rows_wrap(rows[i], cont_size.x,
                                                        &_it_wrap_rows, arena); 
                break;
            case NTG_LABEL_WRAP_WORD:
               _it_wrap_rows_count = get_wrap_rows_wwrap(rows[i], cont_size.x,
                                                         &_it_wrap_rows, arena); 
                break;
            default: assert(0);
        }

        for(j = 0; j < _it_wrap_rows_count; j++)
        {
            if(cont_i >= cont_size.y) break;

            /* Avoid overflow in switch statement */
            _it_wrap_rows[j].len = _min2_size(_it_wrap_rows[j].len, cont_size.x);

            switch(label->_opts.palign)
            {
                case NTG_LABEL_ALIGN_1:
                    it_row_align_indent = 0;
                    break;
                case NTG_LABEL_ALIGN_2:
                    it_row_align_indent = (cont_size.x -_it_wrap_rows[j].len) / 2;
                    break;
                case NTG_LABEL_ALIGN_3:
                    it_row_align_indent = (cont_size.x -_it_wrap_rows[j].len);
                    break;
                case NTG_LABEL_ALIGN_JUSTIFY:
                    it_row_align_indent = 0;
                    break;
                default: assert(0);
            }

            /* If true row, add capped indent */
            it_row_effective_indent = (j == 0) ?
                _max2_size(capped_indent, it_row_align_indent) :
                it_row_align_indent;
            cont_j = it_row_effective_indent;

            it_wrap_row_space_counter = 0;
            it_wrap_row_space_count = ntg_str_utf32_count(_it_wrap_rows[j], ' ');
            it_wrap_row_cont_space = _it_wrap_rows[j].len + it_row_effective_indent;
            it_wrap_row_extra_space = _ssub_size(cont_size.x, it_wrap_row_cont_space);
            for(k = 0; k < _it_wrap_rows[j].len; k++)
            {
                if(_it_wrap_rows[j].data[k] == ' ')
                {
                    if((j < (_it_wrap_rows_count - 1)) && label->_opts.palign == NTG_LABEL_ALIGN_JUSTIFY)
                    {
                        size_t space_justified_count = (it_wrap_row_extra_space / it_wrap_row_space_count) +
                            (it_wrap_row_space_counter < (it_wrap_row_extra_space % it_wrap_row_space_count)); 

                        cont_j += space_justified_count;
                    }
                    it_wrap_row_space_counter++;
                }
                if(cont_j >= cont_size.x) break; // if indent is too big

                it_cont = &(cont_buff[cont_size.x * cont_i + cont_j]);
                (*it_cont) = _it_wrap_rows[j].data[k]; 

                cont_j++;
            }

            cont_i++;

        }
    }

    /* Transpose the cont matrix if needed */
    struct ntg_vcell* it_cell;
    if(label->_opts.orient == NTG_ORIENT_H)
    {
        for(i = 0; i < cont_size.y; i++)
        {
            for(j = 0; j < cont_size.x; j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, i));
                it_cont = &(cont_buff[cont_size.x * i + j]);

                (*it_cell) = ntg_vcell_full((*it_cont), label->_opts.gfx);
            }
        }
    }
    else // NTG_ORIENT_V
    {
        for(i = 0; i < cont_size.y; i++)
        {
            for(j = 0; j < cont_size.x; j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(i, j));
                it_cont = &(cont_buff[cont_size.x * i + j]);

                (*it_cell) = ntg_vcell_full((*it_cont), label->_opts.gfx);
            }
        }
    }
}

/* -------------------------------------------------------------------------- */

static struct ntg_object_measure 
measure_nowrap_fn(const ntg_label* label,
                  const struct ntg_strv_utf32* rows,
                  size_t row_count, ntg_orient orient,
                  size_t for_size, sarena* arena)
{
    size_t indent = label->_opts.indent;
    size_t label_orient = label->_opts.orient;

    if(label_orient == orient)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].len == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].len + indent);
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .nat_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
    else
    {
        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_count, DEFAULT_SIZE),
            .nat_size = _max2_size(row_count, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
}

static struct ntg_object_measure 
measure_wrap_fn(const ntg_label* label,
                const struct ntg_strv_utf32* rows,
                size_t row_count, ntg_orient orient,
                size_t for_size, sarena* arena)
{
    size_t indent = label->_opts.indent;
    size_t label_orient = label->_opts.orient;

    size_t i;
    if(label_orient == orient)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].len == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].len + indent);
        }

        return (struct ntg_object_measure) {
            .min_size = DEFAULT_SIZE,
            .nat_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
    else
    {
        size_t row_counter = 0;
        struct ntg_strv_utf32* it_row_wrap_rows;
        size_t it_row_wrap_row_count;
        for(i = 0; i < row_count; i++)
        {
            it_row_wrap_row_count = get_wrap_rows_wrap(rows[i], for_size,
                    &it_row_wrap_rows, arena);

            row_counter += it_row_wrap_row_count;
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .nat_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
}

static struct ntg_object_measure 
measure_wwrap_fn(const ntg_label* label,
                 const struct ntg_strv_utf32* rows,
                 size_t row_count, ntg_orient orient,
                 size_t for_size, sarena* arena)
{
    size_t indent = label->_opts.indent;
    size_t label_orient = label->_opts.orient;

    size_t i, j;
    struct ntg_strv_utf32* it_words;
    size_t it_word_count;
    if(label_orient == orient)
    {
        size_t max_row_len = 0;
        size_t max_word_len = 0;
        size_t j_word_adj_indent;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].len == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].len + indent);

            it_word_count = ntg_str_utf32_count(rows[i], ' ') + 1;
            it_words = sarena_malloc(arena, sizeof(struct ntg_strv_utf32) *
                                     it_word_count);
            ntg_str_utf32_split(rows[i], ' ', it_words, it_word_count);

            for(j = 0; j < it_word_count; j++)
            {
                // if first word in row, count indent
                j_word_adj_indent = (j == 0) ? indent : 0;

                max_word_len = _max2_size(max_word_len,
                        it_words[j].len + j_word_adj_indent);
            }
            it_words = NULL;
            it_word_count = 0;
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(max_word_len, DEFAULT_SIZE),
            .nat_size = _max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
    else
    {
        size_t row_counter = 0;
        struct ntg_strv_utf32* it_row_wrap_rows;
        size_t it_row_wrap_row_count;
        for(i = 0; i < row_count; i++)
        {
            it_row_wrap_row_count = get_wrap_rows_wwrap(rows[i], for_size,
                    &it_row_wrap_rows, arena);

            row_counter += it_row_wrap_row_count;
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .nat_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
}

static size_t 
get_wrap_rows_nowrap(const struct ntg_strv_utf32 row,
                     size_t for_size,
                     struct ntg_strv_utf32** out_wrap_rows,
                     sarena* arena)
{
    assert(out_wrap_rows != NULL);
    assert(for_size != 0);

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = sarena_malloc(arena, sizeof(struct ntg_strv_utf32));
        assert((*out_wrap_rows) != NULL);
        (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    (*out_wrap_rows) = sarena_malloc(arena, sizeof(struct ntg_strv_utf32));
    assert(out_wrap_rows != NULL);
    (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
        .data = row.data,
        .len = _min2_size(for_size, row.len)
    };
    return 1;
}

static size_t 
get_wrap_rows_wrap(const struct ntg_strv_utf32 row,
                   size_t for_size,
                   struct ntg_strv_utf32** out_wrap_rows,
                   sarena* arena)
{
    assert(out_wrap_rows != NULL);
    assert(for_size != 0);

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = sarena_malloc(arena, sizeof(struct ntg_strv_utf32));
        assert((*out_wrap_rows) != NULL);
        (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    size_t wrap_row_count = ceil((1.0 * row.len) / for_size);
    struct ntg_strv_utf32* wrap_rows = sarena_malloc(arena,
            wrap_row_count * sizeof(struct ntg_strv_utf32));
    assert(wrap_rows != NULL);

    size_t i;
    size_t it_start = 0, it_end;
    for(i = 0; i < wrap_row_count; i++)
    {
        it_end = it_start + _min2_size(for_size, row.len - it_start);

        wrap_rows[i] = (struct ntg_strv_utf32) {
            .data = &(row.data[it_start]),
            .len = it_end - it_start
        };

        it_start = it_end;
    }

    (*out_wrap_rows) = wrap_rows;
    return wrap_row_count;
}

static size_t 
get_wrap_rows_wwrap(const struct ntg_strv_utf32 row,
                    size_t for_size,
                    struct ntg_strv_utf32** out_wrap_rows,
                    sarena* arena)
{
    assert(out_wrap_rows != NULL);
    assert(for_size != 0);

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrap_rows) = (struct ntg_strv_utf32*)sarena_malloc(
                arena, sizeof(struct ntg_strv_utf32));
        assert((*out_wrap_rows) != NULL);
        (*out_wrap_rows)[0] = (struct ntg_strv_utf32) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    size_t word_count = ntg_str_utf32_count(row, ' ') + 1;
    struct ntg_strv_utf32* words = sarena_malloc(arena, word_count *
                                                 sizeof(struct ntg_strv_utf32));
    ntg_str_utf32_split(row, ' ', words, word_count);
    size_t wrap_row_max_count = word_count;

    struct ntg_strv_utf32* wrap_rows = sarena_malloc(arena, wrap_row_max_count *
                                                     sizeof(struct ntg_strv_utf32));
    assert(wrap_rows != NULL);
    size_t i;
    struct ntg_strv_utf32 it_word;
    size_t it_row_len = 0;
    size_t it_row_word_count = 0;
    size_t wrap_row_counter = 0;
    struct ntg_strv_utf32 it_row_start_word = words[0];
    struct ntg_strv_utf32 it_row_end_word;
    size_t effective_space;
    for(i = 0; i < word_count; i++)
    {
        effective_space = (it_row_word_count == 0) ? 0 : 1;
        it_word = words[i];
        if((it_row_len + it_word.len + effective_space) <= for_size)
        {
            it_row_len += (effective_space + it_word.len);
            it_row_word_count++;

            if(i == (word_count - 1))
            {
                /* process last row */

                it_row_end_word = words[i];

                wrap_rows[wrap_row_counter] = (struct ntg_strv_utf32) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[it_row_end_word.len]) -
                        &(it_row_start_word.data[0])
                };

                wrap_row_counter++;
                // it_row_start_word = words[i];
            }
        }
        else
        {
            /* next row */

            if(it_row_word_count > 0)
            {
                it_row_end_word = words[i - 1];

                wrap_rows[wrap_row_counter] = (struct ntg_strv_utf32) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[it_row_end_word.len]) -
                        &(it_row_start_word.data[0])
                };

                it_row_start_word = words[i];

                wrap_row_counter++;
            }

            if(it_word.len < for_size) // can fit in next row
            {
                it_row_len = it_word.len;
                it_row_word_count = 1;

                /* process last row */
                if(i == (word_count - 1))
                {
                    it_row_end_word = words[i];

                    wrap_rows[wrap_row_counter] = (struct ntg_strv_utf32) {
                        .data = it_row_start_word.data,
                            .len = &(it_row_end_word.data[it_row_end_word.len]) -
                                &(it_row_start_word.data[0])
                    };

                    // it_row_start_word = words[i];

                    wrap_row_counter++;
                }
            }
            else // can't fit in next row(or can, but just right)
            {
                /* next row, again */

                it_row_end_word = words[i];

                wrap_rows[wrap_row_counter] = (struct ntg_strv_utf32) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[for_size]) -
                        &(it_row_start_word.data[0])
                };

                if(i < (word_count - 1))
                    it_row_start_word = words[i + 1];

                wrap_row_counter++;

                it_row_len = 0;
                it_row_word_count = 0;
            }
        }
    }

    (*out_wrap_rows) = wrap_rows;
    return wrap_row_counter;
}

static void trim_text(struct ntg_str* text)
{
    if((text->len == 0) || (text->data == NULL)) return;

    struct ntg_strv view = {
        .data = text->data,
        .len = text->len
    };

    size_t word_count = ntg_str_count(view, ' ') + 1;
    struct ntg_strv* words = malloc(word_count * sizeof(struct ntg_strv));
    assert(words != NULL);
    ntg_str_split(view, ' ', words, word_count);

    size_t space_needed = 0;

    size_t i;
    for(i = 0; i < word_count; i++)
    {
        if(words[i].len > 0)
        {
            memmove(text->data + space_needed,
                    words[i].data,
                    words[i].len);

            if(i < (word_count - 1))
            {
                text->data[space_needed + words[i].len] = ' ';
                space_needed += (1 + words[i].len);
            }
            else
                space_needed += words[i].len;
            
        }
    }

    if((space_needed > 0) && (text->data[space_needed - 1] == ' '))
        space_needed--;

    char *tmp = realloc(text->data, space_needed + 1);
    tmp[space_needed] = 0;
    assert(tmp != NULL);
    text->data = tmp;
    text->len = space_needed;

    free(words);
}
