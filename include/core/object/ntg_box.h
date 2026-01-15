#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_measure.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_box_opts
{
    ntg_orientation orientation;
    ntg_alignment palignment;
    ntg_alignment salignment;
    size_t spacing;
};

struct ntg_box_opts ntg_box_opts_def();

struct ntg_box
{
    ntg_object __base;

    struct ntg_box_opts _opts;

    ntg_object_vec __children;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_box* ntg_box_new(ntg_entity_system* system);
void ntg_box_init(ntg_box* box);

struct ntg_box_opts ntg_box_get_opts(const ntg_box* box);
void ntg_box_set_opts(ntg_box* box, struct ntg_box_opts opts);

void ntg_box_add_child(ntg_box* box, ntg_object* child);
void ntg_box_rm_child(ntg_box* box, ntg_object* child);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_box_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_box_measure_fn(
        const ntg_object* _box,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

void _ntg_box_constrain_fn(
        const ntg_object* _box,
        void* _layout_data,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena);

void _ntg_box_arrange_fn(
        const ntg_object* _box,
        void* _layout_data,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena);

#endif // _NTG_BOX_H_
