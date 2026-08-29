#ifndef NTG_LABEL_H
#define NTG_LABEL_H

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

struct ntg_label_opts
{
    enum ntg_orient orient;

    /* If NTG_TEXT_BG_OVERLAY bg color is ignored. */
    struct nt_gfx gfx;

    enum ntg_text_wrap wrap;
    enum ntg_text_line_mode line_mode;
    enum ntg_align prim_align; // active only if NTG_TEXT_LINE_ALIGN
    enum ntg_align sec_align;
    enum ntg_text_bg_mode bg_mode;

    size_t indent;
};

static const struct ntg_label_opts NTG_LABEL_OPTS_ZERO = {0};

struct ntg_label
{
    ntg_text_wgt _base;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_label_init(ntg_label* label, const struct ntg_label_opts* opts);

NTG_API int
ntg_label_deinit(ntg_label* label);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API int
ntg_label_get_opts(ntg_label* label, struct ntg_label_opts* out_opts);

NTG_API int
ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts);

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

NTG_API struct ntg_str_view
ntg_label_get_text(const struct ntg_label* label);

NTG_API int
ntg_label_set_text(ntg_label* label, const char* text, size_t len);

NTG_API int
ntg_label_set_text_cstr(ntg_label* label, const char* text);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_label_vtable
{
    struct ntg_text_wgt_vtable base;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_label_init_inherit(
        ntg_label* label,
        const struct ntg_label_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_label_vtable NTG_LABEL_VTABLE;

NTG_API int
ntg_label_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena);

NTG_API int
ntg_label_measure_fn(
        const ntg_widget* _label,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_label_draw_fn(
        const ntg_widget* _label,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

NTG_API void
ntg_label_deinit_fn(ntg_object* _label);

NTG_API void
ntg_label_post_draw_fn(
        const ntg_text_wgt* _label,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

#endif // NTG_LABEL_H
