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
    struct ntg_text_opts text_opts;
};

NTG_API struct ntg_label_opts
ntg_label_opts_default();

NTG_API bool
ntg_label_opts_are_eql(
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
ntg_label_deinit_void(void* _label);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API void
ntg_label_get_opts(const ntg_label* label, struct ntg_label_opts* out_opts);


NTG_API void
ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_label_get_text(const struct ntg_label* label);


NTG_API void
ntg_label_set_text_unsafe(
        ntg_label* label,
        const char* text,
        ntg_text_set_flags flags,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_label_set_text(
        ntg_label* label,
        const char* text,
        size_t len,
        ntg_text_set_flags flags,
        int* out_status);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_label_init_inherit(
        ntg_label* label,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
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
        const ntg_object* _label,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_label_deinit_fn(ntg_object* _label);

NTG_API void
ntg_label_focus_fn(ntg_object* _label, ntg_object* old_focused);

NTG_API void
ntg_label_unfocus_fn(ntg_object* _label, ntg_object* new_focused);

NTG_API extern const struct ntg_object_vtable NTG_LABEL_VTABLE_OBJECT;

NTG_API void
ntg_label_post_draw_fn(
        const ntg_text* _label,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena);

NTG_API extern const struct ntg_text_vtable NTG_LABEL_VTABLE_TEXT;

#endif // NTG_LABEL_H
