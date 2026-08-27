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

bool ntg_label_opts_are_eql(
        const struct ntg_label_opts* opts1,
        const struct ntg_label_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ntg_text_opts_are_eql(&opts1->text_opts, &opts2->text_opts);
}

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

    ntg_entity_zero(label);
    ntg_text_deinit(ntg_txt(label));

    return 0;
}

void ntg_label_deinit_void(void* _label)
{
    ntg_label_deinit(_label);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_label_get_opts(const ntg_label* label, struct ntg_label_opts* out_opts)
{
    if(!label || !out_opts) return NTG_ERR_INV_ARG;

    out_opts->text_opts = (ntg_txt(label))->ro.opts;

    return 0;
}

int ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts)
{
    if(!label) return NTG_ERR_INV_ARG;

    struct ntg_label_opts new_opts = (opts ? (*opts) : NTG_LABEL_OPTS_ZERO);
    struct ntg_label_opts old_opts = {0};
    ntg_label_get_opts(label, &old_opts);

    if(ntg_label_opts_are_eql(&new_opts, &old_opts))
        return 0;

    ntg_text_set_opts(ntg_txt(label), &new_opts.text_opts);

    struct ntg_event_label_optchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &new_opts
    };
    ntg_entity_event_raise(ntg_ent(label), NTG_EVENT_LABEL_OPTCHG, &event_dt);

    return 0;
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
        struct ntg_object_layout_dt* layout_dt)
{
    if(!label || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_LABEL))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_text_init_inherit(ntg_txt(label), &vtable->base, type, layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_entity_zero(label);

    // TODO:
    ntg_object_set_focusable(ntg_obj(label), NTG_OBJECT_FOCUSABLE);
    ntg_object_set_clickable(ntg_obj(label), NTG_OBJECT_CLICKABLE_CONT);
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
            .cont_resize_fn = ntg_text_cont_resize_fn,
            .focus_fn = ntg_label_focus_fn,
            .unfocus_fn = ntg_label_unfocus_fn
        },
        .post_draw_fn = ntg_label_post_draw_fn
    }
};

int ntg_label_layout_prepare_fn(
        ntg_object* object, 
        struct ntg_object_layout_dt* layout_dt,
        sarena* arena)
{
    return ntg_text_layout_prepare_fn(object, layout_dt, arena);
}

int ntg_label_measure_fn(
        const ntg_object* _label,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure)
{
    return ntg_text_measure_fn(
            _label, layout_dt, orient, arena, relayout, out_measure);
}

int ntg_label_draw_fn(
        const ntg_object* _label,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_draw* out_drawing,
        sarena* arena)
{
    if(ntg_xy_is_zero_any(ntg_object_get_size_cont(_label))) return 0;

    return ntg_text_draw_fn(_label, layout_dt, out_drawing, arena);
}

void ntg_label_deinit_fn(ntg_entity* _label)
{
    ntg_label_deinit(ntg_lbl(_label));
}

void ntg_label_focus_fn(ntg_object* _label)
{
    ntg_text_focus_fn(_label);
}

void ntg_label_unfocus_fn(ntg_object* _label)
{
    ntg_text_unfocus_fn(_label);
}

void ntg_label_post_draw_fn(
        const ntg_text* _label,
        ntg_object_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)_label;
    (void)out_drawing;
    (void)arena;
}

