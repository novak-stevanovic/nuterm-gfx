#ifndef NTG_PROG_BAR_H
#define NTG_PROG_BAR_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

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
    struct ntg_vcell complete, uncomplete,
                     threshold;
};

NTG_API struct ntg_prog_bar_style
ntg_prog_bar_style_auto(void);

NTG_API bool
ntg_prog_bar_style_are_eql(
        const struct ntg_prog_bar_style* style1,
        const struct ntg_prog_bar_style* style2);

struct ntg_prog_bar_opts
{
    enum ntg_orient orient;
};

static const struct ntg_prog_bar_opts NTG_PROG_BAR_OPTS_ZERO = {0};

NTG_API bool
ntg_prog_bar_opts_are_eql(
        const struct ntg_prog_bar_opts* opts1,
        const struct ntg_prog_bar_opts* opts2);

/* ------------------------------------------------------ */

struct ntg_prog_bar
{
    ntg_object _base;

    struct
    {
        double prog;
        struct ntg_prog_bar_opts opts;
        struct ntg_prog_bar_style style;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_prog_bar_init(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts,
        const struct ntg_prog_bar_style* style);

NTG_API int
ntg_prog_bar_deinit(ntg_prog_bar* prog_bar);

NTG_API int
ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts);

NTG_API int
ntg_prog_bar_set_style(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_style* opts);

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
    struct ntg_object_vtable base;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_prog_bar_init_inherit(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_prog_bar_vtable NTG_PROG_BAR_VTABLE;

NTG_API int
ntg_prog_bar_measure_fn(
        const ntg_object* _prog_bar,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure);

NTG_API int
ntg_prog_bar_draw_fn(
        const ntg_object* _prog_bar,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_draw* out_drawing,
        sarena* arena);

NTG_API void
ntg_prog_bar_deinit_fn(ntg_entity* _prog_bar);

#endif // NTG_PROG_BAR_H
