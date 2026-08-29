#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

static inline struct nt_gfx
get_effective_gfx(const ntg_button* button)
{
    return (ntg_widget_is_focused(ntg_wgt(button)) ?
        button->ro.focused_gfx :
        (button->ro.enabled ? button->ro.enabled_gfx : button->ro.disabled_gfx));
}

static void update_text_gfx(ntg_button* button)
{
    struct ntg_text_opts text_opts = {
        .orient = ntg_txt(button)->ro.opts.orient,
        .gfx = get_effective_gfx(button),
        .wrap = ntg_txt(button)->ro.opts.wrap,
        .line_mode = ntg_txt(button)->ro.opts.line_mode,
        .prim_align = ntg_txt(button)->ro.opts.prim_align,
        .sec_align = ntg_txt(button)->ro.opts.sec_align,
        .bg_mode = ntg_txt(button)->ro.opts.bg_mode,
        .indent = ntg_txt(button)->ro.opts.indent
    };

    ntg_text_set_opts(ntg_txt(button), &text_opts);
}

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

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_button_get_opts(ntg_button* button, struct ntg_button_opts* out_opts)
{
    if(!button || !out_opts) return NTG_ERR_INV_ARG;

    (*out_opts) = (struct ntg_button_opts) {
        .orient = ntg_txt(button)->ro.opts.orient,
        .enabled_gfx = button->ro.enabled_gfx,
        .disabled_gfx = button->ro.disabled_gfx,
        .focused_gfx = button->ro.focused_gfx,
        .wrap = ntg_txt(button)->ro.opts.wrap,
        .line_mode = ntg_txt(button)->ro.opts.line_mode,
        .prim_align = ntg_txt(button)->ro.opts.prim_align,
        .sec_align = ntg_txt(button)->ro.opts.sec_align,
        .bg_mode = ntg_txt(button)->ro.opts.bg_mode,
        .indent = ntg_txt(button)->ro.opts.indent
    };

    return 0;
}

int ntg_button_set_opts(ntg_button* button, const struct ntg_button_opts* opts)
{
    if(!button) return NTG_ERR_INV_ARG;

    struct ntg_button_opts opts_final = (opts ? (*opts) : NTG_BUTTON_OPTS_ZERO);

    if(nt_gfx_are_eql(opts_final.enabled_gfx, NT_GFX_ZERO) &&
        nt_gfx_are_eql(opts_final.disabled_gfx, NT_GFX_ZERO) &&
        nt_gfx_are_eql(opts_final.focused_gfx, NT_GFX_ZERO))
    {
        opts_final.focused_gfx = nt_gfx_invert_style(NT_GFX_ZERO);
    }

    button->ro.enabled_gfx = opts_final.enabled_gfx;
    button->ro.disabled_gfx = opts_final.disabled_gfx;
    button->ro.focused_gfx = opts_final.focused_gfx;

    struct ntg_text_opts text_opts = {
        .orient = opts_final.orient,
        .gfx = get_effective_gfx(button),
        .wrap = opts_final.wrap,
        .line_mode = opts_final.line_mode,
        .prim_align = opts_final.prim_align,
        .sec_align = opts_final.sec_align,
        .bg_mode = opts_final.bg_mode,
        .indent = opts_final.indent
    };
    
    ntg_text_set_opts(ntg_txt(button), &text_opts);

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

int ntg_button_enable(ntg_button* button)
{
    if(!button) return NTG_ERR_INV_ARG;

    if(button->ro.enabled) return 0;

    ntg_widget_set_focusable(ntg_wgt(button), NTG_WIDGET_FOCUSABLE);
    ntg_widget_set_clickable(ntg_wgt(button), NTG_WIDGET_CLICKABLE_CONT_PAD);

    button->ro.enabled = true;

    update_text_gfx(button);

    struct ntg_button_vtable* vtable = ntg_btn_vtbl(button);
    if(vtable->enable_fn)
        vtable->enable_fn(button);

    ntg_object_event_raise(ntg_obj(button), NTG_EVENT_BUTTON_ENABLE, NULL);

    return 0;
}

int ntg_button_disable(ntg_button* button)
{
    if(!button) return NTG_ERR_INV_ARG;

    if(!button->ro.enabled) return 0;

    ntg_widget_set_focusable(ntg_wgt(button), NTG_WIDGET_UNFOCUSABLE);
    ntg_widget_set_clickable(ntg_wgt(button), NTG_WIDGET_UNCLICKABLE);

    button->ro.enabled = false;

    update_text_gfx(button);

    struct ntg_button_vtable* vtable = ntg_btn_vtbl(button);
    if(vtable->disable_fn)
        vtable->disable_fn(button);

    ntg_object_event_raise(ntg_obj(button), NTG_EVENT_BUTTON_DISABLE, NULL);

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

int ntg_button_set_text_cstr(ntg_button* button, const char* text)
{
    if(!button)
        return NTG_ERR_INV_ARG;

    return ntg_text_set_text_cstr(ntg_txt(button), text);
}

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

    ntg_button_enable(button);

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
            .resize_cont_fn = ntg_text_cont_resize_fn,
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
    return ntg_text_measure_fn(_button, layout_dt, orient,
                               arena, relayout, out_measure);
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

    update_text_gfx(ntg_btn(_button));
}

void ntg_button_unfocus_fn(ntg_widget* _button)
{
    ntg_text_unfocus_fn(_button);

    update_text_gfx(ntg_btn(_button));
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

