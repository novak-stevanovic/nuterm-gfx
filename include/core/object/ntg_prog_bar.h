#ifndef _NTG_PROG_BAR_H_
#define _NTG_PROG_BAR_H_

#include "core/object/ntg_object.h"

typedef struct ntg_prog_bar ntg_prog_bar;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void _ntg_prog_bar_init_(
        ntg_prog_bar* prog_bar,
        ntg_orientation orientation,
        ntg_cell complete_cell,
        ntg_cell uncomplete_cell,
        ntg_cell threshold_cell,
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data);
void _ntg_prog_bar_deinit_(ntg_prog_bar* prog_bar);

void ntg_prog_bar_set_percentage(ntg_prog_bar* prog_bar, double percentage);
double ntg_prog_bar_get_percentage(const ntg_prog_bar* prog_bar);

struct ntg_prog_bar
{
    ntg_object __base;

    ntg_orientation __orientation;
    double __percentage; // [0, 1]

    ntg_cell __complete_cell, __uncomplete_cell,
             __threshold_cell;
};

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_prog_bar_deinit_fn(ntg_object* object);

struct ntg_object_measure _ntg_prog_bar_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena);

void _ntg_prog_bar_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena);

#endif // _NTG_PROG_BAR_H_
