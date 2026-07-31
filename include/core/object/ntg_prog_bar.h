#ifndef NTG_PROG_BAR_H
#define NTG_PROG_BAR_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_prog_bar_style
{
    struct ntg_vcell complete_style;
    struct ntg_vcell uncomplete_style;
    struct ntg_vcell threshold_style;
};

NTG_API struct ntg_prog_bar_style
ntg_prog_bar_style_def();

/* ------------------------------------------------------ */


NTG_API bool
ntg_prog_bar_style_are_eql(
        const struct ntg_prog_bar_style* style1,
        const struct ntg_prog_bar_style* style2);

/* ------------------------------------------------------ */

struct ntg_prog_bar_opts
{
    struct ntg_prog_bar_style style;
    ntg_orient orient;
};


NTG_API struct ntg_prog_bar_opts
ntg_prog_bar_opts_def();

/* ------------------------------------------------------ */


NTG_API bool
ntg_prog_bar_opts_are_eql(
        const struct ntg_prog_bar_opts* opts1,
        const struct ntg_prog_bar_opts* opts2);

/* ------------------------------------------------------ */

struct ntg_prog_bar_hooks
{
    void (*on_prog_chng_fn)(
            ntg_prog_bar* prog_bar,
            double old_prog,
            double new_prog);

    void (*on_opts_chng_fn)(
            ntg_prog_bar* prog_bar,
            const struct ntg_prog_bar_opts* old_opts,
            const struct ntg_prog_bar_opts* new_opts);
};

/* ------------------------------------------------------ */

struct ntg_prog_bar
{
    ntg_object __base;

    double _prog; 
    struct ntg_prog_bar_opts _opts;
    struct ntg_prog_bar_hooks hooks;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */


NTG_API void
ntg_prog_bar_init(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_prog_bar_deinit(ntg_prog_bar* prog_bar);

/* ------------------------------------------------------ */


NTG_API void
ntg_prog_bar_deinit_void(void* _prog_bar);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */


NTG_API void
ntg_prog_bar_set_opts(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts);

/* ------------------------------------------------------ */
/* PROGRESS */
/* ------------------------------------------------------ */


NTG_API void
ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double progress);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */


NTG_API void
ntg_prog_bar_init_inherit(
        ntg_prog_bar* prog_bar,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API struct ntg_object_measure
ntg_prog_bar_measure_fn(
        const ntg_object* _prog_bar,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */


NTG_API void
ntg_prog_bar_draw_fn(
        const ntg_object* _prog_bar,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

NTG_API void
ntg_prog_bar_deinit_fn(ntg_object* _prog_bar);

NTG_API extern const struct ntg_object_vtable NTG_PROG_BAR_VTABLE;

#endif // NTG_PROG_BAR_H
