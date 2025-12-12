#ifndef _NTG_OBJECT_DRAWING_H_
#define _NTG_OBJECT_DRAWING_H_

#include "base/ntg_cell.h"
#include "shared/ntg_xy.h"

typedef struct ntg_stage_drawing ntg_stage_drawing;

typedef struct ntg_object_drawing
{
    ntg_cell_vgrid ___data;
} ntg_object_drawing;

void __ntg_object_drawing_init__(ntg_object_drawing* object_drawing);
void __ntg_object_drawing_deinit__(ntg_object_drawing* object_drawing);

ntg_object_drawing* ntg_object_drawing_new();
void ntg_object_drawing_destroy(ntg_object_drawing* object_drawing);

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* object_drawing);
void ntg_object_drawing_set_size(ntg_object_drawing* object_drawing, struct ntg_xy size);

void ntg_object_drawing_place(const ntg_object_drawing* src_object_drawing,
        struct ntg_xy src_start_pos, struct ntg_xy src_box_size,
        ntg_object_drawing* dest_object_drawing, struct ntg_xy dest_start_pos);

void ntg_object_drawing_place_(const ntg_object_drawing* src_object_drawing,
        struct ntg_xy src_start_pos, struct ntg_xy src_box_size,
        ntg_stage_drawing* dest_object_drawing, struct ntg_xy dest_start_pos);

static inline const struct ntg_cell* ntg_object_drawing_at(
        const ntg_object_drawing* object_drawing, struct ntg_xy pos)
{
    return (object_drawing != NULL) ?
        ntg_cell_vgrid_at(&object_drawing->___data, pos) :
        NULL;
}

static inline struct ntg_cell* ntg_object_drawing_at_(
        ntg_object_drawing* object_drawing, struct ntg_xy pos)
{
    return (object_drawing != NULL) ?
        ntg_cell_vgrid_at_(&object_drawing->___data, pos) :
        NULL;
}

#endif //  _NTG_OBJECT_DRAWING_H_
