#ifndef NTG_STAGE_DRAWING_H
#define NTG_STAGE_DRAWING_H

#include "shared/ntg_shared.h"
#include "base/ntg_cell.h"
#include "base/ntg_xy.h"
#include <assert.h>

struct ntg_stage_drawing
{
    ntg_cell_vecgrid __data;
};

void ntg_stage_drawing_init(ntg_stage_drawing* drawing);
void ntg_stage_drawing_deinit(ntg_stage_drawing* drawing);

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing);

void ntg_stage_drawing_set_size(
        ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap);

static inline struct ntg_cell
ntg_stage_drawing_get(const ntg_stage_drawing* drawing, struct ntg_xy pos)
{
    assert(drawing);

    return ntg_cell_vecgrid_get(&drawing->__data, pos);
}

static inline void
ntg_stage_drawing_set(ntg_stage_drawing* drawing, struct ntg_cell cell, struct ntg_xy pos)
{
    assert(drawing);

    ntg_cell_vecgrid_set(&drawing->__data, cell, pos);
}

#endif // NTG_STAGE_DRAWING_H
