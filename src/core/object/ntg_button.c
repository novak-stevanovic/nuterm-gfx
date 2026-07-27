#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_button* button)
{
}

static void
button_opts_from_text(
        struct ntg_button_opts* button_opts,
        const struct ntg_text_opts* text_opts)
{
    if(!button_opts || !text_opts) return;

    (*button_opts) = (struct ntg_button_opts) {
        .orient = text_opts->orient,
        .gfx = text_opts->gfx,
        .focused_gfx = text_opts->focused_gfx,
        .text_mode = text_opts->text_mode,
        .bg_mode = text_opts->bg_mode,
        .prim_align = text_opts->prim_align,
        .sec_align = text_opts->sec_align,
        .wrap = text_opts->wrap,
        .scroll = text_opts->scroll,
        .indent = text_opts->indent
    };
}

static void
button_opts_extract(
        const struct ntg_button_opts* button_opts,
        struct ntg_text_opts* out_text_opts)
{
    if(!button_opts || !out_text_opts) return;

    (*out_text_opts) = (struct ntg_text_opts) {
        .orient = button_opts->orient,
        .gfx = button_opts->gfx,
        .focused_gfx = button_opts->focused_gfx,
        .text_mode = button_opts->text_mode,
        .bg_mode = button_opts->bg_mode,
        .prim_align = button_opts->prim_align,
        .sec_align = button_opts->sec_align,
        .wrap = button_opts->wrap,
        .scroll = button_opts->scroll,
        .indent = button_opts->indent
    };
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_button_opts ntg_button_opts_def()
{
    struct ntg_text_opts def_text_opts = ntg_text_opts_def();

    struct ntg_button_opts button_opts = {0};
    button_opts_from_text(&button_opts, &def_text_opts);

    return button_opts;
}

bool ntg_button_opts_are_eql(
        const struct ntg_button_opts* opts1,
        const struct ntg_button_opts* opts2)
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
           (opts1->scroll == opts2->scroll) &&
           (opts1->indent == opts2->indent));
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_button_init(
        ntg_button* button,
        const struct ntg_button_opts* opts,
        bool (*mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event),
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    struct ntg_object_vtable vtable = NTG_BUTTON_VTABLE_OBJECT;
    vtable.process_mouse_fn = mouse_fn;

    ntg_button_init_inherit(button, &vtable, &NTG_BUTTON_VTABLE_TEXT, &NTG_TYPE_BUTTON, &_status);
    if(!_status)
    {
        ntg_button_set_opts(button, opts);
        ntg_button_set_text_unsafe(button, "", 0, NULL);
    }

    button->hooks = (struct ntg_button_hooks) {0};

    ntg_vreturn(out_status, _status);
}

void ntg_button_deinit(ntg_button* button)
{
    if(!button) return;

    ntg_text_deinit(ntg_txt(button));

    button->hooks = (struct ntg_button_hooks) {0};
    button->__vtable = (struct ntg_object_vtable) {0};
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
        (*out_opts) = ntg_button_opts_def();
    else
        button_opts_from_text(out_opts, &(ntg_txt(button))->_opts);
}

void ntg_button_set_opts(ntg_button* button, const struct ntg_button_opts* opts)
{
    if(!button) return;

    struct ntg_button_opts new_opts = (opts ? (*opts) : ntg_button_opts_def());
    struct ntg_button_opts old_opts = {0};
    ntg_button_get_opts(button, &old_opts);

    if(ntg_button_opts_are_eql(&new_opts, &old_opts))
        return;

    struct ntg_text_opts new_opts_text = {0};
    button_opts_extract(&new_opts, &new_opts_text);

    ntg_text_set_opts(ntg_txt(button), &new_opts_text);

    if(button->hooks.on_opts_chng_fn)
    {
        button->hooks.on_opts_chng_fn(button, &old_opts, &new_opts);
    }
}

/* ------------------------------------------------------ */
/* CLICK HANDLER */
/* ------------------------------------------------------ */

void ntg_button_set_click_fn(
        ntg_button* button,
        bool (*mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event))
{
    if(!button) return; 

    button->__vtable.process_mouse_fn = mouse_fn;
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
            .data = (ntg_txt(button))->_text.data,
            .len = (ntg_txt(button))->_text.len,
        };
    }
}

void ntg_button_set_text_unsafe(
        ntg_button* button,
        const char* text,
        ntg_text_set_flags flags,
        int* out_status)
{
    if(!button)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_text_set_text_unsafe(ntg_txt(button), text, flags, out_status);
}

void ntg_button_set_text(
        ntg_button* button,
        const char* text,
        size_t len,
        ntg_text_set_flags flags,
        int* out_status)
{
    ntg_text_set_text(ntg_txt(button), text, len, flags, out_status);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_button_init_inherit(
        ntg_button* button,
        const struct ntg_object_vtable* object_vtable,
        const struct ntg_text_vtable* text_vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    if(!button || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_BUTTON))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    button->__vtable = (object_vtable ? (*object_vtable) : NTG_BUTTON_VTABLE_OBJECT);

    ntg_text_init_inherit(ntg_txt(button), &button->__vtable, text_vtable, type, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    init_default(button);

    ntg_object_set_focusable(ntg_obj(button), NTG_OBJECT_FOCUSABLE);
    ntg_object_set_clickable(ntg_obj(button), NTG_OBJECT_CLICKABLE_CONT);
}

struct ntg_object_measure
ntg_button_measure_fn(
        const ntg_object* _button,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena)
{
    return ntg_text_measure_fn(_button, orient, _layout_cache, arena);
}

void ntg_button_draw_fn(
        const ntg_object* _button,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena)
{
    ntg_text_draw_fn(_button, out_drawing, _layout_cache, arena);
}

void ntg_button_deinit_fn(ntg_object* _button)
{
    ntg_button_deinit(ntg_btn(_button));
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
    .focus_fn = ntg_button_focus_fn,
    .unfocus_fn = ntg_button_unfocus_fn
};

void ntg_button_post_draw_fn(
        const ntg_text* _button,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena) {}

const struct ntg_text_vtable NTG_BUTTON_VTABLE_TEXT = {
    .post_draw_fn = ntg_button_post_draw_fn
};
