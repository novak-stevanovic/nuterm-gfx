#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_button_opts ntg_button_opts_default(void)
{
    return (struct ntg_button_opts) {
        .text_opts = ntg_text_opts_default()
    };
}

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

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_button_init(
        ntg_button* button,
        const struct ntg_button_opts* opts,
        bool (*click_fn)(ntg_button* button))
{
    int _status = ntg_button_init_inherit(
        button,
        &NTG_BUTTON_VTABLE_OBJECT,
        &NTG_BUTTON_VTABLE_TEXT,
        &NTG_TYPE_BUTTON,
        NULL);
    if(!_status)
    {
        ntg_button_set_opts(button, opts);
        ntg_button_set_click_fn(button, click_fn);
    }

    return _status;
}

void ntg_button_deinit(ntg_button* button)
{
    if(!button) return;

    ntg_text_deinit(ntg_txt(button));

    button->hooks = (struct ntg_button_hooks) {0};
    button->__click_fn = NULL;
}

void ntg_button_deinit_void(void* _button)
{
    ntg_button_deinit(_button);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

void ntg_button_get_opts(const ntg_button* button, struct ntg_button_opts* out_opts)
{
    if(!out_opts) return;

    if(!button)
        (*out_opts) = ntg_button_opts_default();
    else
        out_opts->text_opts = (ntg_txt(button))->_opts;
}

void ntg_button_set_opts(ntg_button* button, const struct ntg_button_opts* opts)
{
    if(!button) return;

    struct ntg_button_opts new_opts = (opts ? (*opts) : ntg_button_opts_default());
    struct ntg_button_opts old_opts = {0};
    ntg_button_get_opts(button, &old_opts);

    if(ntg_button_opts_are_eql(&new_opts, &old_opts))
        return;

    ntg_text_set_opts(ntg_txt(button), &new_opts.text_opts);

    if(button->hooks.on_opts_chng_fn)
    {
        button->hooks.on_opts_chng_fn(button, &old_opts, &new_opts);
    }
}

/* ------------------------------------------------------ */
/* CLICK HANDLER */
/* ------------------------------------------------------ */

void ntg_button_set_click_fn(ntg_button* button, bool (*click_fn)(ntg_button* button))
{
    if(!button) return; 

    button->__click_fn = click_fn;
}

/* ------------------------------------------------------ */
/* TEXT */
/* ------------------------------------------------------ */

struct ntg_str_view ntg_button_get_text(const struct ntg_button* button)
{
    if(!button)
    {
        return (struct ntg_str_view) {
            .data = "",
            .len = 0
        };
    }
    else
    {
        return (struct ntg_str_view) {
            .data = (ntg_txt(button))->_text,
            .len = (ntg_txt(button))->_text_len,
        };
    }
}

int ntg_button_set_text_unsafe(
        ntg_button* button,
        const char* text,
        uint16_t flags)
{
    if(!button)
        return NTG_ERR_INV_ARG;

    return ntg_text_set_text_unsafe(ntg_txt(button), text, flags);
}

int ntg_button_set_text(
        ntg_button* button,
        const char* text,
        size_t len,
        uint16_t flags)
{
    return ntg_text_set_text(ntg_txt(button), text, len, flags);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

int ntg_button_init_inherit(
        ntg_button* button,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!button || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instance_of(type, &NTG_TYPE_BUTTON))
        return NTG_ERR_INV_TYPE;

    button->hooks = (struct ntg_button_hooks) {0};
    button->__click_fn = NULL;

    int _status = ntg_text_init_inherit(
            ntg_txt(button), object_vtable, text_vtable, type, layout_dt);
    if(_status != 0)
        return _status;

    ntg_button_set_opts(button, NULL);
    (void)ntg_button_set_text_unsafe(button, "", 0);

    ntg_object_set_focusable(ntg_obj(button), NTG_OBJECT_FOCUSABLE);
    ntg_object_set_clickable(ntg_obj(button), NTG_OBJECT_CLICKABLE_CONT);
    return 0;
}

int
ntg_button_measure_fn(
        const ntg_object* _button,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure)
{
    return ntg_text_measure_fn(
            _button, layout_dt, orient, arena, relayout, out_measure);
}

int ntg_button_draw_fn(
        const ntg_object* _button,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout)
{
    if(ntg_xy_size_is_zero(ntg_object_get_size_cont(_button))) return 0;

    return ntg_text_draw_fn(
            _button, layout_dt, out_drawing, arena, relayout);
}

void ntg_button_deinit_fn(ntg_object* _button)
{
    ntg_button_deinit(ntg_btn(_button));
}

bool ntg_button_process_mouse_fn(ntg_object* _button, const struct ntg_object_mouse* event)
{
    if(!_button) return false;

    ntg_button* button = ntg_btn(_button);

    if(button->__click_fn && (event->mouse.type == NT_MOUSE_CLICK_LEFT))
        return button->__click_fn(button);
    else
        return false;
}

void ntg_button_focus_fn(ntg_object* _button, ntg_object* old_focused)
{
    ntg_text_focus_fn(_button, old_focused);
}

void ntg_button_unfocus_fn(ntg_object* _button, ntg_object* new_focused)
{
    ntg_text_unfocus_fn(_button, new_focused);
}

const struct ntg_object_vtable NTG_BUTTON_VTABLE_OBJECT = {
    .measure_fn = ntg_button_measure_fn,
    .draw_fn = ntg_button_draw_fn,
    .deinit_fn = ntg_button_deinit_fn,
    .process_mouse_fn = ntg_button_process_mouse_fn,
    .focus_fn = ntg_button_focus_fn,
    .unfocus_fn = ntg_button_unfocus_fn
};

void ntg_button_post_draw_fn(
        const ntg_text* _button,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena)
{
    (void)_button;
    (void)out_drawing;
    (void)arena;
}

const struct ntg_text_vtable NTG_BUTTON_VTABLE_TEXT = {
    .post_draw_fn = ntg_button_post_draw_fn
};
