#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "core/object/ntg_object.h"

typedef struct ntg_box ntg_box;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void _ntg_box_init_(
        ntg_box* box,
        ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment,
        struct ntg_object_event_ops event_ops,
        ntg_object_deinit_fn deinit_fn,
        void* data,
        ntg_object_container* container);

ntg_orientation ntg_box_get_orientation(const ntg_box* box);
ntg_alignment ntg_box_get_primary_alignment(const ntg_box* box);
ntg_alignment ntg_box_get_secondary_alignment(const ntg_box* box);
size_t ntg_box_get_spacing(const ntg_box* box);

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_spacing(ntg_box* box, size_t spacing);

void ntg_box_add_child(ntg_box* box, ntg_object* child);
void ntg_box_rm_child(ntg_box* box, ntg_object* child);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_box
{
    ntg_object __base;

    ntg_orientation __orientation;
    ntg_alignment __primary_alignment,
                  __secondary_alignment;

    size_t __spacing;
};

void _ntg_box_deinit_fn(ntg_object* _box);

struct ntg_object_measure _ntg_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

void _ntg_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena);

void __ntg_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena);

#endif // _NTG_BOX_H_
