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

    ntg_object_vec _children;
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

struct ntg_box_ldata
{
    struct ntg_object_measure hmeasure, vmeasure;
    void* data;
};

void* ntg_box_layout_init_fn(const ntg_object* object);
void _ntg_box_layout_deinit_fn(const ntg_object* object, void* layout_data);

struct ntg_object_measure _ntg_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* _layout_data,
        sarena* arena);

void _ntg_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* _layout_data,
        sarena* arena);

void __ntg_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* _layout_data,
        sarena* arena);

#endif // _NTG_BOX_H_
