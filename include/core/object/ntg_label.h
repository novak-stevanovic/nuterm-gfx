#ifndef NTG_LABEL_H
#define NTG_LABEL_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"

// HOW TO APPLY ON_FOCUS EFFECT ON INPUT FIELD?
// WHAT IF SIZE CHANGES FOR INPUT AND MAKES CURSOR POS OUT OF RANGE?

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

enum ntg_label_wrap
{
    NTG_LABEL_WRAP_NONE,
    NTG_LABEL_WRAP_CHAR,
    NTG_LABEL_WRAP_WORD
};

enum ntg_label_mode
{
    NTG_LABEL_TEXT_ALIGN,
    NTG_LABEL_TEXT_JUSTIFY
};

enum ntg_label_bg_mode
{
    NTG_LABEL_BG_FULL,
    NTG_LABEL_BG_FLT
};

struct ntg_label_opts
{
    ntg_orient orient;
    struct nt_gfx gfx;
    ntg_label_mode text_mode;
    ntg_align prim_align; // Active only if NTG_LABEL_TEXT_ALIGN
    ntg_align sec_align;
    ntg_label_bg_mode bg_mode;
    ntg_label_wrap wrap;
    bool autotrim;
    size_t indent;
};

/* Creates horizontal label defaults with default graphics, aligned text, full
 * background, start alignment, no wrapping, automatic trimming, and no
 * indentation.
 *
 * RETURN VALUE:
 * The default `ntg_label_opts` value. */
NTG_API struct ntg_label_opts
ntg_label_opts_def();

/* ------------------------------------------------------ */

/* Compares two label option values. Pointer identity counts as equal; otherwise
 * a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_label_opts_are_eq(
        const struct ntg_label_opts* opts1,
        const struct ntg_label_opts* opts2);

struct ntg_label_hooks
{
    void (*on_opts_chng_fn)(
            ntg_label* label,
            const struct ntg_label_opts* old_opts,
            const struct ntg_label_opts* new_opts);

    // Not null terminated
    void (*on_text_chng_fn)(
            ntg_label* label,
            const char* old_text,
            size_t old_len,
            const char* new_text,
            size_t new_len);
};

struct ntg_label
{
    ntg_object __base;

    struct ntg_label_opts _opts;

    struct
    {
        char* data;
        size_t len;
    } _text;

    struct ntg_label_priv* __priv;

    struct ntg_label_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes an empty text label and its base object. A `NULL` options pointer
 * selects defaults.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `label` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: label or base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_label_init(
        ntg_label* label,
        const struct ntg_label_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Releases the label text, private layout data, and base-object resources.
 * Passing `NULL` has no effect. */
NTG_API void
ntg_label_deinit(ntg_label* label);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_label_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_label_deinit_(void* _label);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Updates label orientation, graphics, wrapping, alignment, background,
 * trimming, and indentation. A `NULL` options pointer applies defaults;
 * unchanged options are ignored. */
NTG_API void
ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

/* Replaces label text from a non-`NULL`, null-terminated UTF-8 string. The
 * label copies the text. The current implementation calls `strlen` before
 * validation, so `text == NULL` causes undefined behavior rather than a
 * reported error.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `label` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: text or conversion storage cannot be allocated.
 * - `NTG_ERR_UTF_CONV`: the byte sequence is not valid UTF-8 or cannot be
 *   converted. */
NTG_API void
ntg_label_set_text(ntg_label* label, const char* text, int* out_status);

/* ------------------------------------------------------ */

/* Replaces label text from up to `len` UTF-8 bytes, capped at `NTG_SIZE_MAX *
 * NTG_SIZE_MAX`; the input need not be null-terminated. The label copies the
 * text, and `text` must be non-`NULL` even when `len` is zero.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `label` or `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: text, trimming, conversion, or row storage cannot be
 *   allocated.
 * - `NTG_ERR_UTF_CONV`: the specified byte sequence is not valid UTF-8 or
 *   cannot be converted. */
NTG_API void
ntg_label_set_text_safe(
        ntg_label* label,
        const char* text,
        size_t len,
        int* out_status);

#endif // NTG_LABEL_H
