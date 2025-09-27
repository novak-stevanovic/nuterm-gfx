#ifndef _NTG_PROG_BAR_H_
#define _NTG_PROG_BAR_H_

#include "object/ntg_object.h"

#define NTG_PROG_BAR(prog_bar_ptr) ((ntg_prog_bar*)(prog_bar_ptr))

typedef struct ntg_prog_bar ntg_prog_bar;

void __ntg_prog_bar_init__(ntg_prog_bar* prog_bar,
        ntg_orientation orientation, ntg_cell complete_cell,
        ntg_cell uncomplete_cell, ntg_cell threshold_cell,
        ntg_object_process_key_fn process_key_fn);
void __ntg_prog_bar_deinit__(ntg_prog_bar* prog_bar);

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

struct ntg_measure_result _ntg_prog_bar_measure_fn(const ntg_object* _prog_bar,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

void _ntg_prog_bar_arrange_drawing_fn(const ntg_object* _prog_bar,
        struct ntg_xy size, ntg_object_drawing* out_drawing);

#endif // _NTG_PROG_BAR_H_
