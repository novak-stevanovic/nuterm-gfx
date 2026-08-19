#ifndef NTG_TEXT_H
#define NTG_TEXT_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"
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
    NTG_TEXT_SET_RM_WS = (1 << 0)
};

/* ------------------------------------------------------ */

struct ntg_text_opts
{
    enum ntg_orient orient;
    struct nt_gfx gfx, focused_gfx;
    enum ntg_text_wrap wrap;
    enum ntg_text_line_mode line_mode;
    enum ntg_align prim_align; // active only if NTG_TEXT_LINE_ALIGN
    enum ntg_align sec_align;
    enum ntg_text_bg_mode bg_mode;

    size_t indent;
};

NTG_API struct ntg_text_opts
ntg_text_opts_default(void);

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
        sarena* arena);
};

/* ------------------------------------------------------ */

struct ntg_text
{
    ntg_object __base;

    const struct ntg_text_vtable* __vtable;

    struct ntg_text_opts _opts;
    
    struct nt_gfx _gfx;

    char* _text;
    size_t _text_len;

    struct ntg_xy _scroll;

    struct ntg_str32 __utf32_text;
    size_t __utf32_row_count;
    struct ntg_str32_view* __utf32_rows;
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

NTG_API int
ntg_text_set_text_unsafe(
        ntg_text* text_obj,
        const char* text,
        uint16_t flags);

/* ------------------------------------------------------ */

NTG_API int
ntg_text_set_text(
        ntg_text* text_obj,
        const char* text,
        size_t len,
        uint16_t flags);

/* ------------------------------------------------------ */
/* SCROLL */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_set_scroll(ntg_text* text_obj, struct ntg_xy scroll);

NTG_API int
ntg_text_scroll(ntg_text* text_obj, struct ntg_dxy scroll_diff);

/* ------------------------------------------------------ */
/* INHERIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_text_init_inherit(
        ntg_text* text_obj,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt);

NTG_API int
ntg_text_deinit(ntg_text* text_obj);

NTG_API void
ntg_text_deinit_void(void* _text);

NTG_API int
ntg_text_measure_fn(
        const ntg_object* _text_obj,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure);

NTG_API int
ntg_text_draw_fn(
        const ntg_object* _text_obj,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout);

NTG_API void
ntg_text_layout_finalize_fn(ntg_object* object, sarena* arena);

NTG_API void
ntg_text_deinit_fn(ntg_object* _text_obj);

NTG_API void
ntg_text_focus_fn(ntg_object* _text_obj);

NTG_API void
ntg_text_unfocus_fn(ntg_object* _text_obj);

NTG_API extern const struct ntg_object_vtable NTG_TEXT_VTABLE;

#endif // NTG_TEXT_H
