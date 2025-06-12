#ifndef _NTG_OBJECT_DRAWING_H_
#define _NTG_OBJECT_DRAWING_H_

#include "base/ntg_cell.h"
#include "shared/ntg_xy.h"

typedef struct ntg_object_drawing ntg_object_drawing_t;

typedef struct ntg_object_drawing
{
    ntg_cell_grid_t* __data;
    struct ntg_xy __size;
} ntg_object_drawing_t;

void __ntg_object_drawing_init__(ntg_object_drawing_t* drawing);
void __ntg_object_drawing_deinit__(ntg_object_drawing_t* drawing);

ntg_object_drawing_t* ntg_object_drawing_new();
void ntg_object_drawing_destroy(ntg_object_drawing_t* drawing);

const ntg_cell_t* ntg_object_drawing_at(const ntg_object_drawing_t* drawing,
        struct ntg_xy pos);
ntg_cell_t* ntg_object_drawing_at_(ntg_object_drawing_t* drawing,
        struct ntg_xy pos);

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing_t* drawing);
void ntg_object_drawing_set_size(ntg_object_drawing_t* drawing, struct ntg_xy size);

#endif //  _NTG_OBJECT_DRAWING_H_
