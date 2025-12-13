#ifndef _NTG_STAGE_DRAWING_H_
#define _NTG_STAGE_DRAWING_H_

#include "base/ntg_cell.h"
#include "shared/ntg_xy.h"

typedef struct ntg_stage_drawing
{
    ntg_rcell_vgrid ___data;
} ntg_stage_drawing;

void _ntg_stage_drawing_init_(ntg_stage_drawing* drawing);
void _ntg_stage_drawing_deinit_(ntg_stage_drawing* drawing);

ntg_stage_drawing* ntg_stage_drawing_new();
void ntg_stage_drawing_destroy(ntg_stage_drawing* drawing);

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing);
void ntg_stage_drawing_set_size(ntg_stage_drawing* drawing, struct ntg_xy size);

static inline const struct ntg_rcell* ntg_stage_drawing_at(
        const ntg_stage_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_rcell_vgrid_at(&drawing->___data, pos) :
        NULL;
}

static inline struct ntg_rcell* ntg_stage_drawing_at_(
        ntg_stage_drawing* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_rcell_vgrid_at_(&drawing->___data, pos) :
        NULL;
}

#endif // _NTG_STAGE_DRAWING_H_
