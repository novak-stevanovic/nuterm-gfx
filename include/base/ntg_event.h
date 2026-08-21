#ifndef NTG_EVENT_H
#define NTG_EVENT_H

#include "shared/ntg_shared.h"
#include "shared/ntg_str.h"
#include "nt_gfx.h"
#include "nt_event.h"
#include "thirdparty/genc.h"

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
GENC_VECTOR_DECLARE(ntg__event_sub_vec, struct ntg__event_sub, 1.5, )

#define NTG_EVENT_INVALID 0

struct ntg_event
{
    unsigned int type;
    void* source;
    void* data;
};

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
ntg_event_new(unsigned int type, void* source, void* data)
{
    if(type == NTG_EVENT_INVALID)
        return (struct ntg_event) {0};

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
        void* subscriber,
        void (*handler_fn)(void* subscriber, struct ntg_event event),
        ntg_event_binding* out_binding);

NTG_API int
ntg_event_unbind(ntg_event_binding* binding);

NTG_API int
ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event);

/* ========================================================================== */
/* EVENTS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* ntg_object */
/* ------------------------------------------------------ */

#define NTG_EVENT_OBJECT_KEY 2
struct ntg_event_object_key_dt
{
    const struct ntg_object_key* key;
};

#define NTG_EVENT_OBJECT_MOUSE 3
struct ntg_event_object_mouse_dt
{
    const struct ntg_object_mouse* mouse;
};

#define NTG_EVENT_OBJECT_FCS 4
/* No payload */

#define NTG_EVENT_OBJECT_UNFCS 5
/* No payload */

#define NTG_EVENT_OBJECT_PRNTSET 6
struct ntg_event_object_prntset_dt
{
    ntg_object* parent;
};

#define NTG_EVENT_OBJECT_PRNTRM 7
struct ntg_event_object_prntrm_dt
{
    ntg_object* parent;
};

#define NTG_EVENT_OBJECT_CHLDRM 8
struct ntg_event_object_chldrm_dt
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_CHLDADD 9
struct ntg_event_object_chldadd_dt
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_ANCHADD 10
struct ntg_event_object_anchadd_dt
{
    ntg_object* anchored;
};

#define NTG_EVENT_OBJECT_ANCHRM 11
struct ntg_event_object_anchrm_dt
{
    ntg_object* anchored;
};

#define NTG_EVENT_OBJECT_SCNSET 12
struct ntg_event_object_scnset_dt
{
    ntg_scene* scene;
};

#define NTG_EVENT_OBJECT_SCNRM 13
struct ntg_event_object_scnrm_dt
{
    ntg_scene* scene;
};

#define NTG_EVENT_OBJECT_BSSET 14
struct ntg_event_object_bsset_dt
{
    ntg_object* base;
};

#define NTG_EVENT_OBJECT_BSRM 15
struct ntg_event_object_bsrm_dt
{
    ntg_object* base;
};

#define NTG_EVENT_OBJECT_BDROPTCHG 16
struct ntg_event_object_bdroptchg_dt
{
    const struct ntg_bdr_opts* old_opts;
    const struct ntg_bdr_opts* new_opts;
};

/* ---------------------------------- */

#define NTG_EVENT_OBJECT_PADOPTCHG 17
struct ntg_event_object_padoptchg_dt
{
    const struct ntg_pad_opts* old_opts;
    const struct ntg_pad_opts* new_opts;
};

/* ---------------------------------- */

#define NTG_EVENT_OBJECT_LAYOPTCHG 18
struct ntg_event_object_layoptchg_dt
{
    const struct ntg_lay_opts* old_opts;
    const struct ntg_lay_opts* new_opts;
};

#define NTG_EVENT_OBJECT_SZCHG 19
struct ntg_event_object_szchg_dt
{
    const struct ntg_xy* old_size;
    const struct ntg_xy* new_size;
};

#define NTG_EVENT_OBJECT_CONTSZCHG 20
struct ntg_event_object_contszchg_dt
{
    const struct ntg_xy* old_size;
    const struct ntg_xy* new_size;
};

#define NTG_EVENT_OBJECT_POSCHG 21
struct ntg_event_object_poschg_dt
{
    const struct ntg_xy* old_pos;
    const struct ntg_xy* new_pos;
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

#define NTG_EVENT_LABEL_OPTCHG 101
struct ntg_event_label_optchg_dt
{
    const struct ntg_label_opts* old_opts;
    const struct ntg_label_opts* new_opts;
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

#define NTG_EVENT_BUTTON_OPTCHG 111
struct ntg_event_button_optchg_dt
{
    const struct ntg_button_opts* old_opts;
    const struct ntg_button_opts* new_opts;
};

/* ------------------------------------------------------ */
/* ntg_box */
/* ------------------------------------------------------ */

#define NTG_EVENT_BOX_OPTCHG 120
struct ntg_event_box_optchg_dt
{
    const struct ntg_box_opts* old_opts;
    const struct ntg_box_opts* new_opts;
};

#define NTG_EVENT_BOX_CHLDADD 121
struct ntg_event_box_chldadd_dt
{
    ntg_object* child;
};

#define NTG_EVENT_BOX_CHLDRM 122
struct ntg_event_box_chldrm_dt
{
    ntg_object* child;
};

/* ------------------------------------------------------ */
/* ntg_main_panel */
/* ------------------------------------------------------ */

#define NTG_EVENT_MAIN_PANEL_OPTCHG 130
struct ntg_event_main_panel_optchg_dt
{
    const struct ntg_main_panel_opts* old_opts;
    const struct ntg_main_panel_opts* new_opts;
};

#define NTG_EVENT_MAIN_PANEL_CHLDCHG 131
struct ntg_event_main_panel_chldchg_dt
{
    ntg_object* old_child;
    ntg_object* new_child;
    int pos;
};

/* ------------------------------------------------------ */
/* ntg_clr_block */
/* ------------------------------------------------------ */

#define NTG_EVENT_CLR_BLOCK_CLRCHG 140
struct ntg_event_clr_block_clrchg_dt
{
    struct nt_color old_color;
    struct nt_color new_color;
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

#define NTG_EVENT_PROG_BAR_OPTCHG 151
struct ntg_event_prog_bar_optchg_dt
{
    const struct ntg_prog_bar_opts* old_opts;
    const struct ntg_prog_bar_opts* new_opts;
};

/* ------------------------------------------------------ */
/* ntg_scene */
/* ------------------------------------------------------ */

#define NTG_EVENT_SCENE_KEY 200
struct ntg_event_scene_key_dt
{
    struct nt_key key;
};

#define NTG_EVENT_SCENE_MOUSE 201
struct ntg_event_scene_mouse_dt
{
    struct nt_mouse mouse;
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

#define NTG_EVENT_SCENE_OBJADD 205
struct ntg_event_scene_objadd_dt
{
    ntg_object* object;
};

#define NTG_EVENT_SCENE_OBJRM 206
struct ntg_event_scene_objrm_dt
{
    ntg_object* object;
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
    struct nt_key key;
};

#define NTG_EVENT_STAGE_MOUSE 301
struct ntg_event_stage_mouse_dt
{
    struct nt_mouse mouse;
};

#define NTG_EVENT_STAGE_SCNCHG 302
struct ntg_event_stage_scnchg_dt
{
    ntg_scene* old_scene;
    ntg_scene* new_scene;
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
    ntg_object* old_focused;
    ntg_object* new_focused;
};

#define NTG_EVENT_FCS_MANAGER_SCPSH 307
struct ntg_event_fcs_manager_scpsh_dt
{
    ntg_fcs_scope* scope;
};

#define NTG_EVENT_FCS_MANAGER_SCPOP 308
struct ntg_event_fcs_manager_scpop_dt
{
    ntg_fcs_scope* scope;
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
