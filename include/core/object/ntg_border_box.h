#ifndef NTG_BORDER_BOX_H
#define NTG_BORDER_BOX_H

#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_border_box_opts
{
    size_t placeholder;
};

struct ntg_border_box_opts ntg_border_box_opts_def();

enum ntg_border_box_pos
{
    NTG_BORDER_BOX_NORTH = 0,
    NTG_BORDER_BOX_EAST,
    NTG_BORDER_BOX_SOUTH,
    NTG_BORDER_BOX_WEST,
    NTG_BORDER_BOX_CENTER
};

struct ntg_border_box
{
    ntg_object __base;

    ntg_object* _children[5];
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_border_box* ntg_border_box_new(ntg_entity_system* system);

void ntg_border_box_init(ntg_border_box* box);

void ntg_border_box_set(ntg_border_box* box,
        ntg_object* object,
        enum ntg_border_box_pos position);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_border_box_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_border_box_measure_fn(
        const ntg_object* _border_box,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

void _ntg_border_box_constrain_fn(
        const ntg_object* _border_box,
        void* _layout_data,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena);

void _ntg_border_box_arrange_fn(
        const ntg_object* _border_box,
        void* _layout_data,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena);

#endif // NTG_BORDER_BOX_H
