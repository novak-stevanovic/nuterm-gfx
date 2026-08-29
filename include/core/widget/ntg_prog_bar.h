#ifndef NTG_PROG_BAR_H
#define NTG_PROG_BAR_H

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

struct ntg_prog_bar_style
{
    struct ntg_vcell complete, uncomplete, threshold;
};

static const struct ntg_prog_bar_style NTG_PROG_BAR_STYLE_ZERO = {0};

struct ntg_prog_bar_opts
{
    enum ntg_orient orient;
    struct ntg_prog_bar_style style; /* zero for auto */
};

static const struct ntg_prog_bar_opts NTG_PROG_BAR_OPTS_ZERO = {0};

/* ------------------------------------------------------ */

struct ntg_prog_bar
{
    ntg_widget _base;

    struct
    {
        double prog;
        struct ntg_prog_bar_opts opts;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_prog_bar_init(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts);

NTG_API int
ntg_prog_bar_deinit(ntg_prog_bar* prog_bar);

NTG_API int
ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts);

NTG_API int
ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double progress);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_prog_bar_vtable
{
    struct ntg_widget_vtable base;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_prog_bar_init_inherit(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_prog_bar_vtable NTG_PROG_BAR_VTABLE;

NTG_API int
ntg_prog_bar_measure_fn(
        const ntg_widget* _prog_bar,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_prog_bar_draw_fn(
        const ntg_widget* _prog_bar,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

NTG_API void
ntg_prog_bar_deinit_fn(ntg_object* _prog_bar);

#endif // NTG_PROG_BAR_H
