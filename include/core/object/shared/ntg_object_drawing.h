#ifndef NTG_OBJECT_DRAWING_H
#define NTG_OBJECT_DRAWING_H

#include "shared/ntg_typedef.h"
#include "base/ntg_cell.h"

struct ntg_object_drawing
{
    ntg_vcell_vecgrid __data;
};

void ntg_object_drawing_init(ntg_object_drawing* drawing);
void ntg_object_drawing_deinit(ntg_object_drawing* drawing);

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

struct ntg_tmp_object_drawing
{
    struct ntg_vcell* __data;
    struct ntg_xy _size;
};

void ntg_tmp_object_drawing_init(ntg_tmp_object_drawing* drawing,
        struct ntg_xy size, sarena* arena);

static inline const struct ntg_vcell*
ntg_tmp_object_drawing_at(const ntg_tmp_object_drawing* drawing, struct ntg_xy pos)
{
    return (((drawing != NULL) && ntg_xy_is_greater(drawing->_size, pos))) ?
        &(drawing->__data[drawing->_size.x * pos.y + pos.x]) :
        NULL;
}

static inline struct ntg_vcell*
ntg_tmp_object_drawing_at_(ntg_tmp_object_drawing* drawing, struct ntg_xy pos)
{
    return (((drawing != NULL) && ntg_xy_is_greater(drawing->_size, pos))) ?
        &(drawing->__data[drawing->_size.x * pos.y + pos.x]) :
        NULL;
}

#endif //  NTG_OBJECT_DRAWING_H
