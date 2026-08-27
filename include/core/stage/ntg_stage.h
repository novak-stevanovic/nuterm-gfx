#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "shared/ntg_shared.h"
#include "core/stage/ntg_stage_draw.h"
#include "base/entity/ntg_entity.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_stage
{
    ntg_entity _base;

    struct
    {
        void* data;
    } pub;

    struct
    {
        ntg_scene* scene;

        struct ntg_xy size;
        ntg_stage_draw drawing;

        bool in_loop;
        bool dirty;

    } ro;

};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_stage_init(ntg_stage* stage);

NTG_API int
ntg_stage_deinit(ntg_stage* stage);

NTG_API void
ntg_stage_deinit_void(void* _stage);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

/* Can be used for custom loop */
NTG_API bool
ntg_stage_compose(ntg_stage* stage, sarena* arena);

NTG_API int
ntg_stage_mark_dirty(ntg_stage* stage);

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

NTG_API int
ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_stage_feed_key(ntg_stage* stage, nt_key key);

NTG_API bool
ntg_stage_feed_mouse(ntg_stage* stage, nt_mouse mouse);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_stage_vtable
{
    struct ntg_entity_vtable base;

    bool (*handle_key_fn)(ntg_stage* stage, nt_key key);
    bool (*handle_mouse_fn)(ntg_stage* stage, nt_mouse mouse);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_stage_init_inherit(
        ntg_stage* stage,
        const struct ntg_stage_vtable* vtable,
        const ntg_type* type);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_stage_vtable NTG_STAGE_VTABLE;

NTG_API bool
ntg_stage_dispatch_key_fn(ntg_stage* stage, nt_key key);

NTG_API bool
ntg_stage_dispatch_mouse_fn(ntg_stage* stage, nt_mouse mouse);

NTG_API void
ntg_stage_deinit_fn(ntg_entity* _stage);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__stage_set_size(ntg_stage* stage, struct ntg_xy size);
void ntg__stage_clean(ntg_stage* stage);
void ntg__stage_enter_loop(ntg_stage* stage);
void ntg__stage_leave_loop(ntg_stage* stage);

#endif // NTG_STAGE_H
