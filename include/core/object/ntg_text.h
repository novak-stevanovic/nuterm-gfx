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
    NTG_TEXT_ALIGN,
    NTG_TEXT_JUSTIFY
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
NTG_API struct ntg_text_opts ntg_text_opts_def();

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
            ntg_text* text_obj,
            const struct ntg_text_opts* old_opts,
            const struct ntg_text_opts* new_opts);

    // Not null terminated
    void (*on_text_chng_fn)(
            ntg_text* text_obj,
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

NTG_API void
ntg_text_init(
        ntg_text* text_obj,
        const struct ntg_text_opts* opts,
        int* out_status);

NTG_API void
ntg_text_deinit(ntg_text* text_obj);

NTG_API void
ntg_text_deinit_(void* _text);

NTG_API void
ntg_text_set_opts(ntg_text* text_obj, const struct ntg_text_opts* opts);

NTG_API void
ntg_text_set_text(ntg_text* text_obj, const char* text, int* out_status);

NTG_API void
ntg_text_set_text_safe(
        ntg_text* text_obj,
        const char* text,
        size_t len,
        int* out_status);

#endif // NTG_TEXT_H
