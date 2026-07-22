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

enum ntg_text_mode
{
    NTG_TEXT_ALIGN,
    NTG_TEXT_JUSTIFY
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
    ntg_text_mode text_mode;
    ntg_align prim_align; // Active only if NTG_TEXT_ALIGN
    ntg_align sec_align;
    ntg_text_bg_mode bg_mode;
    ntg_text_wrap wrap;
    size_t indent;
};

/* Creates horizontal text defaults with default graphics, aligned text, full
 * background, start alignment, no wrapping, automatic trimming, and no
 * indentation. */
NTG_API struct ntg_text_opts
ntg_text_opts_def();

/* Compares two text option values. Pointer identity counts as equal; otherwise
 * a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_text_opts_are_eql(
        const struct ntg_text_opts* opts1,
        const struct ntg_text_opts* opts2);

/* ------------------------------------------------------ */

struct ntg_text
{
    ntg_object __base;

    struct ntg_text_opts _opts;
    
    struct nt_gfx _gfx;

    struct
    {
        char* data;
        size_t len;
    } _text;

    struct
    {
        struct ntg_str32 utf32_text;

        size_t utf32_row_count;
        struct ntg_str32_view* utf32_rows;
    } _cache;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Updates text orientation, graphics, alignment, wrapping, trimming, and
 * indentation. A `NULL` options pointer applies defaults. */
NTG_API void
ntg_text_set_opts(ntg_text* text_obj, const struct ntg_text_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

/* Copies a null-terminated UTF-8 string into the text object.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `text_obj` or `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: a text or conversion buffer cannot be allocated.
 * - `NTG_ERR_UTF_CONV`: UTF-8 to UTF-32 conversion fails. */
NTG_API void
ntg_text_set_text_unsafe(
        ntg_text* text_obj,
        const char* text,
        ntg_text_set_flags flags,
        int* out_status);

/* ------------------------------------------------------ */

/* Copies exactly `len` bytes from a UTF-8 buffer into the text object. The
 * input does not need to be null-terminated.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `text_obj` or `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: a text or conversion buffer cannot be allocated.
 * - `NTG_ERR_UTF_CONV`: UTF-8 to UTF-32 conversion fails. */
NTG_API void
ntg_text_set_text(
        ntg_text* text_obj,
        const char* text,
        size_t len,
        ntg_text_set_flags flags,
        int* out_status);

NTG_API void
ntg_text_init_inherit(
        ntg_text* text_obj,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

NTG_API void
ntg_text_deinit(ntg_text* text_obj);

/* Implements text measurement for an object virtual table. */
NTG_API struct ntg_object_measure
ntg_text_measure_fn(
        const ntg_object* _text_obj,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

/* Draws the text content using the configured wrapping and alignment. */
NTG_API void
ntg_text_draw_fn(
        const ntg_object* _text_obj,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

/* Virtual deinitializer that dispatches to `ntg_text_deinit`. */
NTG_API void
ntg_text_deinit_fn(ntg_object* _text_obj);

NTG_API void
ntg_text_focus_fn(ntg_object* object, ntg_object* old_focused);

NTG_API void
ntg_text_unfocus_fn(ntg_object* object, ntg_object* new_focused);

/* Default virtual table used by `ntg_text_init`. */
NTG_API extern const struct ntg_object_vtable NTG_TEXT_VTABLE;

#endif // NTG_TEXT_H
