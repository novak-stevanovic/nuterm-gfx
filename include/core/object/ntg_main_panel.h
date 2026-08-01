#ifndef NTG_MAIN_PANEL_H
#define NTG_MAIN_PANEL_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_main_panel_opts
{
    struct ntg_vcell bg;
};

NTG_API struct ntg_main_panel_opts
ntg_main_panel_opts_default();

/* ------------------------------------------------------ */

NTG_API bool
ntg_main_panel_opts_are_eql(
        const struct ntg_main_panel_opts* opts1,
        const struct ntg_main_panel_opts* opts2);

/* ------------------------------------------------------ */

enum ntg_main_panel_pos
{
    NTG_MAIN_PANEL_NORTH = 0,
    NTG_MAIN_PANEL_EAST,
    NTG_MAIN_PANEL_SOUTH,
    NTG_MAIN_PANEL_WEST,
    NTG_MAIN_PANEL_CENTER
};

/* ------------------------------------------------------ */

struct ntg_main_panel_hooks
{
    void (*on_child_chng_fn)(
            ntg_main_panel* panel,
            ntg_object* old_child,
            ntg_object* new_child,
            enum ntg_main_panel_pos pos);

    void (*on_opts_chng_fn)(
            ntg_main_panel* panel,
            const struct ntg_main_panel_opts* old_opts,
            const struct ntg_main_panel_opts* new_opts);
};

/* ------------------------------------------------------ */

struct ntg_main_panel
{
    ntg_object __base;

    struct ntg_main_panel_opts _opts;
    ntg_object* _children[5];
    struct ntg_main_panel_hooks hooks;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */


NTG_API void
ntg_main_panel_init(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_main_panel_deinit(ntg_main_panel* panel);

/* ------------------------------------------------------ */


NTG_API void
ntg_main_panel_deinit_void(void* _panel);

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

NTG_API void
ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_object* object,
        enum ntg_main_panel_pos pos,
        int* out_status);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API void
ntg_main_panel_set_opts(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_main_panel_init_inherit(
        ntg_main_panel* panel,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

/* ------------------------------------------------------ */

NTG_API struct ntg_object_measure
ntg_main_panel_measure_fn(
        const ntg_object* _panel,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

NTG_API void
ntg_main_panel_constrain_fn(
        const ntg_object* _panel,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

NTG_API void
ntg_main_panel_arrange_fn(
        const ntg_object* _panel,
        ntg_object_pos_map* out_pos_map,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

NTG_API void
ntg_main_panel_child_rm_fn(ntg_object* _panel, ntg_object* child);

/* ------------------------------------------------------ */

NTG_API void
ntg_main_panel_deinit_fn(ntg_object* _panel);

NTG_API extern const struct ntg_object_vtable NTG_MAIN_PANEL_VTABLE;

#endif // NTG_MAIN_PANEL_H
