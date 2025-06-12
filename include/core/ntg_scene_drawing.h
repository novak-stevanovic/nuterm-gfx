#ifndef _NTG_SCENE_DRAWING_H_
#define _NTG_SCENE_DRAWING_H_

#include "base/ntg_cell.h"
#include "shared/ntg_xy.h"

typedef struct ntg_scene_drawing
{
    ntg_rcell_grid_t* __data;
    struct ntg_xy __size;
} ntg_scene_drawing_t;

void __ntg_scene_drawing_init__(ntg_scene_drawing_t* drawing);
void __ntg_scene_drawing_deinit__(ntg_scene_drawing_t* drawing);

ntg_scene_drawing_t* ntg_scene_drawing_new();
void ntg_scene_drawing_destroy(ntg_scene_drawing_t* drawing);

const struct ntg_rcell* ntg_scene_drawing_at(const ntg_scene_drawing_t* drawing,
        struct ntg_xy pos);
struct ntg_rcell* ntg_scene_drawing_at_(ntg_scene_drawing_t* drawing,
        struct ntg_xy pos);

struct ntg_xy ntg_scene_drawing_get_size(const ntg_scene_drawing_t* drawing);
void ntg_scene_drawing_set_size(ntg_scene_drawing_t* drawing, struct ntg_xy size);

#endif // _NTG_SCENE_DRAWING_H_
