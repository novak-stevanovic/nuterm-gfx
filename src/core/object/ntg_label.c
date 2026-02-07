#include "ntg.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

struct str
{
    char* data;
    size_t len;
};

struct str32
{
    uint32_t* data;
    size_t len;
};

struct str_view
{
    const char* data;
    size_t len;
};

static inline struct str_view get_str_view(struct str string, size_t offset)
{
    if(offset > string.len) offset = string.len;

    return (struct str_view){
        .data = string.data + offset,
        .len  = string.len - offset
    };
}

struct str32_view
{
    const uint32_t* data;
    size_t len;
};

static inline struct str32_view get_str32_view(struct str32 string, size_t offset)
{
    if(offset > string.len) offset = string.len;

    return (struct str32_view){
        .data = string.data + offset,
        .len  = string.len - offset
    };
}

struct ntg_label_priv
{
    struct str32 utf32_text;

    size_t utf32_row_count;
    struct str32_view* utf32_rows;
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static size_t str_count(struct str_view str, char sep);

static size_t str32_count(struct str32_view str, uint32_t sep);

static size_t str_split(struct str_view str, char sep,
        struct str_view* out_views, size_t cap);

static size_t str32_split(struct str32_view str, uint32_t sep,
        struct str32_view* out_views, size_t cap);

static struct ntg_object_measure measure_fn(
        const ntg_object* _label,
        ntg_orient orient,
        void* layout_ch,
        sarena* arena);

static void draw_fn(
        const ntg_object* _label,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena);

/* UGLY CODE - TODO: rewrite sometime */

#define DEFAULT_SIZE 1

static size_t get_wrows_nowrap(
        struct str32_view row,
        size_t for_size,
        struct str32_view** out_rows,
        sarena* arena);

static size_t get_wrows_wrap(
        struct str32_view row,
        size_t for_size,
        struct str32_view** out_rows,
        sarena* arena);

static size_t get_wrows_wwrap(
        struct str32_view row,
        size_t for_size,
        struct str32_view** out_rows,
        sarena* arena);

static struct ntg_object_measure measure_nowrap_fn(
        const ntg_label* label,
        const struct str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena);

static struct ntg_object_measure measure_wrap_fn(
        const ntg_label* label,
        const struct str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena);

static struct ntg_object_measure measure_wwrap_fn(
        const ntg_label* label,
        const struct str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena);

static void trim_text(struct str* text);

/* -------------------------------------------------------------------------- */

static void init_default(ntg_label* label)
{
    label->_opts = ntg_label_opts_def();
    label->_text.len = 0;
    label->_text.data = NULL;
    label->__priv->utf32_text = (struct str32) {0};

    label->__priv->utf32_rows = NULL;
    label->__priv->utf32_row_count = 0;
}

struct ntg_label_opts ntg_label_opts_def()
{
    return (struct ntg_label_opts) {
        .orient = NTG_ORIENT_H,
        .gfx = NT_GFX_DEFAULT,
        .text_mode = NTG_LABEL_TEXT_ALIGN,
        .bg_mode = NTG_LABEL_BG_FULL,
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1,
        .wrap = NTG_LABEL_WRAP_NONE,
        .autotrim = true,
        .indent = false
    };
}

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_label_init(ntg_label* label)
{
    assert(label != NULL);

    struct ntg_object_layout_ops layout_ops = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn
    };

    struct ntg_object_hooks hooks = {0};

    ntg_object_init((ntg_object*)label, &layout_ops, &hooks, &NTG_TYPE_LABEL);

    label->__priv = malloc(sizeof(struct ntg_label_priv));
    assert(label->__priv);

    init_default(label);

    ntg_label_set_text(label, "");
}

void ntg_label_deinit(ntg_label* label)
{
    if(label->_text.data != NULL)
        free(label->_text.data);

    if(label->__priv->utf32_text.data)
        free(label->__priv->utf32_text.data);

    if(label->__priv->utf32_rows)
        free(label->__priv->utf32_rows);

    init_default(label);
    
    free(label->__priv);
    label->__priv = NULL;

    ntg_object_deinit((ntg_object*)label);
}

void ntg_label_deinit_(void* _label)
{
    ntg_label_deinit(_label);
}

struct ntg_label_opts ntg_label_get_opts(const ntg_label* label)
{
    assert(label != NULL);

    return label->_opts;
}

void ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts)
{
    assert(label != NULL);

    label->_opts = (opts ? (*opts) : ntg_label_opts_def());

    struct nt_gfx gfx = label->_opts.gfx;

    struct ntg_vcell cell =
            (label->_opts.bg_mode == NTG_LABEL_BG_FULL) ?
            ntg_vcell_bg(gfx.bg) :
            ntg_vcell_overlay(' ',  gfx.fg, gfx.style);

    ntg_object_set_def_bg((ntg_object*)label, cell);

    ntg_object_mark_dirty((ntg_object*)label, NTG_OBJECT_DIRTY_FULL);
}

void ntg_label_set_text_safe(ntg_label* label, const char* text, size_t len)
{
    assert(label != NULL);

    len = _min2_size(len, (NTG_SIZE_MAX * NTG_SIZE_MAX));

    char* new_text = realloc(label->_text.data, len + 1);
    assert(new_text != NULL);
    label->_text.data = new_text;
    label->_text.len = len;
    label->_text.data[len] = 0;

    if(len)
        memmove(label->_text.data, text, len);

    struct str label_text = {
        label->_text.data,
        label->_text.len
    };

    if(label->_opts.autotrim) trim_text(&label_text);

    label->_text.data = label_text.data;
    label->_text.len = label_text.len;

    if(label->_text.len == 0)
    {
        if(label->__priv->utf32_text.data)
            free(label->__priv->utf32_text.data);
        if(label->__priv->utf32_rows)
            free(label->__priv->utf32_rows);

        label->__priv->utf32_text.data = NULL;
        label->__priv->utf32_text.len = 0;

        label->__priv->utf32_row_count = 0;
        label->__priv->utf32_rows = NULL;

        return;
    }

    // UTF-32 CACHE

    // Realloc old label's buffer - alloc extra space because of unknown size
    size_t utf32_cap = label->_text.len;
    uint32_t* new_utf32_text = realloc(
            label->__priv->utf32_text.data,
            sizeof(uint32_t) * utf32_cap);
    assert(new_utf32_text != NULL);

    // Convert new text to UTF-32 and store in newly alloced buff
    size_t _width;
    int _status;
    uc_utf8_to_utf32((uint8_t*)label->_text.data, utf32_cap,
            new_utf32_text, label->_text.len, 0, &_width, &_status);
    assert(_status == UC_SUCCESS);

    // Shrink the new buffer to save memory
    uint32_t* shrunk_utf32_text = realloc(
            new_utf32_text,
            sizeof(uint32_t) * _width);
    assert(shrunk_utf32_text != NULL);

    // Update label cache to point at the new buff
    label->__priv->utf32_text = (struct str32) {
        .data = shrunk_utf32_text,
        .len = _width
    };

    // Calculate row count
    size_t newline_count;
    newline_count = str32_count(get_str32_view(label->__priv->utf32_text, 0), '\n');
    size_t row_count = newline_count + 1;

    // Create buffer to store rows and their lengths
    struct str32_view* new_rows = realloc(
            label->__priv->utf32_rows,
            sizeof(struct str32_view) * row_count);
    assert(new_rows);

    // Get rows
    str32_split(get_str32_view(label->__priv->utf32_text, 0), '\n', new_rows, row_count);

    // Update cached rows and their lengths
    label->__priv->utf32_rows = new_rows;
    label->__priv->utf32_row_count = row_count;

    ntg_object_mark_dirty((ntg_object*)label, NTG_OBJECT_DIRTY_FULL);
}

void ntg_label_set_text(ntg_label* label, const char* text)
{
    ntg_label_set_text_safe(label, text, strlen(text));
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static struct ntg_object_measure measure_fn(
        const ntg_object* _label,
        ntg_orient orient,
        void* layout_ch,
        sarena* arena)
{
    const ntg_label* label = (const ntg_label*)_label;
    size_t for_size = ntg_object_get_for_size_cont(_label, orient);
    
    if(label->_text.len == 0) return (struct ntg_object_measure) {0};

    size_t row_count = label->__priv->utf32_row_count;
    const struct str32_view* rows = label->__priv->utf32_rows;

    if(row_count == 0) return (struct ntg_object_measure) {0};

    struct ntg_object_measure result;
    switch(label->_opts.wrap)
    {
        case NTG_LABEL_WRAP_NONE:
            result = measure_nowrap_fn(label, rows,
                    row_count, orient, for_size, arena);
            break;
        case NTG_LABEL_WRAP_CHAR:
            result = measure_wrap_fn(label, rows,
                    row_count, orient, for_size, arena);
            break;
        case NTG_LABEL_WRAP_WORD:
            result = measure_wwrap_fn(label, rows,
                    row_count, orient, for_size, arena);
            break;

        default: assert(0);
    }

    return result;
}

static void draw_fn(
        const ntg_object* _label,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena)
{
    const ntg_label* label = (const ntg_label*)_label;
    if((label->_text.len == 0) || (label->_text.data == NULL)) return;

    struct ntg_xy size = ntg_object_get_size_cont(_label);
    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    struct ntg_label_opts opts = label->_opts;

    /* Init cont matrix */
    struct ntg_xy cont_size = 
        (opts.orient == NTG_ORIENT_H) ?
        size :
        ntg_xy_transpose(size);

    size_t i, j, k;
    size_t cont_size_prod = cont_size.x * cont_size.y;
    uint32_t* cont_buff = sarena_malloc(arena, sizeof(uint32_t) * cont_size_prod);
    for(i = 0; i < cont_size_prod; i++) cont_buff[i] = NTG_CELL_EMPTY;

    size_t row_count = label->__priv->utf32_row_count;
    const struct str32_view* rows = label->__priv->utf32_rows;

    size_t capped_indent = _min2_size(opts.indent, cont_size.x);

    /* Create cont matrix */
    size_t cont_i = 0, cont_j = 0;
    /* align variables */
    size_t it_row_align_indent, it_row_effective_indent;
    /* wrap variables */
    size_t _it_wrows_count;
    struct str32_view* _it_wrows;
    uint32_t* it_cont;
    /* justify variables */
    size_t it_wrow_cont_space, it_wrow_extra_space,
            it_wrow_space_count, it_wrow_space_counter;
    for(i = 0; i < row_count; i++)
    {
        _it_wrows = NULL;
        _it_wrows_count = 0;
        switch(opts.wrap)
        {
            case NTG_LABEL_WRAP_NONE:
               _it_wrows_count = get_wrows_nowrap(rows[i],
                       cont_size.x, &_it_wrows, arena); 
                break;
            case NTG_LABEL_WRAP_CHAR:
               _it_wrows_count = get_wrows_wrap(rows[i],
                       cont_size.x, &_it_wrows, arena); 
                break;
            case NTG_LABEL_WRAP_WORD:
               _it_wrows_count = get_wrows_wwrap(rows[i],
                       cont_size.x, &_it_wrows, arena); 
                break;
            default: assert(0);
        }

        for(j = 0; j < _it_wrows_count; j++)
        {
            if(cont_i >= cont_size.y) break;

            /* Avoid overflow in switch statement */
            _it_wrows[j].len = _min2_size(_it_wrows[j].len, cont_size.x);

            if(opts.text_mode == NTG_LABEL_TEXT_ALIGN)
            {
                it_row_align_indent = ntg_align_offset(cont_size.x,
                        _it_wrows[j].len, opts.prim_align);
            }
            else
                it_row_align_indent = 0;

            /* If true row, add capped indent */
            it_row_effective_indent = (j == 0) ?
                _max2_size(capped_indent, it_row_align_indent) :
                it_row_align_indent;
            cont_j = it_row_effective_indent;

            it_wrow_space_counter = 0;
            it_wrow_space_count = str32_count(_it_wrows[j], ' ');
            it_wrow_cont_space = _it_wrows[j].len + it_row_effective_indent;
            it_wrow_extra_space = _ssub_size(cont_size.x, it_wrow_cont_space);
            for(k = 0; k < _it_wrows[j].len; k++)
            {
                if(_it_wrows[j].data[k] == ' ')
                {
                    if((j < (_it_wrows_count - 1)) && opts.text_mode == NTG_LABEL_TEXT_JUSTIFY)
                    {
                        size_t space_justified_count = (it_wrow_extra_space / it_wrow_space_count) +
                            (it_wrow_space_counter < (it_wrow_extra_space % it_wrow_space_count)); 

                        cont_j += space_justified_count;
                    }
                    it_wrow_space_counter++;
                }
                if(cont_j >= cont_size.x) break; // if indent is too big

                it_cont = &(cont_buff[cont_size.x * cont_i + cont_j]);
                (*it_cont) = _it_wrows[j].data[k]; 

                cont_j++;
            }

            cont_i++;

        }
    }

    /* Transpose the cont matrix if needed */
    struct ntg_vcell it_cell;
    struct ntg_xy it_xy;
    for(i = 0; i < cont_size.y; i++)
    {
        for(j = 0; j < cont_size.x; j++)
        {
            it_xy = (opts.orient == NTG_ORIENT_H) ? ntg_xy(j, i) : ntg_xy(i, j);

            it_cont = &(cont_buff[cont_size.x * i + j]);

            it_cell = (opts.bg_mode == NTG_LABEL_BG_FULL) ? 
                    ntg_vcell_full(*it_cont, opts.gfx) :
                    ntg_vcell_overlay(*it_cont, opts.gfx.fg, opts.gfx.style);

            ntg_object_tmp_drawing_set(out_drawing, it_cell, it_xy);
        }
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static size_t str_count(struct str_view str, char sep)
{
    size_t count = 0;

    for (size_t i = 0; i < str.len; ++i)
    {
        if (str.data[i] == sep) ++count;
    }

    return count;
}

static size_t str32_count(struct str32_view str, uint32_t sep)
{
    size_t count = 0;

    for (size_t i = 0; i < str.len; ++i)
    {
        if (str.data[i] == sep) ++count;
    }

    return count;
}

static size_t str_split(struct str_view str, char sep,
        struct str_view* out_views, size_t cap)
{
    size_t count = 0;
    size_t start = 0;
    size_t i;

    if (cap == 0)
        return 0;

    for (i = 0; i <= str.len; ++i)
    {
        if (i == str.len || str.data[i] == sep)
        {
            if (count >= cap) return count;

            out_views[count].data = str.data + start;
            out_views[count].len  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}

static size_t str32_split(struct str32_view str, uint32_t sep,
        struct str32_view* out_views, size_t cap)
{
    size_t count = 0;
    size_t start = 0;
    size_t i;

    if (cap == 0)
        return 0;

    for (i = 0; i <= str.len; ++i)
    {
        if (i == str.len || str.data[i] == sep)
        {
            if (count >= cap) return count;

            out_views[count].data = str.data + start;
            out_views[count].len  = i - start;
            ++count;

            start = i + 1;
        }
    }

    return count;
}

static struct ntg_object_measure measure_nowrap_fn(
        const ntg_label* label,
        const struct str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena)
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

static struct ntg_object_measure measure_wrap_fn(
        const ntg_label* label,
        const struct str32_view* rows,
        size_t row_count,
        ntg_orient orient,
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
        struct str32_view* it_row_wrows;
        size_t it_row_wrow_count;
        for(i = 0; i < row_count; i++)
        {
            it_row_wrow_count = get_wrows_wrap(rows[i], for_size,
                    &it_row_wrows, arena);

            row_counter += it_row_wrow_count;
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .nat_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
}

static struct ntg_object_measure measure_wwrap_fn(
        const ntg_label* label,
        const struct str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena)
{
    size_t indent = label->_opts.indent;
    size_t label_orient = label->_opts.orient;

    size_t i, j;
    struct str32_view* it_words;
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

            it_word_count = str32_count(rows[i], ' ') + 1;
            it_words = sarena_malloc(arena, sizeof(struct str32_view) *
                                     it_word_count);
            str32_split(rows[i], ' ', it_words, it_word_count);

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
        struct str32_view* it_row_wrows;
        size_t it_row_wrow_count;
        for(i = 0; i < row_count; i++)
        {
            it_row_wrow_count = get_wrows_wwrap(rows[i], for_size,
                    &it_row_wrows, arena);

            row_counter += it_row_wrow_count;
        }

        return (struct ntg_object_measure) {
            .min_size = _max2_size(row_counter, DEFAULT_SIZE),
            .nat_size = _max2_size(row_counter, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
}

static size_t get_wrows_nowrap(
        const struct str32_view row,
        size_t for_size,
        struct str32_view** out_wrows,
        sarena* arena)
{
    assert(out_wrows != NULL);
    assert(for_size != 0);

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = sarena_malloc(arena, sizeof(struct str32_view));
        assert((*out_wrows) != NULL);
        (*out_wrows)[0] = (struct str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    (*out_wrows) = sarena_malloc(arena, sizeof(struct str32_view));
    assert(out_wrows != NULL);
    (*out_wrows)[0] = (struct str32_view) {
        .data = row.data,
        .len = _min2_size(for_size, row.len)
    };
    return 1;
}

static size_t get_wrows_wrap(
        const struct str32_view row,
        size_t for_size,
        struct str32_view** out_wrows,
        sarena* arena)
{
    assert(out_wrows != NULL);
    assert(for_size != 0);

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = sarena_malloc(arena, sizeof(struct str32_view));
        assert((*out_wrows) != NULL);
        (*out_wrows)[0] = (struct str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    size_t wrow_count = ceil((1.0 * row.len) / for_size);
    struct str32_view* wrows = sarena_malloc(arena,
            wrow_count * sizeof(struct str32_view));
    assert(wrows != NULL);

    size_t i;
    size_t it_start = 0, it_end;
    for(i = 0; i < wrow_count; i++)
    {
        it_end = it_start + _min2_size(for_size, row.len - it_start);

        wrows[i] = (struct str32_view) {
            .data = &(row.data[it_start]),
            .len = it_end - it_start
        };

        it_start = it_end;
    }

    (*out_wrows) = wrows;
    return wrow_count;
}

static size_t get_wrows_wwrap(
        const struct str32_view row,
        size_t for_size,
        struct str32_view** out_wrows,
        sarena* arena)
{
    assert(out_wrows != NULL);
    assert(for_size != 0);

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = (struct str32_view*)sarena_malloc(
                arena, sizeof(struct str32_view));
        assert((*out_wrows) != NULL);
        (*out_wrows)[0] = (struct str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    size_t word_count = str32_count(row, ' ') + 1;
    struct str32_view* words = sarena_malloc(arena, word_count *
                                                 sizeof(struct str32_view));
    str32_split(row, ' ', words, word_count);
    size_t wrow_max_count = word_count;

    struct str32_view* wrows = sarena_malloc(arena, wrow_max_count *
                                                     sizeof(struct str32_view));
    assert(wrows != NULL);
    struct str32_view it_word;
    size_t it_row_len = 0;
    size_t it_row_word_count = 0;
    size_t wrow_counter = 0;
    struct str32_view it_row_start_word = words[0];
    struct str32_view it_row_end_word;
    size_t effective_space;
    size_t i;
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

                wrows[wrow_counter] = (struct str32_view) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[it_row_end_word.len]) -
                        &(it_row_start_word.data[0])
                };

                wrow_counter++;
                // it_row_start_word = words[i];
            }
        }
        else
        {
            /* next row */

            if(it_row_word_count > 0)
            {
                it_row_end_word = words[i - 1];

                wrows[wrow_counter] = (struct str32_view) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[it_row_end_word.len]) -
                        &(it_row_start_word.data[0])
                };

                it_row_start_word = words[i];

                wrow_counter++;
            }

            if(it_word.len < for_size) // can fit in next row
            {
                it_row_len = it_word.len;
                it_row_word_count = 1;

                /* process last row */
                if(i == (word_count - 1))
                {
                    it_row_end_word = words[i];

                    wrows[wrow_counter] = (struct str32_view) {
                        .data = it_row_start_word.data,
                            .len = &(it_row_end_word.data[it_row_end_word.len]) -
                                &(it_row_start_word.data[0])
                    };

                    // it_row_start_word = words[i];

                    wrow_counter++;
                }
            }
            else // can't fit in next row(or can, but just right)
            {
                /* next row, again */

                it_row_end_word = words[i];

                wrows[wrow_counter] = (struct str32_view) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[for_size]) -
                        &(it_row_start_word.data[0])
                };

                if(i < (word_count - 1))
                    it_row_start_word = words[i + 1];

                wrow_counter++;

                it_row_len = 0;
                it_row_word_count = 0;
            }
        }
    }

    (*out_wrows) = wrows;
    return wrow_counter;
}

static void trim_text(struct str* text)
{
    if((text->len == 0) || (text->data == NULL)) return;

    struct str_view view = get_str_view(*text, 0);

    size_t word_count = str_count(view, ' ') + 1;
    struct str_view* words = malloc(word_count * sizeof(struct str_view));
    assert(words != NULL);
    str_split(view, ' ', words, word_count);

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
