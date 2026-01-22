#ifndef NTG_BOX_H
#define NTG_BOX_H

#include "core/object/widget/ntg_widget.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_box_opts
{
    ntg_orient orient;
    ntg_align palign;
    ntg_align salign;
    size_t spacing;
};

struct ntg_box_opts ntg_box_opts_def();

struct ntg_box
{
    ntg_widget __base;

    struct ntg_box_opts _opts;
    ntg_widget_vec _children;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_box* ntg_box_new(ntg_entity_system* system);
void ntg_box_init(ntg_box* box);

struct ntg_box_opts ntg_box_get_opts(const ntg_box* box);
void ntg_box_set_opts(ntg_box* box, struct ntg_box_opts opts);

void ntg_box_add_child(ntg_box* box, ntg_widget* child);
void ntg_box_rm_child(ntg_box* box, ntg_widget* child);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_box_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_box_measure_fn(
        const ntg_widget* _box,
        void* _layout_data,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void _ntg_box_constrain_fn(
        const ntg_widget* _box,
        void* _layout_data,
        ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena);

void _ntg_box_arrange_fn(
        const ntg_widget* _box,
        void* _layout_data,
        ntg_widget_xy_map* out_pos_map,
        sarena* arena);

#endif // NTG_BOX_H
