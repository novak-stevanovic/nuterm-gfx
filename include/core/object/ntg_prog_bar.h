#ifndef NTG_PROG_BAR_H
#define NTG_PROG_BAR_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_prog_bar_style
{
    struct ntg_vcell complete_style;
    struct ntg_vcell uncomplete_style;
    struct ntg_vcell threshold_style;
};

/* Creates the default progress-bar style: green complete and threshold cells,
 * and a red incomplete cell.
 *
 * RETURN VALUE:
 * The default `ntg_prog_bar_style` value. */
NTG_API struct ntg_prog_bar_style
ntg_prog_bar_style_def();

/* ------------------------------------------------------ */

/* Compares two progress-bar style values. Pointer identity counts as equal;
 * otherwise a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_prog_bar_style_are_eq(
        const struct ntg_prog_bar_style* style1,
        const struct ntg_prog_bar_style* style2);

struct ntg_prog_bar_opts
{
    struct ntg_prog_bar_style style;
    ntg_orient orient;
};

/* Creates horizontal progress-bar options using the default style.
 *
 * RETURN VALUE:
 * The default `ntg_prog_bar_opts` value. */
NTG_API struct ntg_prog_bar_opts
ntg_prog_bar_opts_def();

/* ------------------------------------------------------ */

/* Compares two progress-bar option values. Pointer identity counts as equal;
 * otherwise a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_prog_bar_opts_are_eq(
        const struct ntg_prog_bar_opts* opts1,
        const struct ntg_prog_bar_opts* opts2);

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

struct ntg_prog_bar
{
    ntg_object __base;

    double _prog; // [0, 1]
    struct ntg_prog_bar_opts _opts;
    struct ntg_prog_bar_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes the base progress-bar object and applies options, using defaults
 * for `NULL`. The current implementation does not assign `_prog`; callers must
 * provide zero-initialized storage to obtain zero initial progress.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `prog_bar` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_prog_bar_init(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts,
        int* out_status);

/* ------------------------------------------------------ */

/* Releases resources owned by a progress bar. Passing `NULL` has no effect. */
NTG_API void
ntg_prog_bar_deinit(ntg_prog_bar* prog_bar);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_prog_bar_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_prog_bar_deinit_(void* _prog_bar);

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Updates progress-bar orientation and cell styles. A `NULL` options pointer
 * applies defaults; unchanged options are ignored. */
NTG_API void
ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts);

/* ------------------------------------------------------ */
/* PROGRESS */
/* ------------------------------------------------------ */

/* Processes a requested progress value. Requests above `1.0` are clamped for
 * the hook, the object is marked for redraw, and the hook is invoked when the
 * request differs from the stored value. The current implementation does not
 * assign the value to `_prog` and does not clamp negative values. */
NTG_API void
ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double percentage);

#endif // NTG_PROG_BAR_H
