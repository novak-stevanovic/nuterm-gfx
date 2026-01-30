#ifndef NTG_OBJECT_DRAWING_H
#define NTG_OBJECT_DRAWING_H

#include "shared/ntg_shared.h"

struct ntg_object_drawing
{
    ntg_vcell_vecgrid __data;
};

void ntg_object_drawing_init(ntg_object_drawing* drawing);
void ntg_object_drawing_deinit(ntg_object_drawing* drawing);

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing);

void ntg_object_drawing_set_size(
        ntg_object_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap);

void ntg_object_drawing_place(
        const ntg_object_drawing* src_drawing,
        struct ntg_xy src_start_pos,
        struct ntg_xy src_box_size,
        ntg_object_drawing* dest_drawing,
        struct ntg_xy dest_start_pos);

void ntg_object_drawing_place_(
        const ntg_object_drawing* src_drawing,
        struct ntg_xy src_start_pos,
        struct ntg_xy src_box_size,
        ntg_stage_drawing* dest_drawing,
        struct ntg_xy dest_start_pos);

static inline struct ntg_vcell
ntg_object_drawing_get(const ntg_object_drawing* drawing, struct ntg_xy pos)
{
    assert(drawing);

    return ntg_vcell_vecgrid_get(&drawing->__data, pos);
}

static inline void
ntg_object_drawing_set(ntg_object_drawing* drawing, struct ntg_vcell cell, struct ntg_xy pos)
{
    assert(drawing);

    ntg_vcell_vecgrid_set(&drawing->__data, cell, pos);
}

#endif //  NTG_OBJECT_DRAWING_H
