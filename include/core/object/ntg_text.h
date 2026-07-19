#ifndef NTG_TEXT_H
#define NTG_TEXT_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

enum ntg_text_wrap
{
    NTG_TEXT_WRAP_NONE,
    NTG_TEXT_WRAP_CHAR,
    NTG_TEXT_WRAP_WORD
};

enum ntg_text_mode
{
    NTG_TEXT_TEXT_ALIGN,
    NTG_TEXT_TEXT_JUSTIFY
};

enum ntg_text_bg_mode
{
    NTG_TEXT_BG_FULL,
    NTG_TEXT_BG_FLT
};

struct ntg_text_opts
{
    ntg_orient orient;
    struct nt_gfx gfx;
    ntg_text_mode text_mode;
    ntg_align prim_align; // Active only if NTG_TEXT_TEXT_ALIGN
    ntg_align sec_align;
    ntg_text_bg_mode bg_mode;
    ntg_text_wrap wrap;
    bool autotrim;
    size_t indent;
};

/* Creates horizontal text defaults with default graphics, aligned text, full
 * background, start alignment, no wrapping, automatic trimming, and no
 * indentation.
 *
 * RETURN VALUE:
 * The default `ntg_text_opts` value. */
NTG_API struct ntg_text_opts
ntg_text_opts_def();

/* ------------------------------------------------------ */

/* Compares two text option values. Pointer identity counts as equal; otherwise
 * a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_text_opts_are_eq(
        const struct ntg_text_opts* opts1,
        const struct ntg_text_opts* opts2);

struct ntg_text_hooks
{
    void (*on_opts_chng_fn)(
            ntg_text* text,
            const struct ntg_text_opts* old_opts,
            const struct ntg_text_opts* new_opts);

    // Not null terminated
    void (*on_text_chng_fn)(
            ntg_text* text,
            const char* old_text,
            size_t old_len,
            const char* new_text,
            size_t new_len);
};

struct ntg_text
{
    ntg_object __base;

    struct ntg_text_opts _opts;

    struct
    {
        char* data;
        size_t len;
    } _text;

    struct ntg_text_priv* __priv;

    struct ntg_text_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes an empty text text and its base object. A `NULL` options pointer
 * selects defaults.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: text or base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_text_init(
        ntg_text* text,
        const struct ntg_text_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Releases the text text, private layout data, and base-object resources.
 * Passing `NULL` has no effect. */
NTG_API void
ntg_text_deinit(ntg_text* text);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_text_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_text_deinit_(void* _text);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Updates text orientation, graphics, wrapping, alignment, background,
 * trimming, and indentation. A `NULL` options pointer applies defaults;
 * unchanged options are ignored. */
NTG_API void
ntg_text_set_opts(ntg_text* text, const struct ntg_text_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

/* Replaces text text from a non-`NULL`, null-terminated UTF-8 string. The
 * text copies the text. The current implementation calls `strlen` before
 * validation, so `text == NULL` causes undefined behavior rather than a
 * reported error.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: text or conversion storage cannot be allocated.
 * - `NTG_ERR_UTF_CONV`: the byte sequence is not valid UTF-8 or cannot be
 *   converted. */
NTG_API void
ntg_text_set_text(ntg_text* text, const char* text, int* out_status);

/* ------------------------------------------------------ */

/* Replaces text text from up to `len` UTF-8 bytes, capped at `NTG_SIZE_MAX *
 * NTG_SIZE_MAX`; the input need not be null-terminated. The text copies the
 * text, and `text` must be non-`NULL` even when `len` is zero.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `text` or `text` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: text, trimming, conversion, or row storage cannot be
 *   allocated.
 * - `NTG_ERR_UTF_CONV`: the specified byte sequence is not valid UTF-8 or
 *   cannot be converted. */
NTG_API void
ntg_text_set_text_safe(
        ntg_text* text,
        const char* text,
        size_t len,
        int* out_status);

#endif // NTG_TEXT_H
