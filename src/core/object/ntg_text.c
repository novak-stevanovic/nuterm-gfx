#include "ntg.h"
#include <math.h>
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"
#include <string.h>

// TODO:
#include <assert.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static const struct ntg_text_vtable TEXT_VTABLE_EMPTY = {0};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

#define DEFAULT_SIZE 1

static size_t get_wrows_nowrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena,
        int* out_retry);

static size_t get_wrows_wrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena,
        int* out_retry);

static size_t get_wrows_wwrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena,
        int* out_retry);

static struct ntg_object_measure measure_nowrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena,
        int* out_remeasure);

static struct ntg_object_measure measure_wrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena,
        int* out_remeasure);

static struct ntg_object_measure measure_wwrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena,
        int* out_remeasure);

static struct ntg_xy calculate_effective_scroll(const ntg_text* text_obj);

static int trim_text(struct ntg_str* text);

static void update_object_bg(ntg_text* text_obj);

/* ------------------------------------------------------ */

static void init_default(ntg_text* text_obj)
{
    text_obj->_opts = ntg_text_opts_default();
    text_obj->__vtable = NULL;

    text_obj->_gfx = NT_GFX_DEFAULT;

    text_obj->_text.len = 0;
    text_obj->_text.data = NULL;

    text_obj->_scroll = ntg_xy(0, 0);

    text_obj->__cache.utf32_text = (struct ntg_str32) {0};
    text_obj->__cache.utf32_rows = NULL;
    text_obj->__cache.utf32_row_count = 0;
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_text_opts ntg_text_opts_default(void)
{
    struct ntg_text_opts opts =  {
        .orient = NTG_ORIENT_H,
        .gfx = NT_GFX_DEFAULT,
        .wrap = NTG_TEXT_WRAP_NONE,
        .focused_gfx = NT_GFX_DEFAULT,
        .line_mode = NTG_TEXT_LINE_ALIGN,
        .bg_mode = NTG_TEXT_BG_FULL,
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1,
        .indent = 0,
    };

    opts.focused_gfx = nt_gfx_style_reverse(opts.gfx);

    return opts;
}

bool ntg_text_opts_are_eql(
        const struct ntg_text_opts* opts1,
        const struct ntg_text_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->orient == opts2->orient) &&
           nt_gfx_are_eql(opts1->gfx, opts2->gfx) &&
           nt_gfx_are_eql(opts1->focused_gfx, opts2->focused_gfx) &&
           (opts1->line_mode == opts2->line_mode) &&
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

    if(text_obj->_text.data)
        free(text_obj->_text.data);
    if(text_obj->__cache.utf32_text.data)
        free(text_obj->__cache.utf32_text.data);
    if(text_obj->__cache.utf32_rows)
        free(text_obj->__cache.utf32_rows);

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

    struct ntg_text_opts old_opts = text_obj->_opts;
    struct ntg_text_opts new_opts = (opts ? (*opts) : ntg_text_opts_default());

    if(ntg_text_opts_are_eql(&old_opts, &new_opts))
        return;

    text_obj->_opts = new_opts;

    struct nt_gfx gfx =
        (ntg_object_is_focused(ntg_obj(text_obj)) ?
        new_opts.focused_gfx :
        new_opts.gfx);

    text_obj->_gfx = gfx;

    update_object_bg(text_obj);

    ntg_object_mark_dirty((ntg_object*)text_obj, NTG_OBJECT_DIRTY_FULL);
}

void ntg_text_set_text_unsafe(
        ntg_text* text_obj,
        const char* text,
        uint16_t flags,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!text_obj || !text)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_text_set_text(text_obj, text, strlen(text), flags, out_status);
}

void ntg_text_set_text(
        ntg_text* text_obj,
        const char* text,
        size_t len,
        uint16_t flags,
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

    if(flags & NTG_TEXT_SET_RM_WS)
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
        free(text_obj->__cache.utf32_text.data);
        free(text_obj->__cache.utf32_rows);

        text_obj->_text.data = text_text.data;
        text_obj->_text.len = text_text.len;
        text_obj->__cache.utf32_text = (struct ntg_str32) {0};
        text_obj->__cache.utf32_rows = NULL;
        text_obj->__cache.utf32_row_count = 0;

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

    size_t row_count = ntg_str32_count(ntg_str32_get_view(utf32_text, 0), '\n') + 1;
    struct ntg_str32_view* new_rows = malloc(sizeof(struct ntg_str32_view) * row_count);
    if(!new_rows)
    {
        free(new_utf32_text);
        free(text_text.data);
        free(old_text);
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    ntg_str32_split(ntg_str32_get_view(utf32_text, 0), '\n', new_rows, row_count);

    free(text_obj->_text.data);
    free(text_obj->__cache.utf32_text.data);
    free(text_obj->__cache.utf32_rows);

    text_obj->_text.data = text_text.data;
    text_obj->_text.len = text_text.len;
    text_obj->__cache.utf32_text = utf32_text;
    text_obj->__cache.utf32_rows = new_rows;
    text_obj->__cache.utf32_row_count = row_count;

    ntg_object_mark_dirty((ntg_object*)text_obj,
            NTG_OBJECT_DIRTY_FULL);

    free(old_text);
}

/* ------------------------------------------------------ */
/* SCROLL */
/* ------------------------------------------------------ */

void ntg_text_set_scroll(ntg_text* text_obj, struct ntg_xy scroll)
{
    if(!text_obj) return;

    text_obj->_scroll = scroll;

    text_obj->_scroll = calculate_effective_scroll(text_obj);

    ntg_object_mark_dirty(ntg_obj(text_obj), NTG_OBJECT_DIRTY_DRAW);
}

void ntg_text_scroll(ntg_text* text_obj, struct ntg_dxy scroll_diff)
{
    if(!text_obj) return;
    if((scroll_diff.x == 0) && (scroll_diff.y == 0))
        return;

    struct ntg_dxy curr_scroll_dxy = ntg_dxy_from_xy(text_obj->_scroll);

    struct ntg_dxy scroll_dxy = ntg_dxy_add(curr_scroll_dxy, scroll_diff);

    struct ntg_xy scroll = ntg_xy_from_dxy(scroll_dxy);

    ntg_text_set_scroll(text_obj, scroll);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_text_init_inherit(
        ntg_text* text_obj,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!text_obj || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_TEXT))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    int _status;

    ntg_object_init_inherit(
            (ntg_object*)text_obj, object_vtable, type, layout_dt, &_status);
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

    ntg_text_set_text_unsafe(text_obj, "", 0, &_status);
    if(_status != 0)
    {
        ntg_object_deinit((ntg_object*)text_obj);
        ntg_vreturn(out_status, _status);
    }

    text_obj->__vtable = (text_vtable ? text_vtable : &TEXT_VTABLE_EMPTY);
}

struct ntg_object_measure ntg_text_measure_fn(
        const ntg_object* _text_obj,
        struct ntg_object_layout_dt* layout_dt,
        ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        int* out_status)
{
    (void)layout_dt;
    (void)relayout;
    ntg_init_status(out_status);

    int _remeasure = 0;
    const ntg_text* text_obj = (const ntg_text*)_text_obj;
    size_t for_size = ntg_object_get_for_size_cont(_text_obj, orient);
    if(for_size == 0) return (struct ntg_object_measure) {0};

    if(text_obj->_text.len == 0) return (struct ntg_object_measure) {0};

    size_t row_count = text_obj->__cache.utf32_row_count;
    const struct ntg_str32_view* rows = text_obj->__cache.utf32_rows;

    if(row_count == 0) return (struct ntg_object_measure) {0};

    struct ntg_object_measure result;
    switch(text_obj->_opts.wrap)
    {
        case NTG_TEXT_WRAP_NONE:
            result = measure_nowrap_fn(text_obj, rows,
                    row_count, orient, for_size, arena, &_remeasure);
            break;
        case NTG_TEXT_WRAP_CHAR:
            result = measure_wrap_fn(text_obj, rows,
                    row_count, orient, for_size, arena, &_remeasure);
            break;
        case NTG_TEXT_WRAP_WORD:
            result = measure_wwrap_fn(text_obj, rows,
                    row_count, orient, for_size, arena, &_remeasure);
            break;

        default:
            result = (struct ntg_object_measure) {0};
            break;
    }

    if(_remeasure)
        ntg_return(result, out_status, NTG_ERR_LAYOUT_FAIL);

    return result;
}

void ntg_text_draw_fn(
        const ntg_object* _text_obj,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout,
        int* out_status)
{
    (void)layout_dt;
    (void)relayout;
    ntg_init_status(out_status);

    struct ntg_xy cont_size = ntg_object_get_size_cont(_text_obj);
    if(ntg_xy_size_is_zero(cont_size)) return;

    int _redraw = 0;
    const ntg_text* text_obj = (const ntg_text*)_text_obj;
    if((text_obj->_text.len == 0) || (text_obj->_text.data == NULL)) return;

    struct ntg_text_opts opts = text_obj->_opts;

    struct ntg_xy cont_nat_size = ntg_object_get_nat_size_cont(_text_obj);

    /* Determine full size */

    struct ntg_xy full_size;

    if(opts.wrap == NTG_TEXT_WRAP_NONE)
    {
        /*
        full_size = ntg_xy(
            _max2_size(cont_nat_size.x, cont_size.x),
            _max2_size(cont_nat_size.y, cont_size.y));
        */
        full_size = ntg_xy(cont_nat_size.x, cont_nat_size.y);
    }
    else
    {
        if(opts.orient == NTG_ORIENT_H)
            full_size = ntg_xy(cont_size.x, cont_nat_size.y);
        else
            full_size = ntg_xy(cont_nat_size.x, cont_size.y);
    }

    if(ntg_xy_size_is_zero(full_size)) return;

    struct ntg_xy full_size_adj =
        (opts.orient == NTG_ORIENT_H) ?
        full_size :
        ntg_xy_transpose(full_size);

    size_t i, j, k;
    size_t full_size_prod = full_size_adj.x * full_size_adj.y;
    uint32_t* full_buff = sarena_malloc(arena, sizeof(uint32_t) * full_size_prod);
    if(!full_buff)
        ntg_vreturn(out_status, NTG_ERR_LAYOUT_FAIL);

    for(i = 0; i < full_size_prod; i++) full_buff[i] = ' ';

    size_t row_count = text_obj->__cache.utf32_row_count;
    const struct ntg_str32_view* rows = text_obj->__cache.utf32_rows;

    size_t capped_indent = _min2_size(opts.indent, _sub2_size(full_size_adj.x, 1));
    
    size_t cont_i = 0, cont_j = 0;
    
    size_t it_row_align_indent, it_row_effective_indent;
    
    size_t _it_wrows_count;
    struct ntg_str32_view* _it_wrows;
    size_t it_idx;
    
    size_t it_wrow_cont_space, it_wrow_extra_space,
           it_wrow_space_count, it_wrow_space_counter;
    for(i = 0; i < row_count; i++)
    {
        _it_wrows = NULL;
        switch(opts.wrap)
        {
            case NTG_TEXT_WRAP_NONE:
               _it_wrows_count = get_wrows_nowrap(rows[i],
                       full_size_adj.x, &_it_wrows, arena, &_redraw);
                break;
            case NTG_TEXT_WRAP_CHAR:
               _it_wrows_count = get_wrows_wrap(rows[i],
                       full_size_adj.x, &_it_wrows, arena, &_redraw);
                break;
            case NTG_TEXT_WRAP_WORD:
               _it_wrows_count = get_wrows_wwrap(rows[i],
                       full_size_adj.x, &_it_wrows, arena, &_redraw);
                break;
            default:
                return;
        }

        if(_redraw || (_it_wrows_count == SIZE_MAX))
            ntg_vreturn(out_status, NTG_ERR_LAYOUT_FAIL);

        for(j = 0; j < _it_wrows_count; j++)
        {
            if(cont_i >= full_size_adj.y) break;
            
            _it_wrows[j].len = _min2_size(_it_wrows[j].len, full_size_adj.x);

            if(opts.line_mode == NTG_TEXT_LINE_ALIGN)
            {
                it_row_align_indent = ntg_align_offset(
                        full_size_adj.x,
                        _it_wrows[j].len,
                        opts.prim_align);
            }
            else
                it_row_align_indent = 0;

            
            it_row_effective_indent = (j == 0) ?
                _max2_size(capped_indent, it_row_align_indent) :
                it_row_align_indent;
            cont_j = it_row_effective_indent;

            it_wrow_space_counter = 0;
            it_wrow_space_count = ntg_str32_count(_it_wrows[j], ' ');
            it_wrow_cont_space = _it_wrows[j].len + it_row_effective_indent;
            it_wrow_extra_space = _sub2_size(full_size_adj.x, it_wrow_cont_space);
            for(k = 0; k < _it_wrows[j].len; k++)
            {
                if(_it_wrows[j].data[k] == ' ')
                {
                    if((j < (_it_wrows_count - 1)) && opts.line_mode == NTG_TEXT_LINE_JUSTIFY)
                    {
                        size_t space_justified_count = (it_wrow_extra_space / it_wrow_space_count) +
                            (it_wrow_space_counter < (it_wrow_extra_space % it_wrow_space_count));

                        cont_j += space_justified_count;
                    }
                    it_wrow_space_counter++;
                }
                if(cont_j >= full_size_adj.x) break; 

                it_idx = (full_size_adj.x * cont_i) + cont_j;

                if(it_idx >= full_size_prod)
                    continue;

                full_buff[it_idx] = _it_wrows[j].data[k];

                cont_j++;
            }

            cont_i++;

        }
    }

    /* Viewport size is content size */

    struct ntg_xy scroll = calculate_effective_scroll(text_obj);

    struct ntg_xy scroll_adj = 
        (opts.orient == NTG_ORIENT_H) ?
        scroll :
        ntg_xy_transpose(scroll);

    struct ntg_xy src_xy;
    struct ntg_xy dst_xy;
    struct ntg_vcell it_cell;
    uint32_t it_cont;

    for(i = 0; i < cont_size.y; i++)
    {
        for(j = 0; j < cont_size.x; j++)
        {
            src_xy = ntg_xy(scroll_adj.x + j, scroll_adj.y + i);

            if((src_xy.x < full_size_adj.x) && (src_xy.y < full_size_adj.y))
            {
                it_cont = full_buff[full_size_adj.x * src_xy.y + src_xy.x];

                it_cell = (opts.bg_mode == NTG_TEXT_BG_FULL) ?
                    ntg_vcell_new_full(it_cont, text_obj->_gfx) :
                    ntg_vcell_new_overlay(it_cont, text_obj->_gfx.fg, text_obj->_gfx.style);
            }
            else
            {
                it_cell = (opts.bg_mode == NTG_TEXT_BG_FULL) ?
                    ntg_vcell_new_full(' ' , text_obj->_gfx) :
                    ntg_vcell_new_transparent();
            }

            dst_xy = (opts.orient == NTG_ORIENT_H) ? ntg_xy(j, i) : ntg_xy(i, j);

            ntg_object_tmp_drawing_set(out_drawing, it_cell, dst_xy);
        }
    }

    if(text_obj->__vtable && text_obj->__vtable->post_draw_fn)
        text_obj->__vtable->post_draw_fn(text_obj, out_drawing, arena);
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

    update_object_bg(text);

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW);
}

void ntg_text_unfocus_fn(ntg_object* object, ntg_object* new_focused)
{
    if(!object) return;

    ntg_text* text = (ntg_text*)object;

    text->_gfx = text->_opts.gfx;

    update_object_bg(text);

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW);
}

void ntg_text_layout_finalize_fn(ntg_object* object, sarena* arena)
{
    ntg_text* text = ntg_txt(object);

    text->_scroll = calculate_effective_scroll(text);
}

const struct ntg_object_vtable NTG_TEXT_VTABLE = {
    .measure_fn = ntg_text_measure_fn,
    .draw_fn = ntg_text_draw_fn,
    .layout_finalize_fn = ntg_text_layout_finalize_fn,
    .deinit_fn = ntg_text_deinit_fn,
    .focus_fn = ntg_text_focus_fn,
    .unfocus_fn = ntg_text_unfocus_fn
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static struct ntg_object_measure measure_nowrap_fn(
        const ntg_text* text_obj,
        const struct ntg_str32_view* rows,
        size_t row_count,
        ntg_orient orient,
        size_t for_size,
        sarena* arena,
        int* out_remeasure)
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
        size_t for_size,
        sarena* arena,
        int* out_remeasure)
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
                    &it_row_wrows, arena, out_remeasure);

            if(it_row_wrow_count == SIZE_MAX)
                return (struct ntg_object_measure) {0};

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
        sarena* arena,
        int* out_remeasure)
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

            it_word_count = ntg_str32_count(rows[i], ' ') + 1;
            it_words = sarena_malloc(arena, sizeof(struct ntg_str32_view) *
                                     it_word_count);
            if(!it_words)
            {
                ntg_set_out(out_remeasure, 1);
                return (struct ntg_object_measure) {0};
            }
            ntg_str32_split(rows[i], ' ', it_words, it_word_count);

            for(j = 0; j < it_word_count; j++)
            {
                
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
                    &it_row_wrows, arena, out_remeasure);

            if(it_row_wrow_count == SIZE_MAX)
                return (struct ntg_object_measure) {0};

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
        sarena* arena,
        int* out_retry)
{
    if(for_size == 0) return 0;

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = sarena_malloc(arena, sizeof(struct ntg_str32_view));
        if(!*out_wrows)
        {
            ntg_set_out(out_retry, 1);
            return SIZE_MAX;
        }

        (*out_wrows)[0] = (struct ntg_str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    (*out_wrows) = sarena_malloc(arena, sizeof(struct ntg_str32_view));
    if(!*out_wrows)
    {
        ntg_set_out(out_retry, 1);
        return SIZE_MAX;
    }
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
        sarena* arena,
        int* out_retry)
{
    if(for_size == 0) return 0;

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = sarena_malloc(arena, sizeof(struct ntg_str32_view));
        if(!*out_wrows)
        {
            ntg_set_out(out_retry, 1);
            return SIZE_MAX;
        }
        (*out_wrows)[0] = (struct ntg_str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    size_t wrow_count = (row.len + for_size - 1) / for_size;
    struct ntg_str32_view* wrows = sarena_malloc(arena,
            wrow_count * sizeof(struct ntg_str32_view));
    if(!wrows)
    {
        ntg_set_out(out_retry, 1);
        return SIZE_MAX;
    }

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
        sarena* arena,
        int* out_retry)
{
    if(for_size == 0) return 0;

    if((row.len == 0) || (row.data == NULL))
    {
        (*out_wrows) = (struct ntg_str32_view*)sarena_malloc(
                arena, sizeof(struct ntg_str32_view));
        if(!*out_wrows)
        {
            ntg_set_out(out_retry, 1);
            return SIZE_MAX;
        }

        (*out_wrows)[0] = (struct ntg_str32_view) {
            .data = row.data,
            .len = 0
        };
        return 1;
    }

    struct ntg_str32_view *words, *wrows;
    size_t word_count = ntg_str32_count(row, ' ') + 1;
    words = sarena_malloc(arena, word_count * sizeof(struct ntg_str32_view));
    if(!words)
    {
        ntg_set_out(out_retry, 1);
        return SIZE_MAX;
    }
    ntg_str32_split(row, ' ', words, word_count);
    size_t wrow_max_count = word_count;

    wrows = sarena_malloc(arena, wrow_max_count * sizeof(struct ntg_str32_view));
    if(!wrows)
    {
        ntg_set_out(out_retry, 1);
        return SIZE_MAX;
    }

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
                it_row_end_word = words[i];

                wrows[wrow_counter] = (struct ntg_str32_view) {
                    .data = it_row_start_word.data,
                    .len = &(it_row_end_word.data[it_row_end_word.len]) -
                        &(it_row_start_word.data[0])
                };

                wrow_counter++;
            }
        }
        else
        {
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

            if(it_word.len < for_size) 
            {
                it_row_len = it_word.len;
                it_row_word_count = 1;

                
                if(i == (word_count - 1))
                {
                    it_row_end_word = words[i];

                    wrows[wrow_counter] = (struct ntg_str32_view) {
                        .data = it_row_start_word.data,
                            .len = &(it_row_end_word.data[it_row_end_word.len]) -
                                &(it_row_start_word.data[0])
                    };
                    wrow_counter++;
                }
            }
            else 
            {
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

static struct ntg_xy calculate_effective_scroll(const ntg_text* text_obj)
{
    struct ntg_xy opts_scroll = text_obj->_scroll;
    struct ntg_xy cont_size = ntg_object_get_size_cont(ntg_obj(text_obj));
    struct ntg_xy cont_nat_size = ntg_object_get_nat_size_cont(ntg_obj(text_obj));
    struct ntg_xy full_size;

    if(text_obj->_opts.wrap == NTG_TEXT_WRAP_NONE)
    {
        full_size = ntg_xy(cont_nat_size.x, cont_nat_size.y);
    }
    else
    {
        if(text_obj->_opts.orient == NTG_ORIENT_H)
            full_size = ntg_xy(cont_size.x, cont_nat_size.y);
        else
            full_size = ntg_xy(cont_nat_size.x, cont_size.y);
    }

    struct ntg_xy scroll = ntg_xy(
        _min2_size(opts_scroll.x, _sub2_size(full_size.x, cont_size.x)),
        _min2_size(opts_scroll.y, _sub2_size(full_size.y, cont_size.y))
    );

    if(text_obj->_opts.wrap != NTG_TEXT_WRAP_NONE)
        scroll.x = 0;

    return scroll;
}


static int trim_text(struct ntg_str* text)
{
    if((text->len == 0) || (text->data == NULL))
        return 0;

    struct ntg_str_view view = ntg_str_get_view(*text, 0);

    size_t word_count = ntg_str_count(view, ' ') + 1;
    struct ntg_str_view* words = calloc(word_count, sizeof(struct ntg_str_view));
    if(!words)
        return NTG_ERR_ALLOC_FAIL;
    word_count = ntg_str_split(view, ' ', words, word_count);

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

static void update_object_bg(ntg_text* text_obj)
{
    if(!text_obj) return;

    struct ntg_vcell cell =
            (text_obj->_opts.bg_mode == NTG_TEXT_BG_FULL) ?
            ntg_vcell_new_full(' ', text_obj->_gfx) :
            ntg_vcell_new_transparent();

    ntg_object_set_base_bg(ntg_obj(text_obj), cell);
}
