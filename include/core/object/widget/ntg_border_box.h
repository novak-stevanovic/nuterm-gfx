#ifndef NTG_BORDER_BOX_H
#define NTG_BORDER_BOX_H

#include "core/object/widget/ntg_widget.h"

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
    ntg_widget __base;

    ntg_widget* _children[5];
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_border_box* ntg_border_box_new(ntg_entity_system* system);

void ntg_border_box_init(ntg_border_box* box);

void ntg_border_box_set(ntg_border_box* box,
        ntg_widget* widget,
        enum ntg_border_box_pos pos);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_border_box_deinit(ntg_border_box* box);

struct ntg_object_measure _ntg_border_box_measure_fn(
        const ntg_widget* _border_box,
        void* _ldata,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void _ntg_border_box_constrain_fn(
        const ntg_widget* _border_box,
        void* _ldata,
        ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena);

void _ntg_border_box_arrange_fn(
        const ntg_widget* _border_box,
        void* _ldata,
        ntg_widget_xy_map* out_pos_map,
        sarena* arena);

#endif // NTG_BORDER_BOX_H
