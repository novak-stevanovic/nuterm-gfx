#ifndef _NTG_OBJECT_DRAWING_H_
#define _NTG_OBJECT_DRAWING_H_

#include "base/ntg_cell.h"
#include "shared/ntg_xy.h"

typedef struct ntg_object_drawing
{
    ntg_cell_vgrid __data; // __data->__size = current object drawing size
    struct ntg_xy __vp_offset;
    struct ntg_xy __vp_size;
} ntg_object_drawing;

void __ntg_object_drawing_init__(ntg_object_drawing* drawing);
void __ntg_object_drawing_deinit__(ntg_object_drawing* drawing);

ntg_object_drawing* ntg_object_drawing_new();
void ntg_object_drawing_destroy(ntg_object_drawing* drawing);

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing);
void ntg_object_drawing_set_size(ntg_object_drawing* drawing,
        struct ntg_xy size);

struct ntg_xy ntg_object_drawing_get_vp_offset(const ntg_object_drawing* drawing);
void ntg_object_drawing_set_vp_offset(ntg_object_drawing* drawing,
        struct ntg_xy vp_offset);

struct ntg_xy ntg_object_drawing_get_vp_size(const ntg_object_drawing* drawing);
void ntg_object_drawing_set_vp_size(ntg_object_drawing* drawing,
        struct ntg_xy size);

static inline const ntg_cell* ntg_object_drawing_at(
        const ntg_object_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_cell_vgrid_at(&drawing->__data, pos) :
        NULL;
}

static inline ntg_cell* ntg_object_drawing_at_(
        ntg_object_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_cell_vgrid_at_(&drawing->__data, pos) :
        NULL;
}

static inline const ntg_cell* ntg_object_drawing_vp_at(
        const ntg_object_drawing* drawing, struct ntg_xy pos)
{
    struct ntg_xy vp_pos = ntg_xy_add(pos, drawing->__vp_offset);

    return (drawing != NULL) ?
        ntg_cell_vgrid_at(&drawing->__data, vp_pos) :
        NULL;
}

static inline ntg_cell* ntg_object_drawing_vp_at_(
        ntg_object_drawing* drawing, struct ntg_xy pos)
{
    struct ntg_xy vp_pos = ntg_xy_add(pos, drawing->__vp_offset);

    return (drawing != NULL) ?
        ntg_cell_vgrid_at_(&drawing->__data, vp_pos) :
        NULL;
}

#endif //  _NTG_OBJECT_DRAWING_H_
