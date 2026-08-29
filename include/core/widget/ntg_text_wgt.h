#ifndef NTG_TEXT_WGT_H
#define NTG_TEXT_WGT_H

#include "shared/ntg_shared.h"
#include "core/widget/ntg_widget.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"
#include "shared/ntg_str.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

enum ntg_text_wrap
{
    NTG_TEXT_WRAP_WORD = 0,
    NTG_TEXT_WRAP_CHAR,
    NTG_TEXT_WRAP_NONE
};

enum ntg_text_line_mode
{
    NTG_TEXT_LINE_ALIGN = 0,
    NTG_TEXT_LINE_JUSTIFY
};

enum ntg_text_bg_mode
{
    NTG_TEXT_BG_FULL = 0,
    NTG_TEXT_BG_OVERLAY
};

/* ------------------------------------------------------ */

struct ntg_text_wgt_opts
{
    enum ntg_orient orient;

    /* If NTG_TEXT_BG_OVERLAY bg color is ignored. */
    struct nt_gfx gfx;

    enum ntg_text_wrap wrap;
    enum ntg_text_line_mode line_mode;
    enum ntg_align prim_align; // active only if NTG_TEXT_LINE_ALIGN
    enum ntg_align sec_align;
    enum ntg_text_bg_mode bg_mode;

    size_t indent;
};

static const struct ntg_text_wgt_opts NTG_TEXT_WGT_OPTS_ZERO = {0};

/* ------------------------------------------------------ */

struct ntg_text_wgt_vtable
{
    struct ntg_widget_vtable base;

    void (*post_draw_fn)(
        const ntg_text_wgt* text_wgt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);
};

/* ------------------------------------------------------ */

struct ntg_text_wgt
{
    ntg_widget _base;

    struct
    {
        struct ntg_charvec utf8_text;
        
        struct ntg_str32 utf32_text;
        size_t utf32_row_count;
        struct ntg_str32_view* utf32_rows;
    } priv;

    struct
    {
        struct ntg_text_wgt_opts opts;

        ntg_xy scroll;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */ 
/* OPTS */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_wgt_set_opts(ntg_text_wgt* text_wgt, const struct ntg_text_wgt_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_text_wgt_get_text(const ntg_text_wgt* text_wgt);

NTG_API int
ntg_text_wgt_set_text(ntg_text_wgt* text_wgt, const char* text, size_t len);

NTG_API int
ntg_text_wgt_set_text_cstr(ntg_text_wgt* text_wgt, const char* text);

NTG_API int
ntg_text_wgt_add_text(ntg_text_wgt* text_wgt, const char* text, size_t len);

NTG_API int
ntg_text_wgt_add_text_cstr(ntg_text_wgt* text_wgt, const char* text);

NTG_API int
ntg_text_wgt_rm_text(ntg_text_wgt* text_wgt, size_t count_utf32);

/* ------------------------------------------------------ */
/* SCROLL */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_wgt_set_scroll(ntg_text_wgt* text_wgt, ntg_xy scroll);

NTG_API int
ntg_text_wgt_scroll(ntg_text_wgt* text_wgt, ntg_dxy scroll_diff);

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_wgt_init_inherit(
        ntg_text_wgt* text_wgt,
        const struct ntg_text_wgt_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

NTG_API int
ntg_text_wgt_deinit(ntg_text_wgt* text_wgt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_text_wgt_vtable NTG_TEXT_WGT_VTABLE;

NTG_API void
ntg_text_wgt_deinit_fn(ntg_object* _text_wgt);

NTG_API int
ntg_text_wgt_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena);

NTG_API int
ntg_text_wgt_measure_fn(
        const ntg_widget* _text_wgt,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_text_wgt_draw_fn(
        const ntg_widget* _text_wgt,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

NTG_API void
ntg_text_wgt_cont_resize_fn(ntg_widget* widget, sarena* arena);

NTG_API void
ntg_text_wgt_focus_fn(ntg_widget* _text_wgt);

NTG_API void
ntg_text_wgt_unfocus_fn(ntg_widget* _text_wgt);

#endif // NTG_TEXT_WGT_H
