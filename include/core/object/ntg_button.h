#ifndef NTG_BUTTON_H
#define NTG_BUTTON_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_text.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_button_opts
{
    struct ntg_text_opts text_opts;
};

NTG_API struct ntg_button_opts
ntg_button_opts_default();

NTG_API bool
ntg_button_opts_are_eql(
        const struct ntg_button_opts* opts1,
        const struct ntg_button_opts* opts2);

struct ntg_button_hooks
{
    void (*on_text_chng_fn)(
        ntg_button* button,
        struct ntg_str_view old_text,
        struct ntg_str_view new_text);

    void (*on_opts_chng_fn)(
        ntg_button* button,
        const struct ntg_button_opts* old_opts,
        const struct ntg_button_opts* new_opts);
};

struct ntg_button
{
    ntg_text __base;

    struct ntg_object_vtable __vtable;
    struct ntg_button_hooks hooks;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_button_init(
        ntg_button* button,
        const struct ntg_button_opts* opts,
        bool (*mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event),
        int* out_status);

NTG_API void
ntg_button_deinit(ntg_button* button);

NTG_API void
ntg_button_deinit_void(void* _button);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API void
ntg_button_get_opts(const ntg_button* button, struct ntg_button_opts* out_opts);

NTG_API void
ntg_button_set_opts(ntg_button* button, const struct ntg_button_opts* opts);

/* ------------------------------------------------------ */
/* CLICK HANDLER */
/* ------------------------------------------------------ */

NTG_API void
ntg_button_set_click_fn(
        ntg_button* button,
        bool (*mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event));

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_button_get_text(const struct ntg_button* button);

NTG_API void
ntg_button_set_text_unsafe(
        ntg_button* button,
        const char* text,
        ntg_text_set_flags flags,
        int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_button_set_text(
        ntg_button* button,
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

// `vtable` is copied into button's internal vtable
NTG_API void
ntg_button_init_inherit(
        ntg_button* button,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        int* out_status);

NTG_API struct ntg_object_measure
ntg_button_measure_fn(
        const ntg_object* _button,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_button_draw_fn(
        const ntg_object* _button,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

NTG_API void
ntg_button_deinit_fn(ntg_object* _button);

NTG_API void
ntg_button_focus_fn(ntg_object* _button, ntg_object* old_focused);

NTG_API void
ntg_button_unfocus_fn(ntg_object* _button, ntg_object* new_focused);

NTG_API extern const struct ntg_object_vtable NTG_BUTTON_VTABLE_OBJECT;

NTG_API void
ntg_button_post_draw_fn(
        const ntg_text* _button,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena);

NTG_API extern const struct ntg_text_vtable NTG_BUTTON_VTABLE_TEXT;

#endif // NTG_BUTTON_H
