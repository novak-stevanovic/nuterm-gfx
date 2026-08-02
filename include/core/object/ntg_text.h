#ifndef NTG_TEXT_H
#define NTG_TEXT_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"
#include "shared/ntg_str.h"

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

enum ntg_text_wrap
{
    NTG_TEXT_WRAP_NONE,
    NTG_TEXT_WRAP_CHAR,
    NTG_TEXT_WRAP_WORD
};

enum ntg_text_line_mode
{
    NTG_TEXT_LINE_ALIGN,
    NTG_TEXT_LINE_JUSTIFY
};

enum ntg_text_bg_mode
{
    NTG_TEXT_BG_FULL,
    NTG_TEXT_BG_FLT
};

enum ntg_text_set_flags
{
    NTG_TEXT_SET_DEFAULT = 0,
    NTG_TEXT_SET_RM_WS = 1 << 0
};

/* ------------------------------------------------------ */

struct ntg_text_opts
{
    ntg_orient orient;
    struct nt_gfx gfx, focused_gfx;
    ntg_text_wrap wrap;
    ntg_text_line_mode line_mode;
    ntg_align prim_align; // active only if NTG_TEXT_LINE_ALIGN
    ntg_align sec_align;
    ntg_text_bg_mode bg_mode;

    size_t indent;
};

NTG_API struct ntg_text_opts
ntg_text_opts_default();

NTG_API bool
ntg_text_opts_are_eql(
        const struct ntg_text_opts* opts1,
        const struct ntg_text_opts* opts2);

/* ------------------------------------------------------ */

struct ntg_text_vtable
{
    void (*post_draw_fn)(
        const ntg_text* text,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena);
};

/* ------------------------------------------------------ */

struct ntg_text
{
    ntg_object __base;

    const struct ntg_text_vtable* __vtable;

    struct ntg_text_opts _opts;
    
    struct nt_gfx _gfx;

    struct
    {
        char* data;
        size_t len;
    } _text;

    struct
    {
        // TODO: add setter, override resize fn
        // Primary axis scroll works only if wrap is disabled.
        struct ntg_xy _scroll;
    };

    struct
    {
        struct ntg_str32 utf32_text;

        size_t utf32_row_count;
        struct ntg_str32_view* utf32_rows;
    } __cache;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */ 
/* OPTS */
/* ------------------------------------------------------ */

NTG_API void
ntg_text_set_opts(ntg_text* text_obj, const struct ntg_text_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API void
ntg_text_set_text_unsafe(
        ntg_text* text_obj,
        const char* text,
        ntg_text_set_flags flags,
        int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_text_set_text(
        ntg_text* text_obj,
        const char* text,
        size_t len,
        ntg_text_set_flags flags,
        int* out_status);

/* ------------------------------------------------------ */
/* SCROLL */
/* ------------------------------------------------------ */

NTG_API void
ntg_text_set_scroll(ntg_text* text_obj, struct ntg_xy scroll);

NTG_API void
ntg_text_scroll(ntg_text* text_obj, struct ntg_dxy scroll_diff);

/* ------------------------------------------------------ */
/* INHERIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_text_init_inherit(
        ntg_text* text_obj,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        int* out_status);

NTG_API void
ntg_text_deinit(ntg_text* text_obj);

NTG_API struct ntg_object_measure
ntg_text_measure_fn(
        const ntg_object* _text_obj,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_text_draw_fn(
        const ntg_object* _text_obj,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_text_deinit_fn(ntg_object* _text_obj);

NTG_API void
ntg_text_focus_fn(ntg_object* _text_obj, ntg_object* old_focused);

NTG_API void
ntg_text_unfocus_fn(ntg_object* _text_obj, ntg_object* new_focused);

NTG_API void
ntg_text_cont_resize_fn(
        ntg_object* _text_obj,
        struct ntg_xy old_size,
        struct ntg_xy new_size);

NTG_API extern const struct ntg_object_vtable NTG_TEXT_VTABLE;

#endif // NTG_TEXT_H
