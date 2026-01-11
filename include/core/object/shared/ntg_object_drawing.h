#ifndef _NTG_OBJECT_DRAWING_H_
#define _NTG_OBJECT_DRAWING_H_

#include "shared/ntg_typedef.h"
#include "base/ntg_cell.h"

struct ntg_object_drawing
{
    ntg_vcell_vecgrid __data;
};

void ntg_object_drawing_init(ntg_object_drawing* drawing);
void _ntg_object_drawing_deinit_(ntg_object_drawing* drawing);

ntg_object_drawing* ntg_object_drawing_new();
void ntg_object_drawing_destroy(ntg_object_drawing* drawing);

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing);
void ntg_object_drawing_set_size(ntg_object_drawing* drawing,
    struct ntg_xy size, struct ntg_xy size_cap);

void ntg_object_drawing_place(const ntg_object_drawing* src_drawing,
        struct ntg_xy src_start_pos, struct ntg_xy src_box_size,
        ntg_object_drawing* dest_drawing, struct ntg_xy dest_start_pos);

void ntg_object_drawing_place_(const ntg_object_drawing* src_drawing,
        struct ntg_xy src_start_pos, struct ntg_xy src_box_size,
        ntg_stage_drawing* dest_drawing, struct ntg_xy dest_start_pos);

static inline const struct ntg_vcell*
ntg_object_drawing_at(const ntg_object_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_vcell_vecgrid_at(&drawing->__data, pos) :
        NULL;
}

static inline struct ntg_vcell*
ntg_object_drawing_at_(ntg_object_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_vcell_vecgrid_at_(&drawing->__data, pos) :
        NULL;
}

#endif //  _NTG_OBJECT_DRAWING_H_
