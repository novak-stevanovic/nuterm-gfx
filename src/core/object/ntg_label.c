#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_label* label)
{
    label->hooks = (struct ntg_label_hooks) {0};
}


/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_label_opts ntg_label_opts_default()
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

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_label_init(
        ntg_label* label,
        const struct ntg_label_opts* opts,
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    ntg_label_init_inherit(
            label,
            &NTG_LABEL_VTABLE_OBJECT,
            &NTG_LABEL_VTABLE_TEXT,
            &NTG_TYPE_LABEL,
            &_status);
    if(!_status)
    {
        ntg_label_set_opts(label, opts);
        ntg_label_set_text_unsafe(label, "", 0, NULL);
    }

    ntg_vreturn(out_status, _status);
}

void ntg_label_deinit(ntg_label* label)
{
    if(!label) return;

    ntg_text_deinit(ntg_txt(label));

    init_default(label);
}

void ntg_label_deinit_void(void* _label)
{
    ntg_label_deinit(_label);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

void ntg_label_get_opts(const ntg_label* label, struct ntg_label_opts* out_opts)
{
    if(!out_opts) return;

    if(!label)
        (*out_opts) = ntg_label_opts_default();
    else
        out_opts->text_opts = (ntg_txt(label))->_opts;
}

void ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts)
{
    if(!label) return;

    struct ntg_label_opts new_opts = (opts ? (*opts) : ntg_label_opts_default());
    struct ntg_label_opts old_opts = {0};
    ntg_label_get_opts(label, &old_opts);

    if(ntg_label_opts_are_eql(&new_opts, &old_opts))
        return;

    ntg_text_set_opts(ntg_txt(label), &new_opts.text_opts);

    if(label->hooks.on_opts_chng_fn)
    {
        label->hooks.on_opts_chng_fn(label, &old_opts, &new_opts);
    }
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
            .data = (ntg_txt(label))->_text.data,
            .len = (ntg_txt(label))->_text.len,
        };
    }
}

void ntg_label_set_text_unsafe(
        ntg_label* label,
        const char* text,
        ntg_text_set_flags flags,
        int* out_status)
{
    if(!label)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_text_set_text_unsafe(ntg_txt(label), text, flags, out_status);
}

void ntg_label_set_text(
        ntg_label* label,
        const char* text,
        size_t len,
        ntg_text_set_flags flags,
        int* out_status)
{
    ntg_text_set_text(ntg_txt(label), text, len, flags, out_status);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_label_init_inherit(
        ntg_label* label,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    if(!label || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_LABEL))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    ntg_text_init_inherit(ntg_txt(label), object_vtable, text_vtable, type, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    init_default(label);

    // TODO:
    ntg_object_set_focusable(ntg_obj(label), NTG_OBJECT_FOCUSABLE);
    ntg_object_set_clickable(ntg_obj(label), NTG_OBJECT_CLICKABLE_CONT);
}

struct ntg_object_measure
ntg_label_measure_fn(
        const ntg_object* _label,
        ntg_orient orient,
        sarena* arena,
        int* out_remeasure)
{
    return ntg_text_measure_fn(_label, orient, arena, out_remeasure);
}

void ntg_label_draw_fn(
        const ntg_object* _label,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        int* out_redraw)
{
    ntg_text_draw_fn(_label, out_drawing, arena, out_redraw);
}

void ntg_label_deinit_fn(ntg_object* _label)
{
    ntg_label_deinit(ntg_lbl(_label));
}

void ntg_label_focus_fn(ntg_object* _label, ntg_object* old_focused)
{
    ntg_text_focus_fn(_label, old_focused);
}

void ntg_label_unfocus_fn(ntg_object* _label, ntg_object* new_focused)
{
    ntg_text_unfocus_fn(_label, new_focused);
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
        sarena* arena) {}

const struct ntg_text_vtable NTG_LABEL_VTABLE_TEXT = {
    .post_draw_fn = ntg_label_post_draw_fn,
};
