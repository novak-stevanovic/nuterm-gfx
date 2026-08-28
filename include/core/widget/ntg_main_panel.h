#ifndef NTG_MAIN_PANEL_H
#define NTG_MAIN_PANEL_H

#include "shared/ntg_shared.h"
#include "core/widget/ntg_widget.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_main_panel_opts
{
    struct ntg_vcell bg;
};

static const struct ntg_main_panel_opts NTG_MAIN_PANEL_OPTS_ZERO = {0};

enum ntg_main_panel_pos
{
    NTG_MAIN_PANEL_NORTH = 0,
    NTG_MAIN_PANEL_EAST,
    NTG_MAIN_PANEL_SOUTH,
    NTG_MAIN_PANEL_WEST,
    NTG_MAIN_PANEL_CENTER
};

/* ------------------------------------------------------ */

struct ntg_main_panel
{
    ntg_widget _base;

    struct
    {
        struct 
        {
            struct ntg_vcell bg;
        } opts;
        ntg_widget* children[5];
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_main_panel_init(ntg_main_panel* panel, const struct ntg_main_panel_opts* opts);

NTG_API int
ntg_main_panel_deinit(ntg_main_panel* panel);

NTG_API int
ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_widget* widget,
        enum ntg_main_panel_pos pos);

NTG_API int
ntg_main_panel_set_opts(ntg_main_panel* panel, const struct ntg_main_panel_opts* opts);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_main_panel_vtable
{
    struct ntg_widget_vtable base;
};

NTG_API int
ntg_main_panel_init_inherit(
        ntg_main_panel* panel,
        const struct ntg_main_panel_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_main_panel_vtable NTG_MAIN_PANEL_VTABLE;

NTG_API int
ntg_main_panel_measure_fn(
        const ntg_widget* _panel,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_main_panel_constrain_fn(
        const ntg_widget* _panel,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena,
        uint32_t* relayout);

NTG_API int
ntg_main_panel_arrange_fn(
        const ntg_widget* _panel,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_pos_map* out_pos_map,
        sarena* arena,
        uint32_t* relayout);

NTG_API void
ntg_main_panel_child_rm_fn(ntg_widget* _panel, ntg_widget* child);

NTG_API void
ntg_main_panel_deinit_fn(ntg_object* _panel);

#endif // NTG_MAIN_PANEL_H
