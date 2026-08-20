#ifndef NTG_BOX_H
#define NTG_BOX_H

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

struct ntg_box_opts
{
    enum ntg_orient orient;
    enum ntg_align prim_align;
    enum ntg_align sec_align;
    size_t spacing;

    struct ntg_vcell bg;
};


NTG_API struct ntg_box_opts
ntg_box_opts_default(void);

/* ------------------------------------------------------ */


NTG_API bool
ntg_box_opts_are_eql(
        const struct ntg_box_opts* opts1,
        const struct ntg_box_opts* opts2);

/* ------------------------------------------------------ */

/* ------------------------------------------------------ */

struct ntg_box
{
    struct
    {
        ntg_object base;
    } priv;

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
ntg_box_init(
        ntg_box* box,
        const struct ntg_box_opts* opts);

/* ------------------------------------------------------ */


NTG_API int
ntg_box_deinit(ntg_box* box);

/* ------------------------------------------------------ */


NTG_API void
ntg_box_deinit_void(void* _box);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */


NTG_API int
ntg_box_set_opts(ntg_box* box, const struct ntg_box_opts* opts);

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

NTG_API const struct ntg_objptr_vec*
ntg_box_get_children(const ntg_box* box);

/* ------------------------------------------------------ */


NTG_API int
ntg_box_add_child(ntg_box* box, ntg_object* child);

/* ------------------------------------------------------ */


NTG_API int
ntg_box_rm_child(ntg_box* box, ntg_object* child);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */


NTG_API int
ntg_box_init_inherit(
        ntg_box* box,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt);

/* ------------------------------------------------------ */


NTG_API int
ntg_box_measure_fn(
        const ntg_object* _box,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure);

/* ------------------------------------------------------ */


NTG_API int
ntg_box_constrain_fn(
        const ntg_object* _box,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena,
        uint32_t* relayout);

/* ------------------------------------------------------ */


NTG_API int
ntg_box_arrange_fn(
        const ntg_object* _box,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_pos_map* out_pos_map,
        sarena* arena,
        uint32_t* relayout);

/* ------------------------------------------------------ */

NTG_API void
ntg_box_child_rm_fn(ntg_object* _box, ntg_object* child);

/* ------------------------------------------------------ */

NTG_API void
ntg_box_deinit_fn(ntg_object* _box);

NTG_API extern const struct ntg_object_vtable NTG_BOX_VTABLE;

#endif // NTG_BOX_H
