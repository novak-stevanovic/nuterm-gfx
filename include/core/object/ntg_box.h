#ifndef NTG_BOX_H
#define NTG_BOX_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_box_opts
{
    ntg_orient orient;
    ntg_align prim_align;
    ntg_align sec_align;
    size_t spacing;

    struct ntg_vcell bg;
};

/* Creates horizontal box defaults with start alignment on both axes, zero
 * spacing, and the default full-space background cell.
 *
 * RETURN VALUE:
 * The default `ntg_box_opts` value. */
NTG_API struct ntg_box_opts
ntg_box_opts_def();

/* ------------------------------------------------------ */

/* Compares two box option values. Pointer identity counts as equal; otherwise a
 * `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_box_opts_are_eq(
        const struct ntg_box_opts* opts1,
        const struct ntg_box_opts* opts2);

/* ------------------------------------------------------ */

struct ntg_box_hooks
{
    void (*on_child_add_fn)(ntg_box* box, ntg_object* child);
    void (*on_child_rm_fn)(ntg_box* box, ntg_object* child);

    void (*on_opts_chng_fn)(
            ntg_box* box,
            const struct ntg_box_opts* old_opts,
            const struct ntg_box_opts* new_opts);
};

/* ------------------------------------------------------ */

struct ntg_box
{
    ntg_object __base;

    struct ntg_box_hooks hooks;
    struct ntg_box_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes a box container and its base object. A `NULL` options pointer
 * selects defaults.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `box` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object or box resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_box_init(
        ntg_box* box,
        const struct ntg_box_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Detaches all children and releases resources owned by a box. Passing `NULL`
 * has no effect. */
NTG_API void
ntg_box_deinit(ntg_box* box);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_box_deinit`, intended for cleanup callbacks. */
NTG_API void
ntg_box_deinit_(void* _box);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Updates box orientation, alignment, spacing, and background. A `NULL` options
 * pointer applies defaults; unchanged options are ignored. */
NTG_API void
ntg_box_set_opts(ntg_box* box, const struct ntg_box_opts* opts);

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

/* Gets the box-owned vector of child object pointers in insertion order.
 *
 * RETURN VALUE:
 * A read-only vector pointer, or `NULL` when `box` is `NULL`. */
NTG_API const struct ntg_object_vec*
ntg_box_get_children(const ntg_box* box);

/* ------------------------------------------------------ */

/* Appends `child` to the box and attaches it to the box object tree. Existing
 * parent, scene-root, or anchor relationships on the child are removed by the
 * base attachment API.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `box` or `child` is `NULL`.
 * - `NTG_ERR_MAX_CHILDREN`: the object child limit has been reached.
 * - `NTG_ERR_ALLOC_FAIL`: the child vector cannot grow.
 * - `NTG_ERR_UNEXPECTED`: the base attachment API reports another failure,
 *   including `child == box`. */
NTG_API void
ntg_box_add_child(ntg_box* box, ntg_object* child, int* out_status);

/* ------------------------------------------------------ */

/* Removes `child` from the box when it is a direct child. Missing or `NULL`
 * inputs are ignored. */
NTG_API void
ntg_box_rm_child(ntg_box* box, ntg_object* child);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* Initializes the box portion of an object derived from `NTG_TYPE_BOX`, using
 * the supplied virtual table and concrete type descriptor. A `NULL` options
 * pointer selects defaults.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `box` or `type` is `NULL`.
 * - `NTG_ERR_INVALID_TYPE`: `type` is not derived from `NTG_TYPE_BOX`.
 * - `NTG_ERR_BAD_VTABLE`: `vtable` or `vtable->deinit_fn` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object or box resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_box_init_inherit(
        ntg_box* box,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        const struct ntg_box_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Implements box measurement for an object virtual table. */
NTG_API struct ntg_object_measure
ntg_box_measure_fn(
        const ntg_object* _box,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Assigns constrained child sizes for the requested axis. */
NTG_API void
ntg_box_constrain_fn(
        const ntg_object* _box,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Assigns child positions inside the box content area. */
NTG_API void
ntg_box_arrange_fn(
        const ntg_object* _box,
        ntg_object_pos_map* out_pos_map,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Marks the box layout dirty after a base-level child removal. */
NTG_API void
ntg_box_child_rm_fn(ntg_object* _box, ntg_object* child);

/* ------------------------------------------------------ */

/* Virtual deinitializer that dispatches to `ntg_box_deinit`. */
NTG_API void
ntg_box_deinit_fn(ntg_object* _box);

/* Default virtual table used by `ntg_box_init`. */
NTG_API extern const struct ntg_object_vtable NTG_BOX_VTABLE;

#endif // NTG_BOX_H
