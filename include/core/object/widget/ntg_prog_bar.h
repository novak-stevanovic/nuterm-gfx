#ifndef NTG_PROG_BAR_H
#define NTG_PROG_BAR_H

#include "core/object/widget/ntg_widget.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_prog_bar_opts
{
    ntg_orient orient;
    struct ntg_vcell complete_style;
    struct ntg_vcell uncomplete_style;
    struct ntg_vcell threshold_style;
};

struct ntg_prog_bar_opts ntg_prog_bar_opts_def();

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_prog_bar* ntg_prog_bar_new(ntg_entity_system* system);

void ntg_prog_bar_init(ntg_prog_bar* prog_bar);

void ntg_prog_bar_set_percentage(ntg_prog_bar* prog_bar, double percentage);

struct ntg_prog_bar
{
    ntg_widget __base;

    double __percentage; // [0, 1]
    struct ntg_prog_bar_opts __opts;
};

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_prog_bar_deinit(ntg_prog_bar* prog_bar);

struct ntg_object_measure _ntg_prog_bar_measure_fn(
        const ntg_widget* _prog_bar,
        void* _ldata,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void _ntg_prog_bar_draw_fn(
        const ntg_widget* _prog_bar,
        void* _ldata,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_PROG_BAR_H
