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

/* Creates main-panel defaults with the default full-space background cell.
 *
 * RETURN VALUE:
 * The default `ntg_main_panel_opts` value. */
NTG_API struct ntg_main_panel_opts
ntg_main_panel_opts_def();

/* ------------------------------------------------------ */

/* Compares two main-panel option values. Pointer identity counts as equal;
 * otherwise a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_main_panel_opts_are_eq(
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

/* Initializes a five-region main panel and its base object. A `NULL` options
 * pointer selects defaults.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `panel` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_main_panel_init(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Detaches region children and releases resources owned by a main panel.
 * Passing `NULL` has no effect. */
NTG_API void
ntg_main_panel_deinit(ntg_main_panel* panel);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_main_panel_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_main_panel_deinit_(void* _panel);

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

/* Replaces the object in one north/east/south/west/center region. Passing
 * `NULL` as `object` clears that region; the previous child is detached.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `panel` is `NULL` or `pos` is outside the five
 *   defined regions.
 * - `NTG_ERR_MAX_CHILDREN`: the object child limit has been reached.
 * - `NTG_ERR_ALLOC_FAIL`: tree storage cannot grow.
 * - `NTG_ERR_UNEXPECTED`: the base attachment API reports another failure,
 *   including `object == panel`. */
NTG_API void
ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_object* object,
        enum ntg_main_panel_pos pos,
        int* out_status);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Updates the main-panel background. A `NULL` options pointer applies defaults;
 * unchanged options are ignored. */
NTG_API void
ntg_main_panel_set_opts(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* Initializes the main-panel portion of an object derived from
 * `NTG_TYPE_MAIN_PANEL`, using the supplied virtual table and concrete type
 * descriptor. A `NULL` options pointer selects defaults.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `panel` or `type` is `NULL`.
 * - `NTG_ERR_INVALID_TYPE`: `type` is not derived from
 *   `NTG_TYPE_MAIN_PANEL`.
 * - `NTG_ERR_BAD_VTABLE`: `vtable` or `vtable->deinit_fn` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_main_panel_init_inherit(
        ntg_main_panel* panel,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        const struct ntg_main_panel_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Implements main-panel measurement for an object virtual table. */
NTG_API struct ntg_object_measure
ntg_main_panel_measure_fn(
        const ntg_object* _panel,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Assigns constrained sizes to populated main-panel regions. */
NTG_API void
ntg_main_panel_constrain_fn(
        const ntg_object* _panel,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Assigns positions to populated main-panel regions. */
NTG_API void
ntg_main_panel_arrange_fn(
        const ntg_object* _panel,
        ntg_object_pos_map* out_pos_map,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Clears the matching region after a base-level child removal. */
NTG_API void
ntg_main_panel_child_rm_fn(ntg_object* _panel, ntg_object* child);

/* ------------------------------------------------------ */

/* Virtual deinitializer that dispatches to `ntg_main_panel_deinit`. */
NTG_API void
ntg_main_panel_deinit_fn(ntg_object* _panel);

/* Default virtual table used by `ntg_main_panel_init`. */
NTG_API extern const struct ntg_object_vtable NTG_MAIN_PANEL_VTABLE;

#endif // NTG_MAIN_PANEL_H
