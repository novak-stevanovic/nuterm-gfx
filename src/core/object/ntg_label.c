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

static void
label_opts_from_text(
        struct ntg_label_opts* label_opts,
        const struct ntg_text_opts* text_opts)
{
    if(!label_opts || !text_opts) return;

    (*label_opts) = (struct ntg_label_opts) {
        .orient = text_opts->orient,
        .gfx = text_opts->gfx,
        .focused_gfx = text_opts->focused_gfx,
        .text_mode = text_opts->text_mode,
        .bg_mode = text_opts->bg_mode,
        .prim_align = text_opts->prim_align,
        .sec_align = text_opts->sec_align,
        .wrap = text_opts->wrap,
        .indent = text_opts->indent
    };
}

static void
label_opts_extract(
        const struct ntg_label_opts* label_opts,
        struct ntg_text_opts* out_text_opts)
{
    if(!label_opts || !out_text_opts) return;

    (*out_text_opts) = (struct ntg_text_opts) {
        .orient = label_opts->orient,
        .gfx = label_opts->gfx,
        .focused_gfx = label_opts->focused_gfx,
        .text_mode = label_opts->text_mode,
        .bg_mode = label_opts->bg_mode,
        .prim_align = label_opts->prim_align,
        .sec_align = label_opts->sec_align,
        .wrap = label_opts->wrap,
        .indent = label_opts->indent
    };
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_label_opts ntg_label_opts_def()
{
    struct ntg_text_opts def_text_opts = ntg_text_opts_def();

    struct ntg_label_opts label_opts = {0};
    label_opts_from_text(&label_opts, &def_text_opts);

    return label_opts;
}

bool ntg_label_opts_are_eql(
        const struct ntg_label_opts* opts1,
        const struct ntg_label_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->orient == opts2->orient) &&
           nt_gfx_are_eql(opts1->gfx, opts2->gfx) &&
           (opts1->text_mode == opts2->text_mode) &&
           (opts1->prim_align == opts2->prim_align) &&
           (opts1->sec_align == opts2->sec_align) &&
           (opts1->bg_mode == opts2->bg_mode) &&
           (opts1->wrap == opts2->wrap) &&
           (opts1->indent == opts2->indent));
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

    ntg_label_init_inherit(label, &NTG_LABEL_VTABLE, &NTG_TYPE_LABEL, &_status);
    if(!_status)
    {
        ntg_label_set_opts(label, opts);
        ntg_label_set_text_unsafe(label, "", 0, NULL);

        // TODO:
        ntg_object_set_focusable(ntg_obj(label), NTG_OBJECT_FOCUSABLE);
        ntg_object_set_clickable(ntg_obj(label), NTG_OBJECT_CLICKABLE);
    }
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
        (*out_opts) = ntg_label_opts_def();
    else
        label_opts_from_text(out_opts, &(ntg_txt(label))->_opts);
}

void ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts)
{
    if(!label) return;

    struct ntg_label_opts new_opts = (opts ? (*opts) : ntg_label_opts_def());
    struct ntg_label_opts old_opts = {0};
    ntg_label_get_opts(label, &old_opts);

    if(ntg_label_opts_are_eql(&new_opts, &old_opts))
        return;

    struct ntg_text_opts new_opts_text = {0};
    label_opts_extract(&new_opts, &new_opts_text);

    ntg_text_set_opts(ntg_txt(label), &new_opts_text);

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
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    if(!label || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_LABEL))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    ntg_text_init_inherit(ntg_txt(label), vtable, type, &_status);
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
}

struct ntg_object_measure
ntg_label_measure_fn(
        const ntg_object* _label,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena)
{
    return ntg_text_measure_fn(_label, orient, _layout_cache, arena);
}

void ntg_label_draw_fn(
        const ntg_object* _label,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena)
{
    ntg_text_draw_fn(_label, out_drawing, _layout_cache, arena);
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

const struct ntg_object_vtable NTG_LABEL_VTABLE = {
    .measure_fn = ntg_label_measure_fn,
    .draw_fn = ntg_label_draw_fn,
    .deinit_fn = ntg_label_deinit_fn,
    .focus_fn = ntg_label_focus_fn,
    .unfocus_fn = ntg_label_unfocus_fn
};
