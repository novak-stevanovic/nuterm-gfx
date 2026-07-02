#ifndef NTG_MAIN_PANEL_H
#define NTG_MAIN_PANEL_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_main_panel_opts
{
    struct ntg_vcell bg;
};

/* Creates main-panel defaults with the default full-space background cell. */

/* RETURN VALUE: The default `ntg_main_panel_opts` value. */
struct ntg_main_panel_opts ntg_main_panel_opts_def();
/* Compares two main-panel option values. Pointer identity counts as equal; otherwise a `NULL` */
/* value differs from a non-`NULL` value. */

/* RETURN VALUE: `true` when all relevant fields are equal; otherwise `false`. */
bool ntg_main_panel_opts_are_eq(
        const struct ntg_main_panel_opts* opts1,
        const struct ntg_main_panel_opts* opts2);

enum ntg_main_panel_pos
{
    NTG_MAIN_PANEL_NORTH = 0,
    NTG_MAIN_PANEL_EAST,
    NTG_MAIN_PANEL_SOUTH,
    NTG_MAIN_PANEL_WEST,
    NTG_MAIN_PANEL_CENTER
};

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

struct ntg_main_panel
{
    ntg_object __base;

    struct ntg_main_panel_opts _opts;
    ntg_object* _children[5];
    struct ntg_main_panel_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Initializes a five-region main panel and its base object. A `NULL` options pointer selects */
/* defaults. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `panel` is `NULL`. */
/* - `NTG_ERR_ALLOC_FAIL`: panel or base-object resources cannot be allocated. */
/* - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
void ntg_main_panel_init(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts,
        int* out_status);
/* Detaches region children and releases resources owned by a main panel. Passing `NULL` has no */
/* effect. */
void ntg_main_panel_deinit(ntg_main_panel* panel);
/* Void-pointer adapter for `ntg_main_panel_deinit`, intended for cleanup callbacks. */
void ntg_main_panel_deinit_(void* _panel);

/* Replaces the object in one north/east/south/west/center region. Passing `NULL` as `object` */
/* clears that region; the previous child is detached. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `panel` is `NULL` or `pos` is outside the five defined regions. */
/* - `NTG_ERR_MAX_CHILDREN`: the object child limit has been reached. */
/* - `NTG_ERR_ALLOC_FAIL`: tree storage cannot grow. */
/* - `NTG_ERR_UNEXPECTED`: the base attachment API reports another failure, including `object == */
/*   panel`. */
void ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_object* object,
        enum ntg_main_panel_pos pos,
        int* out_status);

/* Updates the main-panel background. A `NULL` options pointer applies defaults; unchanged */
/* options are ignored. */
void ntg_main_panel_set_opts(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts);

#endif // NTG_MAIN_PANEL_H
