#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

bool ntg_button_opts_are_eql(
        const struct ntg_button_opts* opts1,
        const struct ntg_button_opts* opts2)
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

int ntg_button_init(
        ntg_button* button,
        const struct ntg_button_opts* opts,
        bool (*click_fn)(ntg_button* button))
{
    if(!button)
        return NTG_ERR_INV_ARG;

    int status = ntg_button_init_inherit(button, &NTG_BUTTON_VTABLE, &NTG_TYPE_BUTTON, NULL);
    NTG_POST_INHERIT_CHECK(status);

    ntg_button_set_opts(button, opts);
    ntg_button_set_click_fn(button, click_fn);
    return 0;
}

int ntg_button_deinit(ntg_button* button)
{
    if(!button) return NTG_ERR_INV_ARG;

    ntg_object_zero(button);
    ntg_text_deinit(ntg_txt(button));

    return 0;
}

void ntg_button_deinit_void(void* _button)
{
    ntg_button_deinit(_button);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_button_get_opts(const ntg_button* button, struct ntg_button_opts* out_opts)
{
    if(!button || !out_opts) return NTG_ERR_INV_ARG;

    out_opts->text_opts = (ntg_txt(button))->ro.opts;

    return 0;
}

int ntg_button_set_opts(ntg_button* button, const struct ntg_button_opts* opts)
{
    if(!button) return NTG_ERR_INV_ARG;

    struct ntg_button_opts new_opts = (opts ? (*opts) : NTG_BUTTON_OPTS_ZERO);
    struct ntg_button_opts old_opts = {0};
    ntg_button_get_opts(button, &old_opts);

    if(ntg_button_opts_are_eql(&new_opts, &old_opts))
        return 0;

    ntg_text_set_opts(ntg_txt(button), &new_opts.text_opts);

    struct ntg_event_button_optchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &new_opts
    };
    ntg_object_event_raise(ntg_obj(button), NTG_EVENT_BUTTON_OPTCHG, &event_dt);

    return 0;
}

/* ------------------------------------------------------ */
/* CLICK HANDLER */
/* ------------------------------------------------------ */

int ntg_button_set_click_fn(ntg_button* button, bool (*click_fn)(ntg_button* button))
{
    if(!button) return NTG_ERR_INV_ARG; 

    button->priv.click_fn = click_fn;

    return 0;
}

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

struct ntg_str_view ntg_button_get_text(const struct ntg_button* button)
{
    return ntg_text_get_text(ntg_txt(button));
}

int ntg_button_set_text(ntg_button* button, const char* text, size_t len)
{
    return ntg_text_set_text(ntg_txt(button), text, len);
}

int ntg_button_set_text_unsafe(ntg_button* button, const char* text)
{
    if(!button)
        return NTG_ERR_INV_ARG;

    return ntg_text_set_text_unsafe(ntg_txt(button), text);
}

/* ------------------------------------------------------ */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_button_init_inherit(
        ntg_button* button,
        const struct ntg_button_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!button || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_BUTTON))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_text_init_inherit(ntg_txt(button), &vtable->base, type, layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(button);

    ntg_widget_set_focusable(ntg_wgt(button), NTG_WIDGET_FOCUSABLE);
    ntg_widget_set_clickable(ntg_wgt(button), NTG_WIDGET_CLICKABLE_CONT);

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_button_vtable NTG_BUTTON_VTABLE = {
    .base = {
        .base = {
            .layout_prepare_fn = ntg_button_layout_prepare_fn,
            .measure_fn = ntg_button_measure_fn,
            .draw_fn = ntg_button_draw_fn,
            .base.deinit_fn = ntg_button_deinit_fn,
            .cont_resize_fn = ntg_text_cont_resize_fn,
            .handle_mouse_fn = ntg_button_process_mouse_fn,
            .focus_fn = ntg_button_focus_fn,
            .unfocus_fn = ntg_button_unfocus_fn
        },
        .post_draw_fn = ntg_button_post_draw_fn
    }
};

int ntg_button_layout_prepare_fn(
        ntg_widget* widget, 
        struct ntg_widget_layout_dt* layout_dt,
        sarena* arena)
{
    return ntg_text_layout_prepare_fn(widget, layout_dt, arena);
}

int ntg_button_measure_fn(
        const ntg_widget* _button,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure)
{
    return ntg_text_measure_fn(
            _button, layout_dt, orient, arena, relayout, out_measure);
}

int ntg_button_draw_fn(
        const ntg_widget* _button,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    if(ntg_xy_is_zero_any(ntg_widget_get_size_cont(_button))) return 0;

    return ntg_text_draw_fn(_button, layout_dt, out_drawing, arena);
}

void ntg_button_deinit_fn(ntg_object* _button)
{
    ntg_button_deinit(ntg_btn(_button));
}

bool ntg_button_process_mouse_fn(ntg_widget* _button, const struct ntg_widget_mouse* event)
{
    if(!_button) return false;

    ntg_button* button = ntg_btn(_button);

    if(button->priv.click_fn && (event->mouse.type == NT_MOUSE_CLICK_LEFT))
        return button->priv.click_fn(button);
    else
        return false;
}

void ntg_button_focus_fn(ntg_widget* _button)
{
    ntg_text_focus_fn(_button);
}

void ntg_button_unfocus_fn(ntg_widget* _button)
{
    ntg_text_unfocus_fn(_button);
}

void ntg_button_post_draw_fn(
        const ntg_text* _button,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)_button;
    (void)out_drawing;
    (void)arena;
}

