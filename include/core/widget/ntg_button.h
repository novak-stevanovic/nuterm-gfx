#ifndef NTG_BUTTON_H
#define NTG_BUTTON_H

#include "shared/ntg_shared.h"
#include "core/widget/ntg_text_wgt.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_button_opts
{
    enum ntg_orient orient;

    /* If enabled_gfx, disabled_gfx & focused_gfx are all set to NT_GFX_ZERO,
     * `ntg_button_set_opts()` will set style to NT_STYLE_REVERSE */

    /* If NTG_TEXT_BG_OVERLAY bg colors are ignored. */
    struct nt_gfx enabled_gfx, disabled_gfx, focused_gfx;

    enum ntg_text_wrap wrap;
    enum ntg_text_line_mode line_mode;
    enum ntg_align prim_align; // active only if NTG_TEXT_LINE_ALIGN
    enum ntg_align sec_align;
    enum ntg_text_bg_mode bg_mode;

    size_t indent;
};

static const struct ntg_button_opts NTG_BUTTON_OPTS_ZERO = {0};

struct ntg_button
{
    ntg_text_wgt _base;

    struct
    {
        bool enabled;
        struct nt_gfx enabled_gfx, disabled_gfx, focused_gfx;
    } ro;

    struct
    {
        bool (*click_fn)(ntg_button* button);
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_button_init(
        ntg_button* button,
        const struct ntg_button_opts* opts,
        bool (*click_fn)(ntg_button* button));

NTG_API int
ntg_button_deinit(ntg_button* button);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API int
ntg_button_get_opts(ntg_button* button, struct ntg_button_opts* out_opts);

NTG_API int
ntg_button_set_opts(ntg_button* button, const struct ntg_button_opts* opts);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API int
ntg_button_set_click_fn(ntg_button* button, bool (*click_fn)(ntg_button* button));

NTG_API int
ntg_button_enable(ntg_button* button);

NTG_API int
ntg_button_disable(ntg_button* button);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_button_get_text(const struct ntg_button* button);

NTG_API int
ntg_button_set_text(ntg_button* button, const char* text, size_t len);

NTG_API int
ntg_button_set_text_cstr(ntg_button* button, const char* text);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_button_vtable
{
    struct ntg_text_wgt_vtable base;

    void (*enable_fn)(ntg_button* button);
    void (*disable_fn)(ntg_button* button);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_button_init_inherit(
        ntg_button* button,
        const struct ntg_button_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_button_vtable NTG_BUTTON_VTABLE;

NTG_API int
ntg_button_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena);

NTG_API int
ntg_button_measure_fn(
        const ntg_widget* _button,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_button_draw_fn(
        const ntg_widget* _button,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

NTG_API void
ntg_button_deinit_fn(ntg_object* _button);

NTG_API bool
ntg_button_process_mouse_fn(ntg_widget* _button, const struct ntg_widget_mouse* event);

NTG_API void
ntg_button_focus_fn(ntg_widget* _button);

NTG_API void
ntg_button_unfocus_fn(ntg_widget* _button);

NTG_API void
ntg_button_post_draw_fn(
        const ntg_text_wgt* _button,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

#endif // NTG_BUTTON_H
