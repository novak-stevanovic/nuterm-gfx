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

struct ntg_label_opts ntg_label_opts_default(void)
{
    return (struct ntg_label_opts) {
        .text_opts = ntg_text_opts_default()
    };
}

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

int ntg_label_init(
        ntg_label* label,
        const struct ntg_label_opts* opts)
{
    int _status = ntg_label_init_inherit(
            label,
            &NTG_LABEL_VTABLE_OBJECT,
            &NTG_LABEL_VTABLE_TEXT,
            &NTG_TYPE_LABEL,
            NULL);
    if(!_status)
    {
        ntg_label_set_opts(label, opts);
        ntg_label_set_text_unsafe(label, "", 0);
    }

    return _status;
}

int ntg_label_deinit(ntg_label* label)
{
    if(!label) return NTG_ERR_INV_ARG;

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

    out_opts->text_opts = (ntg_txt(label))->_opts;

    return 0;
}

int ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts)
{
    if(!label) return NTG_ERR_INV_ARG;

    struct ntg_label_opts new_opts = (opts ? (*opts) : ntg_label_opts_default());
    struct ntg_label_opts old_opts = {0};
    ntg_label_get_opts(label, &old_opts);

    if(ntg_label_opts_are_eql(&new_opts, &old_opts))
        return 0;

    ntg_text_set_opts(ntg_txt(label), &new_opts.text_opts);

    struct ntg_event_label_optchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &new_opts
    };
    ntg_event_raise(
            &ntg_obj(label)->_event_del,
            ntg_event_new(NTG_EVENT_LABEL_OPTCHG, label, &event_dt));

    return 0;
}

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

struct ntg_str_view ntg_label_get_text(const struct ntg_label* label)
{
    if(!label)
    {
        return (struct ntg_str_view) {
            .data = "",
            .len = 0
        };
    }
    else
    {
        return (struct ntg_str_view) {
            .data = (ntg_txt(label))->_text,
            .len = (ntg_txt(label))->_text_len,
        };
    }
}

int ntg_label_set_text_unsafe(
        ntg_label* label,
        const char* text,
        uint16_t flags)
{
    if(!label)
        return NTG_ERR_INV_ARG;

    return ntg_text_set_text_unsafe(ntg_txt(label), text, flags);
}

int ntg_label_set_text(
        ntg_label* label,
        const char* text,
        size_t len,
        uint16_t flags)
{
    return ntg_text_set_text(ntg_txt(label), text, len, flags);
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
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!label || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instance_of(type, &NTG_TYPE_LABEL))
        return NTG_ERR_INV_TYPE;

    int _status = ntg_text_init_inherit(
            ntg_txt(label), object_vtable, text_vtable, type, layout_dt);
    if(_status != 0)
        return _status;


    // TODO:
    ntg_object_set_focusable(ntg_obj(label), NTG_OBJECT_FOCUSABLE);
    ntg_object_set_clickable(ntg_obj(label), NTG_OBJECT_CLICKABLE_CONT);
    return 0;
}

int
ntg_label_measure_fn(
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
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout)
{
    if(ntg_xy_is_zero_any(ntg_object_get_size_cont(_label))) return 0;

    return ntg_text_draw_fn(
            _label, layout_dt, out_drawing, arena, relayout);
}

void ntg_label_deinit_fn(ntg_object* _label)
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

const struct ntg_object_vtable NTG_LABEL_VTABLE_OBJECT = {
    .measure_fn = ntg_label_measure_fn,
    .draw_fn = ntg_label_draw_fn,
    .deinit_fn = ntg_label_deinit_fn,
    .focus_fn = ntg_label_focus_fn,
    .unfocus_fn = ntg_label_unfocus_fn
};

void ntg_label_post_draw_fn(
        const ntg_text* _label,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena)
{
    (void)_label;
    (void)out_drawing;
    (void)arena;
}

const struct ntg_text_vtable NTG_LABEL_VTABLE_TEXT = {
    .post_draw_fn = ntg_label_post_draw_fn,
};
