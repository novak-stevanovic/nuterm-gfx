#ifndef _NTG_SCENE_DRAWING_H_
#define _NTG_SCENE_DRAWING_H_

#include "base/ntg_cell.h"
#include "shared/ntg_xy.h"

typedef struct ntg_scene_drawing
{
    ntg_rcell_vgrid_t __data;
} ntg_scene_drawing_t;

void __ntg_scene_drawing_init__(ntg_scene_drawing_t* drawing);
void __ntg_scene_drawing_deinit__(ntg_scene_drawing_t* drawing);

ntg_scene_drawing_t* ntg_scene_drawing_new();
void ntg_scene_drawing_destroy(ntg_scene_drawing_t* drawing);

struct ntg_xy ntg_scene_drawing_get_size(const ntg_scene_drawing_t* drawing);
void ntg_scene_drawing_set_size(ntg_scene_drawing_t* drawing, struct ntg_xy size);

static inline const struct ntg_rcell* ntg_scene_drawing_at(
        const ntg_scene_drawing_t* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_rcell_vgrid_at(&drawing->__data, pos) :
        NULL;
}

static inline struct ntg_rcell* ntg_scene_drawing_at_(
        ntg_scene_drawing_t* drawing, struct ntg_xy pos)
{
    return (drawing != NULL) ?
        ntg_rcell_vgrid_at_(&drawing->__data, pos) :
        NULL;
}

#endif // _NTG_SCENE_DRAWING_H_
