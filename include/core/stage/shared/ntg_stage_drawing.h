#ifndef NTG_STAGE_DRAWING_H
#define NTG_STAGE_DRAWING_H

#include "shared/ntg_typedef.h"
#include "base/ntg_cell.h"

struct ntg_stage_drawing
{
    ntg_cell_vecgrid __data;
};

void ntg_stage_drawing_init(ntg_stage_drawing* drawing);
void ntg_stage_drawing_deinit(ntg_stage_drawing* drawing);

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing);

void ntg_stage_drawing_set_size(ntg_stage_drawing* drawing,
    struct ntg_xy size, struct ntg_xy size_cap);

static inline const struct ntg_cell* 
ntg_stage_drawing_at(const ntg_stage_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ? ntg_cell_vecgrid_at(&drawing->__data, pos) : NULL;
}

static inline struct ntg_cell* 
ntg_stage_drawing_at_(ntg_stage_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ? ntg_cell_vecgrid_at_(&drawing->__data, pos) : NULL;
}

#endif // NTG_STAGE_DRAWING_H
