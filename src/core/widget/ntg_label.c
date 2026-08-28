#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_label_init(ntg_label* label, const struct ntg_label_opts* opts)
{
    if(!label)
        return NTG_ERR_INV_ARG;

    int status = ntg_label_init_inherit(label, &NTG_LABEL_VTABLE, &NTG_TYPE_LABEL, NULL);
    NTG_POST_INHERIT_CHECK(status);

    ntg_label_set_opts(label, opts);
    return 0;
}

int ntg_label_deinit(ntg_label* label)
{
    if(!label) return NTG_ERR_INV_ARG;

    ntg_object_zero(label);
    ntg_text_deinit(ntg_txt(label));

    return 0;
}


/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts)
{
    if(!label) return NTG_ERR_INV_ARG;

    struct ntg_label_opts opts_final = (opts ? (*opts) : NTG_LABEL_OPTS_ZERO);

    return ntg_text_set_opts(ntg_txt(label), &opts_final.text_opts);
}

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

struct ntg_str_view ntg_label_get_text(const struct ntg_label* label)
{
    return ntg_text_get_text(ntg_txt(label));
}

int ntg_label_set_text(ntg_label* label, const char* text, size_t len)
{
    return ntg_text_set_text(ntg_txt(label), text, len);
}

int ntg_label_set_text_unsafe(ntg_label* label, const char* text)
{
    if(!label)
        return NTG_ERR_INV_ARG;

    return ntg_text_set_text_unsafe(ntg_txt(label), text);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_label_init_inherit(
        ntg_label* label,
        const struct ntg_label_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!label || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_LABEL))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_text_init_inherit(ntg_txt(label), &vtable->base, type, layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(label);

    // TODO:
    ntg_widget_set_focusable(ntg_wgt(label), NTG_WIDGET_FOCUSABLE);
    ntg_widget_set_clickable(ntg_wgt(label), NTG_WIDGET_CLICKABLE_CONT);
    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_label_vtable NTG_LABEL_VTABLE = {
    .base = {
        .base = {
            .layout_prepare_fn = ntg_label_layout_prepare_fn,
            .measure_fn = ntg_label_measure_fn,
            .draw_fn = ntg_label_draw_fn,
            .base.deinit_fn = ntg_label_deinit_fn,
            .resize_cont_fn = ntg_text_cont_resize_fn,
            .focus_fn = ntg_label_focus_fn,
            .unfocus_fn = ntg_label_unfocus_fn
        },
        .post_draw_fn = ntg_label_post_draw_fn
    }
};

int ntg_label_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena)
{
    return ntg_text_layout_prepare_fn(widget, layout_dt, arena);
}

int ntg_label_measure_fn(
        const ntg_widget* _label,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure)
{
    return ntg_text_measure_fn(
            _label, layout_dt, orient, arena, relayout, out_measure);
}

int ntg_label_draw_fn(
        const ntg_widget* _label,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    if(ntg_xy_is_zero_any(ntg_widget_get_size_cont(_label))) return 0;

    return ntg_text_draw_fn(_label, layout_dt, out_drawing, arena);
}

void ntg_label_deinit_fn(ntg_object* _label)
{
    ntg_label_deinit(ntg_lbl(_label));
}

void ntg_label_focus_fn(ntg_widget* _label)
{
    ntg_text_focus_fn(_label);
}

void ntg_label_unfocus_fn(ntg_widget* _label)
{
    ntg_text_unfocus_fn(_label);
}

void ntg_label_post_draw_fn(
        const ntg_text* _label,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)_label;
    (void)out_drawing;
    (void)arena;
}

