#ifndef NTG_BOX_H
#define NTG_BOX_H

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

struct ntg_box_opts
{
    enum ntg_orient orient;
    enum ntg_align prim_align;
    enum ntg_align sec_align;
    size_t spacing;

    struct ntg_vcell bg;
};

static const struct ntg_box_opts NTG_BOX_OPTS_ZERO = {0};

/* ------------------------------------------------------ */

struct ntg_box
{
    ntg_widget _base;

    struct
    {
        struct ntg_box_opts opts;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_box_init(ntg_box* box, const struct ntg_box_opts* opts);

NTG_API int
ntg_box_deinit(ntg_box* box);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

NTG_API int
ntg_box_set_opts(ntg_box* box, const struct ntg_box_opts* opts);

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

NTG_API const struct ntg_widget_vec*
ntg_box_get_children(const ntg_box* box);

/* ------------------------------------------------------ */

NTG_API int
ntg_box_add_child(ntg_box* box, ntg_widget* child);

/* ------------------------------------------------------ */

NTG_API int
ntg_box_rm_child(ntg_box* box, ntg_widget* child);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_box_vtable_extend
{
    void (*placeholder_fn)(void);
};

struct ntg_box_vtable
{
    struct ntg_widget_vtable base;
    struct ntg_box_vtable_extend extend;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_box_init_inherit(
        ntg_box* box,
        const struct ntg_box_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_box_vtable NTG_BOX_VTABLE;

NTG_API int
ntg_box_measure_fn(
        const ntg_widget* _box,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

NTG_API int
ntg_box_constrain_fn(
        const ntg_widget* _box,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena,
        uint32_t* relayout);

NTG_API int
ntg_box_arrange_fn(
        const ntg_widget* _box,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_pos_map* out_pos_map,
        sarena* arena,
        uint32_t* relayout);

NTG_API void
ntg_box_child_rm_fn(ntg_widget* _box, ntg_widget* child);

NTG_API void
ntg_box_deinit_fn(ntg_object* _box);

#endif // NTG_BOX_H
