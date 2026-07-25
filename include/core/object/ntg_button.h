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
    ntg_orient orient;
    struct nt_gfx gfx, focused_gfx;
    ntg_text_mode text_mode;
    ntg_align prim_align; 
    ntg_align sec_align;
    ntg_text_bg_mode bg_mode;
    ntg_text_wrap wrap;
    size_t indent;
};

NTG_API struct ntg_button_opts
ntg_button_opts_def();

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
        bool (*process_mouse_fn)(
                ntg_object* object,
                struct nt_mouse_event mouse,
                ntg_object_click_type type),
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
ntg_button_set_on_click_fn(
        ntg_button* button,
        bool (*process_mouse_fn)(
                ntg_object* object,
                struct nt_mouse_event mouse,
                ntg_object_click_type type));

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
        const struct ntg_object_vtable* vtable,
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

NTG_API extern const struct ntg_object_vtable NTG_BUTTON_VTABLE;

#endif // NTG_BUTTON_H
