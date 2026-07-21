#ifndef NTG_LABEL_H
#define NTG_LABEL_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_text.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_label_opts
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

/* Creates horizontal label defaults with default graphics, aligned label, full
 * background, start alignment, no wrapping, automatic trimming, and no
 * indentation. */
NTG_API struct ntg_label_opts
ntg_label_opts_def();

/* Compares two option values. Pointer identity counts as equal; otherwise
 * a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_label_opts_are_eq(
        const struct ntg_label_opts* opts1,
        const struct ntg_label_opts* opts2);

struct ntg_label_hooks
{
    void (*on_text_chng_fn)(
        ntg_label* label,
        struct ntg_str_view old_text,
        struct ntg_str_view new_text);

    void (*on_opts_chng_fn)(
        ntg_label* label,
        const struct ntg_label_opts* old_opts,
        const struct ntg_label_opts* new_opts);
};

struct ntg_label
{
    ntg_text __base;

    struct ntg_label_hooks hooks;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_label_init(ntg_label* label, const struct ntg_label_opts* opts, int* out_status);

NTG_API void
ntg_label_deinit(ntg_label* label);

NTG_API void
ntg_label_deinit_v(void* _label);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API void
ntg_label_get_opts(const ntg_label* label, struct ntg_label_opts* out_opts);

/* Updates label orientation, graphics, alignment, wrapping, trimming, and
 * indentation. A `NULL` options pointer applies defaults. */
NTG_API void
ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_label_get_text(const struct ntg_label* label);

/* Copies a null-terminated UTF-8 string into the label object.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `label` or `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: a text or conversion buffer cannot be allocated.
 * - `NTG_ERR_UTF_CONV`: UTF-8 to UTF-32 conversion fails. */
NTG_API void
ntg_label_set_text_unsafe(
        ntg_label* label,
        const char* text,
        ntg_text_set_mode mode,
        int* out_status);

/* ------------------------------------------------------ */

/* Copies exactly `len` bytes from a UTF-8 buffer into the label object. The
 * input does not need to be null-terminated.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `label` or `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: a text or conversion buffer cannot be allocated.
 * - `NTG_ERR_UTF_CONV`: UTF-8 to UTF-32 conversion fails. */
NTG_API void
ntg_label_set_text(
        ntg_label* label,
        const char* text,
        size_t len,
        ntg_text_set_mode mode,
        int* out_status);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

NTG_API void
ntg_label_init_inherit(
        ntg_label* label,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

NTG_API struct ntg_object_measure
ntg_label_measure_fn(
        const ntg_object* _label,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_label_draw_fn(
        const ntg_object* label,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_label_deinit_fn(ntg_object* _label);

NTG_API void
ntg_label_focus_fn(ntg_object* _label, ntg_object* old_focused);

NTG_API void
ntg_label_unfocus_fn(ntg_object* _label, ntg_object* new_focused);

NTG_API extern const struct ntg_object_vtable NTG_LABEL_VTABLE;

#endif // NTG_LABEL_H
