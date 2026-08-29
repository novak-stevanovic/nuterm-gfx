#ifndef NTG_EVENT_H
#define NTG_EVENT_H

#include "shared/ntg_shared.h"
#include "shared/ntg_str.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* Generic event system.
 *
 * A subscriber-owned ntg_event_binding token must be zero-initialized before
 * its first use. Each token can be tied to at most one delegate at a time.
 *
 * The subscriber can unsubscribe by providing the token to
 * ntg_event_unbind(). If a delegate is destroyed, all of its bindings are
 * released. */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg__event_sub;
GENC_VECTOR_DECLARE(ntg__event_sub_vec, struct ntg__event_sub, )

#define NTG_EVENT_INVALID 0

struct ntg_event
{
    unsigned int type;
    ntg_object* source;
    void* data;
};

static const struct ntg_event NTG_EVENT_ZERO = {0};

struct ntg_event_binding
{
    struct
    {
        ntg_event_delegate* delegate;
    } priv;
};

struct ntg_event_delegate
{
    struct
    {
        struct ntg__event_sub_vec subs;
        bool raise;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static inline struct ntg_event
ntg_event_new(unsigned int type, ntg_object* source, void* data)
{
    if(type == NTG_EVENT_INVALID) return NTG_EVENT_ZERO;

    return (struct ntg_event) {
        .type = type,
        .source = source,
        .data = data
    };
}

NTG_API int
ntg_event_delegate_init(ntg_event_delegate* delegate);

NTG_API int
ntg_event_delegate_deinit(ntg_event_delegate* delegate);

NTG_API int
ntg_event_bind(
        ntg_event_delegate* delegate,
        ntg_object* subscriber,
        void (*handler_fn)(ntg_object* subscriber, struct ntg_event event),
        ntg_event_binding* out_binding);

NTG_API int
ntg_event_unbind(ntg_event_binding* binding);

NTG_API int
ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event);

/* ========================================================================== */
/* EVENTS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* ntg_widget */
/* ------------------------------------------------------ */

#define NTG_EVENT_WIDGET_KEY 2
struct ntg_event_widget_key_dt
{
    const struct ntg_widget_key* key;
};

#define NTG_EVENT_WIDGET_MOUSE 3
struct ntg_event_widget_mouse_dt
{
    const struct ntg_widget_mouse* mouse;
};

#define NTG_EVENT_WIDGET_FCS 4
/* No payload */

#define NTG_EVENT_WIDGET_UNFCS 5
/* No payload */

#define NTG_EVENT_WIDGET_PRNTSET 6
struct ntg_event_widget_prntset_dt
{
    ntg_widget* parent;
};

#define NTG_EVENT_WIDGET_PRNTRM 7
struct ntg_event_widget_prntrm_dt
{
    ntg_widget* parent;
};

#define NTG_EVENT_WIDGET_CHLDRM 8
struct ntg_event_widget_chldrm_dt
{
    ntg_widget* child;
};

#define NTG_EVENT_WIDGET_CHLDADD 9
struct ntg_event_widget_chldadd_dt
{
    ntg_widget* child;
};

#define NTG_EVENT_WIDGET_ANCHADD 10
struct ntg_event_widget_anchadd_dt
{
    ntg_widget* anchored;
};

#define NTG_EVENT_WIDGET_ANCHRM 11
struct ntg_event_widget_anchrm_dt
{
    ntg_widget* anchored;
};

#define NTG_EVENT_WIDGET_SCNSET 12
struct ntg_event_widget_scnset_dt
{
    ntg_scene* scene;
};

#define NTG_EVENT_WIDGET_SCNRM 13
struct ntg_event_widget_scnrm_dt
{
    ntg_scene* scene;
};

#define NTG_EVENT_WIDGET_BSSET 14
struct ntg_event_widget_bsset_dt
{
    ntg_widget* base;
};

#define NTG_EVENT_WIDGET_BSRM 15
struct ntg_event_widget_bsrm_dt
{
    ntg_widget* base;
};

#define NTG_EVENT_WIDGET_SZCHG 19
struct ntg_event_widget_szchg_dt
{
    const ntg_xy* old_size;
    const ntg_xy* new_size;
};

#define NTG_EVENT_WIDGET_CONTSZCHG 20
struct ntg_event_widget_contszchg_dt
{
    const ntg_xy* old_size;
    const ntg_xy* new_size;
};

#define NTG_EVENT_WIDGET_POSCHG 21
struct ntg_event_widget_poschg_dt
{
    const ntg_xy* old_pos;
    const ntg_xy* new_pos;
};

/* ------------------------------------------------------ */
/* ntg_label */
/* ------------------------------------------------------ */

#define NTG_EVENT_LABEL_TXTCHG 100
struct ntg_event_label_txtchg_dt
{
    struct ntg_str_view old_text;
    struct ntg_str_view new_text;
};

/* ------------------------------------------------------ */
/* ntg_button */
/* ------------------------------------------------------ */

#define NTG_EVENT_BUTTON_TXTCHG 110
struct ntg_event_button_txtchg_dt
{
    struct ntg_str_view old_text;
    struct ntg_str_view new_text;
};

#define NTG_EVENT_BUTTON_ENABLE 111
#define NTG_EVENT_BUTTON_DISABLE 112

/* ------------------------------------------------------ */
/* ntg_box */
/* ------------------------------------------------------ */

#define NTG_EVENT_BOX_CHLDADD 121
struct ntg_event_box_chldadd_dt
{
    ntg_widget* child;
};

#define NTG_EVENT_BOX_CHLDRM 122
struct ntg_event_box_chldrm_dt
{
    ntg_widget* child;
};

/* ------------------------------------------------------ */
/* ntg_panel */
/* ------------------------------------------------------ */

#define NTG_EVENT_PANEL_CHLDADD 131
struct ntg_event_panel_chldadd_dt
{
    ntg_widget* child;
    int pos; // enum ntg_panel_pos
};

#define NTG_EVENT_PANEL_CHLDRM 132
struct ntg_event_panel_chldrm_dt
{
    ntg_widget* child;
    int pos;
};

/* ------------------------------------------------------ */
/* ntg_clr_block */
/* ------------------------------------------------------ */

#define NTG_EVENT_CLR_BLOCK_CLRCHG 140
struct ntg_event_clr_block_clrchg_dt
{
    nt_color old_color;
    nt_color new_color;
};

/* ------------------------------------------------------ */
/* ntg_prog_bar */
/* ------------------------------------------------------ */

#define NTG_EVENT_PROG_BAR_PROGCHG 150
struct ntg_event_prog_bar_progchg_dt
{
    double old_prog;
    double new_prog;
};

/* ------------------------------------------------------ */
/* ntg_scene */
/* ------------------------------------------------------ */

#define NTG_EVENT_SCENE_KEY 200
struct ntg_event_scene_key_dt
{
    nt_key key;
};

#define NTG_EVENT_SCENE_MOUSE 201
struct ntg_event_scene_mouse_dt
{
    nt_mouse mouse;
};

#define NTG_EVENT_SCENE_SZCHG 202
struct ntg_event_scene_szchg_dt
{
    size_t old_x, old_y;
    size_t new_x, new_y;
};

#define NTG_EVENT_SCENE_ENTER 203
struct ntg_event_scene_enter_dt
{
    ntg_stage* stage;
};

#define NTG_EVENT_SCENE_LEAVE 204
struct ntg_event_scene_leave_dt
{
    ntg_stage* stage;
};

#define NTG_EVENT_SCENE_WGTADD 205
struct ntg_event_scene_wgtadd_dt
{
    ntg_widget* widget;
};

#define NTG_EVENT_SCENE_WGTRM 206
struct ntg_event_scene_wgtrm_dt
{
    ntg_widget* widget;
};

#define NTG_EVENT_SCENE_LAYPRE 207
/* No payload */

#define NTG_EVENT_SCENE_LAYPOST 208
/* No payload */

/* ------------------------------------------------------ */
/* ntg_stage */
/* ------------------------------------------------------ */

#define NTG_EVENT_STAGE_KEY 300
struct ntg_event_stage_key_dt
{
    nt_key key;
};

#define NTG_EVENT_STAGE_MOUSE 301
struct ntg_event_stage_mouse_dt
{
    nt_mouse mouse;
};

#define NTG_EVENT_STAGE_SCNSET 302
struct ntg_event_stage_scnset_dt
{
    ntg_scene* scene;
};

#define NTG_EVENT_STAGE_SCNRM 311
struct ntg_event_stage_scnrm_dt
{
    ntg_scene* scene;
};

#define NTG_EVENT_STAGE_SZCHG 303
struct ntg_event_stage_szchg_dt
{
    size_t old_x, old_y;
    size_t new_x, new_y;
};

#define NTG_EVENT_STAGE_ENTER 304
/* No payload */

#define NTG_EVENT_STAGE_LEAVE 305
/* No payload */

#define NTG_EVENT_STAGE_CMPSPRE 309
/* No payload */

#define NTG_EVENT_STAGE_CMPSPOST 310
/* No payload */

/* ------------------------------------------------------ */
/* ntg_fcs_manager */
/* ------------------------------------------------------ */

#define NTG_EVENT_FCS_MANAGER_FCSCHG 306
struct ntg_event_fcs_manager_fcschg_dt
{
    ntg_widget* old_focused;
    ntg_widget* new_focused;
};

#define NTG_EVENT_FCS_MANAGER_SCPSH 307
struct ntg_event_fcs_manager_scpsh_dt
{
    const struct ntg_fcs_scope* scope;
};

#define NTG_EVENT_FCS_MANAGER_SCPOP 308
struct ntg_event_fcs_manager_scpop_dt
{
    const struct ntg_fcs_scope* scope;
};

/* ------------------------------------------------------ */
/* ntg_renderer */
/* ------------------------------------------------------ */

#define NTG_EVENT_RENDERER_RNDRPRE 400
#define NTG_EVENT_RENDERER_RNDRPOST 401
struct ntg_event_renderer_rndr_dt
{
    const ntg_stage_draw* drawing;
    sarena* arena;
};

#endif // NTG_EVENT_H
