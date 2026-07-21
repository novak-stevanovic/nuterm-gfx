#include "ntg.h"
#include <math.h>
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"
#include <string.h>

/* UGLY CODE - TODO: rewrite sometime */

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

#define DEFAULT_SIZE 1

static size_t get_wrows_nowrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena);

static size_t get_wrows_wrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena);

static size_t get_wrows_wwrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena);

static struct ntg_object_measure measure_nowrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena);

static struct ntg_object_measure measure_wrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena);

static struct ntg_object_measure measure_wwrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena);

static int trim_text(struct ntg_str* text);

/* ------------------------------------------------------ */

static void init_default(ntg_text* text_obj)
{
    text_obj->_opts = ntg_text_opts_def();

    text_obj->_gfx = NT_GFX_DEFAULT;

    text_obj->_text.len = 0;
    text_obj->_text.data = NULL;

    text_obj->_cache.utf32_text = (struct ntg_str32) {0};
    text_obj->_cache.utf32_rows = NULL;
    text_obj->_cache.utf32_row_count = 0;
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_text_opts ntg_text_opts_def()
{
    return (struct ntg_text_opts) {
        .orient = NTG_ORIENT_H,
        .gfx = NT_GFX_DEFAULT,
        .text_mode = NTG_TEXT_ALIGN,
        .bg_mode = NTG_TEXT_BG_FULL,
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1,
        .wrap = NTG_TEXT_WRAP_NONE,
        .indent = 0,
    };
}

bool ntg_text_opts_are_eq(
        const struct ntg_text_opts* opts1,
        const struct ntg_text_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->orient == opts2->orient) &&
           nt_gfx_are_equal(opts1->gfx, opts2->gfx) &&
           (opts1->text_mode == opts2->text_mode) &&
           (opts1->prim_align == opts2->prim_align) &&
           (opts1->sec_align == opts2->sec_align) &&
           (opts1->bg_mode == opts2->bg_mode) &&
           (opts1->wrap == opts2->wrap) &&
           (opts1->indent == opts2->indent));
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_text_deinit(ntg_text* text_obj)
{
    if(!text_obj) return;

    free(text_obj->_text.data);

    init_default(text_obj);

    ntg_object_deinit((ntg_object*)text_obj);
}

void ntg_text_deinit_(void* _text)
{
    ntg_text_deinit(_text);
}

void ntg_text_set_opts(ntg_text* text_obj, const struct ntg_text_opts* opts)
{
    if(!text_obj) return;

    ntg_object* _text = ntg_obj(text_obj);

    struct ntg_text_opts old_opts = text_obj->_opts;
    struct ntg_text_opts new_opts = (opts ? (*opts) : ntg_text_opts_def());

    if(ntg_text_opts_are_eq(&old_opts, &new_opts))
        return;

    text_obj->_opts = new_opts;

    struct nt_gfx gfx = text_obj->_opts.gfx;

    struct ntg_vcell cell =
            (text_obj->_opts.bg_mode == NTG_TEXT_BG_FULL) ?
            ntg_vcell_bg(gfx.bg) :
            ntg_vcell_overlay(' ',  gfx.fg, gfx.style);

    ntg_object_set_base_bg(_text, cell);

    text_obj->_gfx = 
        (ntg_object_is_focused(ntg_obj(text_obj)) ?
        new_opts.focused_gfx :
        new_opts.gfx);

    ntg_object_mark_dirty((ntg_object*)text_obj, NTG_OBJECT_DIRTY_FULL);
}

void ntg_text_set_text_unsafe(
        ntg_text* text_obj,
        const char* text,
        ntg_text_set_mode mode,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!text_obj || !text)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_text_set_text(text_obj, text, strlen(text), mode, out_status);
}

void ntg_text_set_text(
        ntg_text* text_obj,
        const char* text,
        size_t len,
        ntg_text_set_mode mode,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!text_obj || !text)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    len = _min2_size(len, (NTG_SIZE_MAX * NTG_SIZE_MAX));

    if((text_obj->_text.len == len) &&
            ((len == 0) || (memcmp(text_obj->_text.data, text, len) == 0)))
        return;

    bool raise_hook = (text_obj->_text.data != NULL);
    size_t old_text_len = text_obj->_text.len;
    char* old_text = NULL;
    if(raise_hook)
    {
        old_text = malloc(old_text_len + 1);
        if(!old_text)
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        if(old_text_len > 0)
            memmove(old_text, text_obj->_text.data, old_text_len);
        old_text[old_text_len] = '\0';
    }

    char* new_text = malloc(len + 1);
    if(!new_text)
    {
        free(old_text);
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    if(len > 0)
        memmove(new_text, text, len);
    new_text[len] = '\0';

    struct ntg_str text_text = {
        .data = new_text,
        .len = len
    };

    if(mode == NTG_TEXT_SET_RM_WS)
    {
        int trim_status = trim_text(&text_text);
        if(trim_status != 0)
        {
            free(text_text.data);
            free(old_text);
            ntg_vreturn(out_status, trim_status);
        }
    }

    if(text_text.len == 0)
    {
        free(text_obj->_text.data);
        free(text_obj->_cache.utf32_text.data);
        free(text_obj->_cache.utf32_rows);

        text_obj->_text.data = text_text.data;
        text_obj->_text.len = text_text.len;
        text_obj->_cache.utf32_text = (struct ntg_str32) {0};
        text_obj->_cache.utf32_rows = NULL;
        text_obj->_cache.utf32_row_count = 0;

        ntg_object_mark_dirty((ntg_object*)text_obj,
            NTG_OBJECT_DIRTY_FULL);

        free(old_text);
        return;
    }

    size_t utf32_cap = text_text.len;
    uint32_t* new_utf32_text = malloc(sizeof(uint32_t) * utf32_cap);
    if(!new_utf32_text)
    {
        free(text_text.data);
        free(old_text);
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    size_t width = 0;
    int status;
    uc_utf8_to_utf32((uint8_t*)text_text.data, text_text.len,
            new_utf32_text, utf32_cap, 0, &width, &status);
    if(status != 0)
    {
        free(new_utf32_text);
        free(text_text.data);
        free(old_text);
        ntg_vreturn(out_status, NTG_ERR_UTF_CONV);
    }

    if(width == 0)
    {
        free(new_utf32_text);
        new_utf32_text = NULL;
    }
    else if(width < utf32_cap)
    {
        uint32_t* shrunk = realloc(new_utf32_text, sizeof(uint32_t) * width);
        if(shrunk)
            new_utf32_text = shrunk;
    }

    struct ntg_str32 utf32_text = {
        .data = new_utf32_text,
        .len = width
    };

    size_t row_count = str32_count(get_str32_view(utf32_text, 0), '\n') + 1;
    struct ntg_str32_view* new_rows = malloc(sizeof(struct ntg_str32_view) * row_count);
    if(!new_rows)
    {
        free(new_utf32_text);
        free(text_text.data);
        free(old_text);
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    str32_split(get_str32_view(utf32_text, 0), '\n', new_rows, row_count);

    free(text_obj->_text.data);
    free(text_obj->_cache.utf32_text.data);
    free(text_obj->_cache.utf32_rows);

    text_obj->_text.data = text_text.data;
    text_obj->_text.len = text_text.len;
    text_obj->_cache.utf32_text = utf32_text;
    text_obj->_cache.utf32_rows = new_rows;
    text_obj->_cache.utf32_row_count = row_count;

    ntg_object_mark_dirty((ntg_object*)text_obj,
            NTG_OBJECT_DIRTY_FULL);

    free(old_text);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

void ntg_text_init_inherit(
        ntg_text* text_obj,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!text_obj || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_TEXT))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    int _status;

    ntg_object_init_inherit((ntg_object*)text_obj, vtable, type, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    init_default(text_obj);

    ntg_text_set_text_unsafe(text_obj, "", NTG_TEXT_SET_DEF, &_status);
    if(_status != 0)
    {
        ntg_object_deinit((ntg_object*)text_obj);
        ntg_vreturn(out_status, _status);
    }
}

struct ntg_object_measure ntg_text_measure_fn(
        const ntg_object* _text_obj,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_text* text_obj = (const ntg_text*)_text_obj;
    size_t for_size = ntg_object_get_for_size_cont(_text_obj, orient);
    if(for_size == 0) return (struct ntg_object_measure) {0};

    if(text_obj->_text.len == 0) return (struct ntg_object_measure) {0};

    size_t row_count = text_obj->_cache.utf32_row_count;
    const struct ntg_str32_view* rows = text_obj->_cache.utf32_rows;

    if(row_count == 0) return (struct ntg_object_measure) {0};

    struct ntg_object_measure result;
    switch(text_obj->_opts.wrap)
    {
        case NTG_TEXT_WRAP_NONE:
            result = measure_nowrap_fn(text_obj, rows,
                    row_count, orient, for_size, arena);
            break;
        case NTG_TEXT_WRAP_CHAR:
            result = measure_wrap_fn(text_obj, rows,
                    row_count, orient, for_size, arena);
            break;
        case NTG_TEXT_WRAP_WORD:
            result = measure_wwrap_fn(text_obj, rows,
                    row_count, orient, for_size, arena);
            break;

        default:
            result = (struct ntg_object_measure) {0};
            break;
    }

    return result;
}

void ntg_text_draw_fn(
        const ntg_object* _text_obj,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_text* text_obj = (const ntg_text*)_text_obj;
    if((text_obj->_text.len == 0) || (text_obj->_text.data == NULL)) return;

    struct ntg_xy size = ntg_object_get_size_cont(_text_obj);
    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    struct ntg_text_opts opts = text_obj->_opts;

    /* Init cont matrix */
    struct ntg_xy cont_size =
        (opts.orient == NTG_ORIENT_H) ?
        size :
        ntg_xy_transpose(size);

    size_t i, j, k;
    size_t cont_size_prod = cont_size.x * cont_size.y;
    uint32_t* cont_buff = sarena_malloc(arena, sizeof(uint32_t) * cont_size_prod);
    for(i = 0; i < cont_size_prod; i++) cont_buff[i] = ' ';

    size_t row_count = text_obj->_cache.utf32_row_count;
    const struct ntg_str32_view* rows = text_obj->_cache.utf32_rows;

    size_t capped_indent = _min2_size(opts.indent, cont_size.x);

    /* Create cont matrix */
    size_t cont_i = 0, cont_j = 0;
    /* align variables */
    size_t it_row_align_indent, it_row_effective_indent;
    /* wrap variables */
    size_t _it_wrows_count;
    struct ntg_str32_view* _it_wrows;
    uint32_t* it_cont;
    /* justify variables */
    size_t it_wrow_cont_space, it_wrow_extra_space,
            it_wrow_space_count, it_wrow_space_counter;
    for(i = 0; i < row_count; i++)
    {
        _it_wrows = NULL;
        switch(opts.wrap)
        {
            case NTG_TEXT_WRAP_NONE:
               _it_wrows_count = get_wrows_nowrap(rows[i],
                       cont_size.x, &_it_wrows, arena);
                break;
            case NTG_TEXT_WRAP_CHAR:
               _it_wrows_count = get_wrows_wrap(rows[i],
                       cont_size.x, &_it_wrows, arena);
                break;
            case NTG_TEXT_WRAP_WORD:
               _it_wrows_count = get_wrows_wwrap(rows[i],
                       cont_size.x, &_it_wrows, arena);
                break;
            default:
                return;
        }

        for(j = 0; j < _it_wrows_count; j++)
        {
            if(cont_i >= cont_size.y) break;

            /* Avoid overflow in switch statement */
            _it_wrows[j].len = _min2_size(_it_wrows[j].len, cont_size.x);

            if(opts.text_mode == NTG_TEXT_ALIGN)
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
                    if((j < (_it_wrows_count - 1)) && opts.text_mode == NTG_TEXT_JUSTIFY)
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

            it_cell = (opts.bg_mode == NTG_TEXT_BG_FULL) ?
                    ntg_vcell_full(*it_cont, opts.gfx) :
                    ntg_vcell_overlay(*it_cont, opts.gfx.fg, opts.gfx.style);

            ntg_object_tmp_drawing_set(out_drawing, it_cell, it_xy);
        }
    }
}

void ntg_text_deinit_fn(ntg_object* _text_obj)
{
    ntg_text_deinit((ntg_text*)_text_obj);
}

void ntg_text_focus_fn(ntg_object* object, ntg_object* old_focused)
{
    if(!object) return;

    ntg_text* text = (ntg_text*)object;

    text->_gfx = text->_opts.focused_gfx;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW | NTG_OBJECT_DIRTY_RENDER);
}

void ntg_text_unfocus_fn(ntg_object* object, ntg_object* new_focused)
{
    if(!object) return;

    ntg_text* text = (ntg_text*)object;

    text->_gfx = text->_opts.gfx;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW | NTG_OBJECT_DIRTY_RENDER);
}

const struct ntg_object_vtable NTG_TEXT_VTABLE = {
    .measure_fn = ntg_text_measure_fn,
    .draw_fn = ntg_text_draw_fn,
    .deinit_fn = ntg_text_deinit_fn,
    .focus_fn = ntg_text_focus_fn,
    .unfocus_fn = ntg_text_unfocus_fn
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static struct ntg_object_measure measure_nowrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena)
{
    size_t indent = text_obj->_opts.indent;
    size_t text_orient = text_obj->_opts.orient;

    if(text_orient == orient)
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
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size, sarena* arena)
{
    size_t indent = text_obj->_opts.indent;
    size_t text_orient = text_obj->_opts.orient;

    size_t i;
    if(text_orient == orient)
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
        struct ntg_str32_view* it_row_wrows;
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
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena)
{
    size_t indent = text_obj->_opts.indent;
    size_t text_orient = text_obj->_opts.orient;

    size_t i, j;
    struct ntg_str32_view* it_words;
    size_t it_word_count;
    if(text_orient == orient)
    {
        size_t max_row_len = 0;
        size_t max_word_len = 0;
        size_t j_word_adj_indent;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].len == 0) continue;

            max_row_len = _max2_size(max_row_len, rows[i].len + indent);

            it_word_count = str32_count(rows[i], ' ') + 1;
            it_words = sarena_malloc(arena, sizeof(struct ntg_str32_view) *
                                     it_word_count);
            str32_split(rows[i], ' ', it_words, it_word_count);

            for(j = 0; j < it_word_count; j++)
            {
                // if first word in row, count indent
                j_word_adj_indent = (j == 0) ? indent : 0;

                max_word_len = _max2_size(max_word_len,
                        it_words[j].len + j_word_adj_indent);
            }
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
        struct ntg_str32_view* it_row_wrows;
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
        const struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_wrows,
        sarena* arena)
{
    if(for_size == 0) return 0;

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = sarena_malloc(arena, sizeof(struct ntg_str32_view));
        if(!*out_wrows) return 0;

        (*out_wrows)[0] = (struct ntg_str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    (*out_wrows) = sarena_malloc(arena, sizeof(struct ntg_str32_view));
    if(!*out_wrows) return 0;
    (*out_wrows)[0] = (struct ntg_str32_view) {
        .data = row.data,
        .len = _min2_size(for_size, row.len)
    };
    return 1;
}

static size_t get_wrows_wrap(
        const struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_wrows,
        sarena* arena)
{
    if(for_size == 0) return 0;

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = sarena_malloc(arena, sizeof(struct ntg_str32_view));
        if(!*out_wrows) return 0;
        (*out_wrows)[0] = (struct ntg_str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    size_t wrow_count = (row.len + for_size - 1) / for_size;
    struct ntg_str32_view* wrows = sarena_malloc(arena,
            wrow_count * sizeof(struct ntg_str32_view));
    if(!wrows) return 0;

    size_t i;
    size_t it_start = 0, it_end;
    for(i = 0; i < wrow_count; i++)
    {
        it_end = it_start + _min2_size(for_size, row.len - it_start);

        wrows[i] = (struct ntg_str32_view) {
            .data = &(row.data[it_start]),
            .len = it_end - it_start
        };

        it_start = it_end;
    }

    (*out_wrows) = wrows;
    return wrow_count;
}

static size_t get_wrows_wwrap(
        const struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_wrows,
        sarena* arena)
{
    if(for_size == 0) return 0;

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = (struct ntg_str32_view*)sarena_malloc(
                arena, sizeof(struct ntg_str32_view));
        if(!*out_wrows) return 0;

        (*out_wrows)[0] = (struct ntg_str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    struct ntg_str32_view *words, *wrows;
    size_t word_count = str32_count(row, ' ') + 1;
    words = sarena_malloc(arena, word_count * sizeof(struct ntg_str32_view));
    if(!words) return 0;
    str32_split(row, ' ', words, word_count);
    size_t wrow_max_count = word_count;

    wrows = sarena_malloc(arena, wrow_max_count * sizeof(struct ntg_str32_view));
    if(!wrows) return 0;

    struct ntg_str32_view it_word;
    size_t it_row_len = 0;
    size_t it_row_word_count = 0;
    size_t wrow_counter = 0;
    struct ntg_str32_view it_row_start_word = words[0];
    struct ntg_str32_view it_row_end_word;
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

                wrows[wrow_counter] = (struct ntg_str32_view) {
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

                wrows[wrow_counter] = (struct ntg_str32_view) {
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

                    wrows[wrow_counter] = (struct ntg_str32_view) {
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

                wrows[wrow_counter] = (struct ntg_str32_view) {
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

static int trim_text(struct ntg_str* text)
{
    if((text->len == 0) || (text->data == NULL))
        return 0;

    struct ntg_str_view view = get_str_view(*text, 0);

    size_t word_count = str_count(view, ' ') + 1;
    struct ntg_str_view* words = calloc(word_count, sizeof(struct ntg_str_view));
    if(!words)
        return NTG_ERR_ALLOC_FAIL;
    word_count = str_split(view, ' ', words, word_count);

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
    if(!tmp)
    {
        free(words);
        return NTG_ERR_ALLOC_FAIL;
    }

    tmp[space_needed] = 0;
    text->data = tmp;
    text->len = space_needed;

    free(words);

    return 0;
}
