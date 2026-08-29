#include "ntg.h"
#include <math.h>
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"
#include <string.h>
#include <assert.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

static inline struct ntg_vcell
determine_bg_vcell(const ntg_text_wgt* text_wgt)
{
    struct nt_gfx gfx = text_wgt->ro.opts.gfx;
    enum ntg_text_bg_mode bg_mode = text_wgt->ro.opts.bg_mode;
    if(bg_mode == NTG_TEXT_BG_FULL)
        return ntg_vcell_new_full_bg(gfx.fg, gfx.bg, gfx.style & NT_STYLE_REVERSE);
    else
        return ntg_vcell_new_overlay_bg(gfx.fg, gfx.style & NT_STYLE_REVERSE);
}


/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

#define DEFAULT_SIZE 1

static int get_wrows_nowrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena,
        size_t* out_count);

static int get_wrows_wrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena,
        size_t* out_count);

static int get_wrows_wwrap(
        struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_rows,
        sarena* arena,
        size_t* out_count);

static int measure_nowrap_fn(
        const ntg_text_wgt* text_wgt,
        const struct ntg_str32_view* rows,
        size_t row_count,
        enum ntg_orient orient,
        size_t for_size,
        sarena* arena,
        struct ntg_widget_measure* out_measure);

static int measure_wrap_fn(
        const ntg_text_wgt* text_wgt,
        const struct ntg_str32_view* rows,
        size_t row_count,
        enum ntg_orient orient,
        size_t for_size,
        sarena* arena,
        struct ntg_widget_measure* out_measure);

static int measure_wwrap_fn(
        const ntg_text_wgt* text_wgt,
        const struct ntg_str32_view* rows,
        size_t row_count,
        enum ntg_orient orient,
        size_t for_size,
        sarena* arena,
        struct ntg_widget_measure* out_measure);

static ntg_xy calculate_effective_scroll(const ntg_text_wgt* text_wgt);

static void update_widget_bg(ntg_text_wgt* text_wgt);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_text_wgt_set_opts(ntg_text_wgt* text_wgt, const struct ntg_text_wgt_opts* opts)
{
    if(!text_wgt) return NTG_ERR_INV_ARG;

    struct ntg_text_wgt_opts opts_final = (opts ? (*opts) : NTG_TEXT_WGT_OPTS_ZERO);

    uint16_t dirty = 0;

    if ((text_wgt->ro.opts.orient != opts_final.orient) ||
       (text_wgt->ro.opts.line_mode != opts_final.line_mode) ||
       (text_wgt->ro.opts.wrap != opts_final.wrap) ||
       (text_wgt->ro.opts.indent != opts_final.indent))
    {
        dirty |= NTG_WIDGET_DIRTY_FULL;
    }

    if(!nt_gfx_are_eql(text_wgt->ro.opts.gfx, opts_final.gfx) ||
       (text_wgt->ro.opts.prim_align != opts_final.prim_align) ||
       (text_wgt->ro.opts.sec_align != opts_final.sec_align) ||
       (text_wgt->ro.opts.bg_mode != opts_final.bg_mode))
    {
        dirty |= NTG_WIDGET_DIRTY_DRAW;
    }

    if(!dirty) return 0;

    /* Set new values */
    
    text_wgt->ro.opts = opts_final;

    update_widget_bg(text_wgt);

    ntg_widget_mark_dirty((ntg_widget*)text_wgt, dirty);

    return 0;
}

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

struct ntg_str_view ntg_text_wgt_get_text(const ntg_text_wgt* text_wgt)
{
    if(!text_wgt) return (struct ntg_str_view) {0};

    return (struct ntg_str_view) {
        .data = text_wgt->priv.utf8_text.data,
        .len = text_wgt->priv.utf8_text.size - 1 /* account for \0 */
    };
}

int ntg_text_wgt_set_text(ntg_text_wgt* text_wgt, const char* text, size_t len)
{
    if(!text_wgt) return NTG_ERR_INV_ARG;
    if(!text && len) return NTG_ERR_INV_ARG;
    if(len > (NTG_SIZE_MAX * NTG_SIZE_MAX)) return NTG_ERR_NO_CAP;

    if(!text || !len)
    {
        text = "";
        len = 0;
    }

    int status;

    status = uc_utf8_to_utf32((const uint8_t*)text, len, NULL, SIZE_MAX, 0, NULL);
    switch(status)
    {
        case 0:
            break;
        case UC_ERR_INV_SIZE:
        case UC_ERR_OVERLONG:
        case UC_ERR_SURROGATE:
        case UC_ERR_INV_CP:
        case UC_ERR_INV_SB:
        case UC_ERR_INV_CB:
            return NTG_ERR_BAD_UTF8;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    struct ntg_charvec* text_vec = &text_wgt->priv.utf8_text;
    struct ntg_str_view curr_text = ntg_text_wgt_get_text(text_wgt);

    /* Assume it's worth comparing because if text is different the len is probably different */
    if((text_vec->size > 0) && (curr_text.len == len) &&
        ((len == 0) || (memcmp(curr_text.data, text, len) == 0)))
    {
        return 0; /* Same text */
    }

    size_t req_cap = len + 1; /* include \0 */
    if(req_cap > text_vec->cap)
    {
        status = ntg_charvec_prealloc(text_vec, req_cap - text_vec->cap);
        switch(status)
        {
            case 0: break;
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;
            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    /* Empty but don't shrink, O(1) */
    ntg_charvec_empty(text_vec);

    /* These must not fail because enough space was preallocated */
    status = ntg_charvec_pushb_many(text_vec, text, len);
    if(status) assert(0);
    status = ntg_charvec_pushb(text_vec, '\0');
    if(status) assert(0);

    /* Save space if needed */
    ntg_charvec_shrink(text_vec);

    ntg_widget_mark_dirty(ntg_wgt(text_wgt),
    NTG_WIDGET_DIRTY_PREPARE | NTG_WIDGET_DIRTY_FULL);

    return 0;
}

int ntg_text_wgt_set_text_cstr(ntg_text_wgt* text_wgt, const char* text)
{
    if(!text_wgt || !text)
        return NTG_ERR_INV_ARG;

    return ntg_text_wgt_set_text(text_wgt, text, strlen(text));
}

int ntg_text_wgt_add_text(ntg_text_wgt* text_wgt, const char* text, size_t len)
{
    if(!text_wgt) return NTG_ERR_INV_ARG;
    if(!text && len) return NTG_ERR_INV_ARG;

    if(!text || !len) return 0;

    struct ntg_charvec* text_vec = &text_wgt->priv.utf8_text;
    struct ntg_str_view curr_text = ntg_text_wgt_get_text(text_wgt);

    if((curr_text.len + len) > (NTG_SIZE_MAX * NTG_SIZE_MAX))
        return NTG_ERR_NO_CAP;

    int status;

    status = uc_utf8_to_utf32((const uint8_t*)text, len, NULL, SIZE_MAX, 0, NULL);
    switch(status)
    {
        case 0:
            break;
        case UC_ERR_INV_SIZE:
        case UC_ERR_OVERLONG:
        case UC_ERR_SURROGATE:
        case UC_ERR_INV_CP:
        case UC_ERR_INV_SB:
        case UC_ERR_INV_CB:
            return NTG_ERR_BAD_UTF8;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    size_t req_cap = curr_text.len + len + 1;
    if(req_cap > text_vec->cap)
    {
        status = ntg_charvec_prealloc(text_vec, req_cap - text_vec->cap);
        switch(status)
        {
            case 0: break;
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;
            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    /* Pop \0 */
    ntg_charvec_popb(&text_wgt->priv.utf8_text);

    /* Must not fail because of preallocation */
    ntg_charvec_pushb_many(text_vec, text, len);
    ntg_charvec_pushb(text_vec, '\0');

    ntg_widget_mark_dirty(ntg_wgt(text_wgt),
    NTG_WIDGET_DIRTY_PREPARE | NTG_WIDGET_DIRTY_FULL);

    return 0;

}

int ntg_text_wgt_add_text_cstr(ntg_text_wgt* text_wgt, const char* text)
{
    if(!text_wgt || !text)
        return NTG_ERR_INV_ARG;

    return ntg_text_wgt_add_text(text_wgt, text, strlen(text));
}

int ntg_text_wgt_rm_text(ntg_text_wgt* text_wgt, size_t count_utf32)
{
    if(!text_wgt)
        return NTG_ERR_INV_ARG;

    if(count_utf32 == 0) return 0;

    struct ntg_charvec* text_vec = &text_wgt->priv.utf8_text;
    struct ntg_str_view curr_text = ntg_text_wgt_get_text(text_wgt);

    /* Figure out how many UTF-8 units */

    size_t count = 0;
    int status;

    size_t curr_count = 1;
    while(true)
    {
        if(count_utf32 == 0) break;
        if(count == curr_text.len) break; /* Remove all text */
        if((count + curr_count) > curr_text.len) /* All text but still decoding so invalid UTF-8 */
            return NTG_ERR_UNEXPECTED;

        status = uc_utf8_to_utf32_single(
                (const uint8_t*)((curr_text.data + curr_text.len) - count - curr_count),
                curr_count,
                0,
                NULL);
        switch(status)
        {
            case 0:
                --count_utf32;
                count += curr_count;
                curr_count = 1;
                break;
            case UC_ERR_INV_SIZE:
            case UC_ERR_OVERLONG:
            case UC_ERR_SURROGATE:
            case UC_ERR_INV_CP:
            case UC_ERR_INV_SB:
            case UC_ERR_INV_CB:
                ++curr_count;
                if(curr_count > 4)
                    return NTG_ERR_UNEXPECTED;
                break;
            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    /* Pop \0 */
    ntg_charvec_popb(&text_wgt->priv.utf8_text);

    ntg_charvec_popb_many(&text_wgt->priv.utf8_text, count);

    ntg_charvec_pushb(&text_wgt->priv.utf8_text, '\0');

    ntg_charvec_shrink(&text_wgt->priv.utf8_text);

    ntg_widget_mark_dirty(ntg_wgt(text_wgt),
    NTG_WIDGET_DIRTY_PREPARE | NTG_WIDGET_DIRTY_FULL);

    return 0;
}

/* ------------------------------------------------------ */
/* SCROLL */
/* ------------------------------------------------------ */

int ntg_text_wgt_set_scroll(ntg_text_wgt* text_wgt, ntg_xy scroll)
{
    if(!text_wgt) return NTG_ERR_INV_ARG;

    text_wgt->ro.scroll = scroll;

    text_wgt->ro.scroll = calculate_effective_scroll(text_wgt);

    ntg_widget_mark_dirty(ntg_wgt(text_wgt), NTG_WIDGET_DIRTY_DRAW);

    return 0;
}

int ntg_text_wgt_scroll(ntg_text_wgt* text_wgt, ntg_dxy scroll_diff)
{
    if(!text_wgt) return NTG_ERR_INV_ARG;
    if((scroll_diff.ro.x == 0) && (scroll_diff.ro.y == 0))
        return 0;

    ntg_dxy curr_scroll_dxy = ntg_dxy_from_xy(text_wgt->ro.scroll);

    ntg_dxy scroll_dxy = ntg_dxy_add(curr_scroll_dxy, scroll_diff);

    ntg_xy scroll = ntg_xy_from_dxy(scroll_dxy);

    ntg_text_wgt_set_scroll(text_wgt, scroll);

    return 0;
}

/* ------------------------------------------------------ */
/* INHERIT */
/* ------------------------------------------------------ */

int ntg_text_wgt_init_inherit(
        ntg_text_wgt* text_wgt,
        const struct ntg_text_wgt_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!text_wgt || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_TEXT_WGT))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_widget_init_inherit(ntg_wgt(text_wgt), &vtable->base, type, layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(text_wgt);

    status = ntg_text_wgt_set_text_cstr(text_wgt, "");
    if(status != 0)
    {
        ntg_widget_deinit((ntg_widget*)text_wgt);
        return status;
    }

    return 0;
}

int ntg_text_wgt_deinit(ntg_text_wgt* text_wgt)
{
    if(!text_wgt) return NTG_ERR_INV_ARG;

    ntg_charvec_deinit(&text_wgt->priv.utf8_text);
    if(text_wgt->priv.utf32_text.data)
        free(text_wgt->priv.utf32_text.data);
    if(text_wgt->priv.utf32_rows)
        free(text_wgt->priv.utf32_rows);

    ntg_object_zero(text_wgt);

    ntg_widget_deinit((ntg_widget*)text_wgt);

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_text_wgt_vtable NTG_TEXT_WGT_VTABLE = {
    .base = {
        .base = {
            .deinit_fn = ntg_text_wgt_deinit_fn
        },
        .measure_fn = ntg_text_wgt_measure_fn,
        .focus_fn = ntg_text_wgt_focus_fn,
        .unfocus_fn = ntg_text_wgt_unfocus_fn,
        .draw_fn = ntg_text_wgt_draw_fn,
        .layout_prepare_fn = ntg_text_wgt_layout_prepare_fn,
        .resize_cont_fn = ntg_text_wgt_cont_resize_fn
    }
};

void ntg_text_wgt_deinit_fn(ntg_object* _text_wgt)
{
    ntg_text_wgt_deinit((ntg_text_wgt*)_text_wgt);
}

int ntg_text_wgt_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena)
{
    (void)layout_dt;
    (void)arena;

    ntg_text_wgt* text_wgt = ntg_txt(widget);

    struct ntg_str_view curr_text = ntg_text_wgt_get_text(text_wgt);
    if(curr_text.len == 0)
    {
        free(text_wgt->priv.utf32_text.data);
        free(text_wgt->priv.utf32_rows);
        text_wgt->priv.utf32_text = (struct ntg_str32) {0};
        text_wgt->priv.utf32_row_count = 0;
        text_wgt->priv.utf32_rows = NULL;

        return 0;
    }

    /* Determine UTF32 string width first, to conserve memory */

    size_t width = 0;
    int status = uc_utf8_to_utf32((uint8_t*)curr_text.data,
            curr_text.len, NULL, 0, 0, &width);
    if(status != 0) return NTG_ERR_UTF_CONV_FAIL;

    /* UTF8 len is not 0 but this is 0? */
    if(width == 0) return NTG_ERR_UNEXPECTED;

    uint32_t* new_utf32_text = malloc(sizeof(uint32_t) * width);
    if(!new_utf32_text) return NTG_ERR_ALLOC_FAIL;

    /* Convert to UTF32 */

    status = uc_utf8_to_utf32(
            (uint8_t*)curr_text.data, curr_text.len,
            new_utf32_text, width, 0, NULL);
    if(status != 0)
    {
        free(new_utf32_text);
        return NTG_ERR_UNEXPECTED;
    }

    struct ntg_str32 utf32_text = {
        .data = new_utf32_text,
        .len = width
    };

    size_t row_count = ntg_str32_count(utf32_text.data, utf32_text.len, '\n') + 1;
    struct ntg_str32_view* new_rows = malloc(sizeof(struct ntg_str32_view) * row_count);
    if(!new_rows)
    {
        free(new_utf32_text);
        return NTG_ERR_ALLOC_FAIL;
    }

    ntg_str32_split(utf32_text.data, utf32_text.len, '\n', new_rows, row_count);

    /* Free old UTF32 data */

    free(text_wgt->priv.utf32_text.data);
    free(text_wgt->priv.utf32_rows);

    /* Update UTF32 data */

    text_wgt->priv.utf32_text = utf32_text;
    text_wgt->priv.utf32_rows = new_rows;
    text_wgt->priv.utf32_row_count = row_count;

    return 0;
}

int ntg_text_wgt_measure_fn(
        const ntg_widget* _text_wgt,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure)
{
    (void)layout_dt;
    (void)relayout;

    if(!_text_wgt || !out_measure)
        return NTG_ERR_INV_ARG;

    *out_measure = (struct ntg_widget_measure) {0};

    const ntg_text_wgt* text_wgt = (const ntg_text_wgt*)_text_wgt;

    size_t for_size = ntg_widget_get_for_size_cont(_text_wgt, orient);
    size_t row_count = text_wgt->priv.utf32_row_count;
    const struct ntg_str32_view* rows = text_wgt->priv.utf32_rows;
    if((row_count == 0) || !rows)
        return 0;

    switch(text_wgt->ro.opts.wrap)
    {
        case NTG_TEXT_WRAP_NONE:
            return measure_nowrap_fn(text_wgt, rows, row_count, orient,
                    for_size, arena, out_measure);
        case NTG_TEXT_WRAP_CHAR:
            return measure_wrap_fn(text_wgt, rows, row_count, orient,
                    for_size, arena, out_measure);
        case NTG_TEXT_WRAP_WORD:
            return measure_wwrap_fn(text_wgt, rows, row_count, orient,
                    for_size, arena, out_measure);
        default:
            return NTG_ERR_UNEXPECTED;
    }
}

int ntg_text_wgt_draw_fn(
        const ntg_widget* _text_wgt,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)layout_dt;
    ntg_xy cont_size = ntg_widget_get_size_cont(_text_wgt);
    if(ntg_xy_is_zero_any(cont_size)) return 0;

    const ntg_text_wgt* text_wgt = (const ntg_text_wgt*)_text_wgt;

    ntg_xy cont_nat_size = ntg_widget_get_nat_size_cont(_text_wgt);

    enum ntg_orient orient = text_wgt->ro.opts.orient;
    enum ntg_text_wrap wrap = text_wgt->ro.opts.wrap;
    size_t indent = text_wgt->ro.opts.indent;
    enum ntg_text_line_mode line_mode = text_wgt->ro.opts.line_mode;
    enum ntg_align prim_align = text_wgt->ro.opts.prim_align;
    enum ntg_text_bg_mode bg_mode = text_wgt->ro.opts.bg_mode;
    struct nt_gfx gfx = text_wgt->ro.opts.gfx;

    /* Determine full size */

    ntg_xy full_size;

    if(wrap == NTG_TEXT_WRAP_NONE)
    {
        /*
        full_size = ntg_xy(
            _max2_size(cont_nat_size.ro.x, cont_size.ro.x),
            _max2_size(cont_nat_size.ro.y, cont_size.ro.y));
        */
        full_size = ntg_xy_new(cont_nat_size.ro.x, cont_nat_size.ro.y);
    }
    else
    {
        if(orient == NTG_ORIENT_H)
            full_size = ntg_xy_new(cont_size.ro.x, cont_nat_size.ro.y);
        else
            full_size = ntg_xy_new(cont_nat_size.ro.x, cont_size.ro.y);
    }

    if(ntg_xy_is_zero_any(full_size)) return 0;

    ntg_xy full_size_adj =
        (orient == NTG_ORIENT_H) ?
        full_size :
        ntg_xy_transpose(full_size);

    size_t i, j, k, m;
    size_t full_size_prod = full_size_adj.ro.x * full_size_adj.ro.y;
    uint32_t* full_buff = sarena_malloc(arena, sizeof(uint32_t) * full_size_prod);
    if(!full_buff)
        return NTG_ERR_ALLOC_FAIL;

    for(i = 0; i < full_size_prod; i++) full_buff[i] = 0;

    size_t row_count = text_wgt->priv.utf32_row_count;
    const struct ntg_str32_view* rows = text_wgt->priv.utf32_rows;

    size_t capped_indent = ntg_min2_size(indent, ntg_sub2_size(full_size_adj.ro.x, 1));
    
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
        int _status;
        switch(wrap)
        {
            case NTG_TEXT_WRAP_NONE:
                _status = get_wrows_nowrap(rows[i], full_size_adj.ro.x,
                        &_it_wrows, arena, &_it_wrows_count);
                break;
            case NTG_TEXT_WRAP_CHAR:
                _status = get_wrows_wrap(rows[i], full_size_adj.ro.x,
                        &_it_wrows, arena, &_it_wrows_count);
                break;
            case NTG_TEXT_WRAP_WORD:
                _status = get_wrows_wwrap(rows[i], full_size_adj.ro.x,
                        &_it_wrows, arena, &_it_wrows_count);
                break;
            default:
                return NTG_ERR_UNEXPECTED;
        }

        if(_status != 0)
            return _status;

        for(j = 0; j < _it_wrows_count; j++)
        {
            if(cont_i >= full_size_adj.ro.y) break;
            
            _it_wrows[j].len = ntg_min2_size(_it_wrows[j].len, full_size_adj.ro.x);

            if(line_mode == NTG_TEXT_LINE_ALIGN)
            {
                it_row_align_indent = ntg_align_offset_size(
                        full_size_adj.ro.x,
                        _it_wrows[j].len,
                        prim_align);
            }
            else
                it_row_align_indent = 0;
            
            it_row_effective_indent = (j == 0) ?
                ntg_max2_size(capped_indent, it_row_align_indent) :
                it_row_align_indent;
            cont_j = it_row_effective_indent;

            it_wrow_space_counter = 0;
            it_wrow_space_count = ntg_str32_count(_it_wrows[j].data, _it_wrows[j].len, ' ');
            it_wrow_cont_space = _it_wrows[j].len + it_row_effective_indent;
            it_wrow_extra_space = ntg_sub2_size(full_size_adj.ro.x, it_wrow_cont_space);
            for(k = 0; k < _it_wrows[j].len; k++)
            {
                if(_it_wrows[j].data[k] == ' ')
                {
                    if((j < (_it_wrows_count - 1)) && line_mode == NTG_TEXT_LINE_JUSTIFY)
                    {
                        size_t space_justified_count = (it_wrow_extra_space / it_wrow_space_count) +
                            (it_wrow_space_counter < (it_wrow_extra_space % it_wrow_space_count));

                        for(m = 0; m < space_justified_count; m++)
                        {
                            it_idx = (full_size_adj.ro.x * cont_i) + (cont_j + m);
                            full_buff[it_idx] = ' ';
                        }
                        cont_j += space_justified_count;
                    }
                    it_wrow_space_counter++;
                }
                if(cont_j >= full_size_adj.ro.x) break; 

                it_idx = (full_size_adj.ro.x * cont_i) + cont_j;

                if(it_idx >= full_size_prod)
                    continue;

                full_buff[it_idx] = _it_wrows[j].data[k];

                cont_j++;
            }

            cont_i++;

        }
    }

    /* Viewport size is content size */

    ntg_xy scroll = calculate_effective_scroll(text_wgt);

    ntg_xy scroll_adj = 
        (orient == NTG_ORIENT_H) ?
        scroll :
        ntg_xy_transpose(scroll);

    ntg_xy src_xy;
    ntg_xy dst_xy;
    struct ntg_vcell it_cell;
    uint32_t it_cont;

    for(i = 0; i < cont_size.ro.y; i++)
    {
        for(j = 0; j < cont_size.ro.x; j++)
        {
            src_xy = ntg_xy_new(scroll_adj.ro.x + j, scroll_adj.ro.y + i);

            if((src_xy.ro.x < full_size_adj.ro.x) && (src_xy.ro.y < full_size_adj.ro.y))
            {
                it_cont = full_buff[full_size_adj.ro.x * src_xy.ro.y + src_xy.ro.x];

                /* Determine vcell based on content, gfx, bg mode */

                if(it_cont != 0)
                {
                    if(bg_mode == NTG_TEXT_BG_FULL)
                        it_cell = ntg_vcell_new_full(it_cont, gfx);
                    else
                        it_cell = ntg_vcell_new_overlay(it_cont, gfx.fg, gfx.style);
                }
                else
                    it_cell = determine_bg_vcell(text_wgt);
            }
            else /* Viewport bigger than content area */
                it_cell = determine_bg_vcell(text_wgt);

            dst_xy = (orient == NTG_ORIENT_H) ? ntg_xy_new(j, i) : ntg_xy_new(i, j);

            ntg_widget_tmp_draw_set(out_drawing, it_cell, dst_xy);
        }
    }

    if(ntg_txt_vtbl(text_wgt) && ntg_txt_vtbl(text_wgt)->post_draw_fn)
        ntg_txt_vtbl(text_wgt)->post_draw_fn(text_wgt, out_drawing, arena);

    return 0;
}

void ntg_text_wgt_cont_resize_fn(ntg_widget* widget, sarena* arena)
{
    (void)arena;

    ntg_text_wgt* text_wgt = ntg_txt(widget);

    text_wgt->ro.scroll = calculate_effective_scroll(text_wgt);
}

void ntg_text_wgt_focus_fn(ntg_widget* _text_wgt)
{
    (void)_text_wgt;
}

void ntg_text_wgt_unfocus_fn(ntg_widget* _text_wgt)
{
    (void)_text_wgt;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static int measure_nowrap_fn(
        const ntg_text_wgt* text_wgt,
        const struct ntg_str32_view* rows,
        size_t row_count,
        enum ntg_orient orient,
        size_t for_size,
        sarena* arena,
        struct ntg_widget_measure* out_measure)
{
    (void)for_size;
    (void)arena;

    if(!out_measure)
        return NTG_ERR_INV_ARG;

    size_t indent = text_wgt->ro.opts.indent;
    size_t text_orient = text_wgt->ro.opts.orient;

    if(text_orient == orient)
    {
        size_t i;
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].len == 0) continue;
            max_row_len = ntg_max2_size(max_row_len, rows[i].len + indent);
        }

        *out_measure = (struct ntg_widget_measure) {
            .min_size = ntg_max2_size(max_row_len, DEFAULT_SIZE),
            .nat_size = ntg_max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }
    else
    {
        *out_measure = (struct ntg_widget_measure) {
            .min_size = ntg_max2_size(row_count, DEFAULT_SIZE),
            .nat_size = ntg_max2_size(row_count, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }

    return 0;
}

static int measure_wrap_fn(
        const ntg_text_wgt* text_wgt,
        const struct ntg_str32_view* rows,
        size_t row_count,
        enum ntg_orient orient,
        size_t for_size,
        sarena* arena,
        struct ntg_widget_measure* out_measure)
{
    if(!out_measure)
        return NTG_ERR_INV_ARG;

    size_t indent = text_wgt->ro.opts.indent;
    size_t text_orient = text_wgt->ro.opts.orient;
    size_t i;

    if(text_orient == orient)
    {
        size_t max_row_len = 0;
        for(i = 0; i < row_count; i++)
        {
            if(rows[i].len == 0) continue;
            max_row_len = ntg_max2_size(max_row_len, rows[i].len + indent);
        }

        *out_measure = (struct ntg_widget_measure) {
            .min_size = DEFAULT_SIZE,
            .nat_size = ntg_max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
        return 0;
    }

    size_t row_counter = 0;
    struct ntg_str32_view* it_row_wrows;
    size_t it_row_wrow_count;
    for(i = 0; i < row_count; i++)
    {
        int status = get_wrows_wrap(rows[i], for_size, &it_row_wrows,
                arena, &it_row_wrow_count);
        if(status != 0)
            return status;

        row_counter += it_row_wrow_count;
    }

    *out_measure = (struct ntg_widget_measure) {
        .min_size = ntg_max2_size(row_counter, DEFAULT_SIZE),
        .nat_size = ntg_max2_size(row_counter, DEFAULT_SIZE),
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
    return 0;
}

static int measure_wwrap_fn(
        const ntg_text_wgt* text_wgt,
        const struct ntg_str32_view* rows,
        size_t row_count,
        enum ntg_orient orient,
        size_t for_size,
        sarena* arena,
        struct ntg_widget_measure* out_measure)
{
    if(!out_measure)
        return NTG_ERR_INV_ARG;

    size_t indent = text_wgt->ro.opts.indent;
    size_t text_orient = text_wgt->ro.opts.orient;
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

            max_row_len = ntg_max2_size(max_row_len, rows[i].len + indent);
            it_word_count = ntg_str32_count(rows[i].data, rows[i].len, ' ') + 1;
            it_words = sarena_malloc(arena,
                    sizeof(struct ntg_str32_view) * it_word_count);
            if(!it_words)
                return NTG_ERR_ALLOC_FAIL;

            ntg_str32_split(rows[i].data, rows[i].len, ' ', it_words, it_word_count);
            for(j = 0; j < it_word_count; j++)
            {
                j_word_adj_indent = (j == 0) ? indent : 0;
                max_word_len = ntg_max2_size(max_word_len,
                        it_words[j].len + j_word_adj_indent);
            }
        }

        *out_measure = (struct ntg_widget_measure) {
            .min_size = ntg_max2_size(max_word_len, DEFAULT_SIZE),
            .nat_size = ntg_max2_size(max_row_len, DEFAULT_SIZE),
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
        return 0;
    }

    size_t row_counter = 0;
    struct ntg_str32_view* it_row_wrows;
    size_t it_row_wrow_count;
    for(i = 0; i < row_count; i++)
    {
        int status = get_wrows_wwrap(rows[i], for_size, &it_row_wrows,
                arena, &it_row_wrow_count);
        if(status != 0)
            return status;

        row_counter += it_row_wrow_count;
    }

    *out_measure = (struct ntg_widget_measure) {
        .min_size = ntg_max2_size(row_counter, DEFAULT_SIZE),
        .nat_size = ntg_max2_size(row_counter, DEFAULT_SIZE),
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
    return 0;
}

static int get_wrows_nowrap(
        const struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_wrows,
        sarena* arena,
        size_t* out_count)
{
    if(!out_wrows || !out_count)
        return NTG_ERR_INV_ARG;

    *out_wrows = NULL;
    *out_count = 0;
    if(for_size == 0)
        return 0;

    struct ntg_str32_view* wrows = sarena_malloc(
            arena, sizeof(struct ntg_str32_view));
    if(!wrows)
        return NTG_ERR_ALLOC_FAIL;

    wrows[0] = (struct ntg_str32_view) {
        .data = row.data,
        .len = ((row.len == 0) || !row.data) ? 0 : ntg_min2_size(for_size, row.len)
    };

    *out_wrows = wrows;
    *out_count = 1;
    return 0;
}

static int get_wrows_wrap(
        const struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_wrows,
        sarena* arena,
        size_t* out_count)
{
    if(!out_wrows || !out_count)
        return NTG_ERR_INV_ARG;

    *out_wrows = NULL;
    *out_count = 0;
    if(for_size == 0)
        return 0;

    if((row.len == 0) || !row.data)
        return get_wrows_nowrap(row, for_size, out_wrows, arena, out_count);

    size_t wrow_count = (row.len + for_size - 1) / for_size;
    struct ntg_str32_view* wrows = sarena_malloc(
            arena, wrow_count * sizeof(struct ntg_str32_view));
    if(!wrows)
        return NTG_ERR_ALLOC_FAIL;

    size_t i;
    size_t it_start = 0, it_end;
    for(i = 0; i < wrow_count; i++)
    {
        it_end = it_start + ntg_min2_size(for_size, row.len - it_start);
        wrows[i] = (struct ntg_str32_view) {
            .data = &(row.data[it_start]),
            .len = it_end - it_start
        };
        it_start = it_end;
    }

    *out_wrows = wrows;
    *out_count = wrow_count;
    return 0;
}

static int get_wrows_wwrap(
        const struct ntg_str32_view row,
        size_t for_size,
        struct ntg_str32_view** out_wrows,
        sarena* arena,
        size_t* out_count)
{
    if(!out_wrows || !out_count)
        return NTG_ERR_INV_ARG;

    *out_wrows = NULL;
    *out_count = 0;
    if(for_size == 0)
        return 0;

    if((row.len == 0) || !row.data)
        return get_wrows_nowrap(row, for_size, out_wrows, arena, out_count);

    size_t word_count = ntg_str32_count(row.data, row.len, ' ') + 1;
    struct ntg_str32_view* words = sarena_malloc(
            arena, word_count * sizeof(struct ntg_str32_view));
    if(!words)
        return NTG_ERR_ALLOC_FAIL;
    ntg_str32_split(row.data, row.len, ' ', words, word_count);

    struct ntg_str32_view* wrows = sarena_malloc(
            arena, word_count * sizeof(struct ntg_str32_view));
    if(!wrows)
        return NTG_ERR_ALLOC_FAIL;

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
            it_row_len += effective_space + it_word.len;
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

    *out_wrows = wrows;
    *out_count = wrow_counter;
    return 0;
}

static ntg_xy calculate_effective_scroll(const ntg_text_wgt* text_wgt)
{
    ntg_xy opts_scroll = text_wgt->ro.scroll;
    ntg_xy cont_size = ntg_widget_get_size_cont(ntg_wgt(text_wgt));
    ntg_xy cont_nat_size = ntg_widget_get_nat_size_cont(ntg_wgt(text_wgt));
    ntg_xy full_size;

    if(text_wgt->ro.opts.wrap == NTG_TEXT_WRAP_NONE)
    {
        full_size = ntg_xy_new(cont_nat_size.ro.x, cont_nat_size.ro.y);
    }
    else
    {
        if(text_wgt->ro.opts.orient == NTG_ORIENT_H)
            full_size = ntg_xy_new(cont_size.ro.x, cont_nat_size.ro.y);
        else
            full_size = ntg_xy_new(cont_nat_size.ro.x, cont_size.ro.y);
    }

    ntg_xy scroll = ntg_xy_new(
        ntg_min2_size(opts_scroll.ro.x, ntg_sub2_size(full_size.ro.x, cont_size.ro.x)),
        ntg_min2_size(opts_scroll.ro.y, ntg_sub2_size(full_size.ro.y, cont_size.ro.y))
    );

    if(text_wgt->ro.opts.wrap != NTG_TEXT_WRAP_NONE)
        scroll = ntg_xy_set_x(scroll, 0);

    return scroll;
}

static void update_widget_bg(ntg_text_wgt* text_wgt)
{
    if(!text_wgt) return;

    ntg_widget_set_base_bg(ntg_wgt(text_wgt), determine_bg_vcell(text_wgt));
}
