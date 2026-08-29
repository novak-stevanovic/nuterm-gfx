#ifndef NTG_TEXT_H
#define NTG_TEXT_H

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
    NTG_TEXT_BG_FLT
};

/* ------------------------------------------------------ */

struct ntg_text_opts
{
    enum ntg_orient orient;
    struct nt_gfx gfx;
    enum ntg_text_wrap wrap;
    enum ntg_text_line_mode line_mode;
    enum ntg_align prim_align; // active only if NTG_TEXT_LINE_ALIGN
    enum ntg_align sec_align;
    enum ntg_text_bg_mode bg_mode;

    size_t indent;
};

static const struct ntg_text_opts NTG_TEXT_OPTS_ZERO = {0};

/* ------------------------------------------------------ */

struct ntg_text_vtable
{
    struct ntg_widget_vtable base;

    void (*post_draw_fn)(
        const ntg_text* text,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);
};

/* ------------------------------------------------------ */

struct ntg_text
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
        struct ntg_text_opts opts;

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
ntg_text_set_opts(ntg_text* text_obj, const struct ntg_text_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_text_get_text(const ntg_text* text_obj);

NTG_API int
ntg_text_set_text(ntg_text* text_obj, const char* text, size_t len);

NTG_API int
ntg_text_set_text_unsafe(ntg_text* text_obj, const char* text);

/* ------------------------------------------------------ */
/* SCROLL */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_set_scroll(ntg_text* text_obj, ntg_xy scroll);

NTG_API int
ntg_text_scroll(ntg_text* text_obj, ntg_dxy scroll_diff);

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_init_inherit(
        ntg_text* text_obj,
        const struct ntg_text_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

NTG_API int
ntg_text_deinit(ntg_text* text_obj);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_text_vtable NTG_TEXT_VTABLE;

NTG_API void
ntg_text_deinit_fn(ntg_object* _text_obj);

NTG_API int
ntg_text_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena);

NTG_API int
ntg_text_measure_fn(
        const ntg_widget* _text_obj,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_text_draw_fn(
        const ntg_widget* _text_obj,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

NTG_API void
ntg_text_cont_resize_fn(ntg_widget* widget, sarena* arena);

NTG_API void
ntg_text_focus_fn(ntg_widget* _text_obj);

NTG_API void
ntg_text_unfocus_fn(ntg_widget* _text_obj);

#endif // NTG_TEXT_H
