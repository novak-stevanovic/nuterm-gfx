#include <stdlib.h>
#include <assert.h>

#include "object/shared/ntg_object_drawing.h"
#include "shared/ntg_log.h"

void __ntg_object_drawing_init__(ntg_object_drawing* drawing)
{
    assert(drawing != NULL);

    __ntg_cell_vgrid_init__(&drawing->__data);

    drawing->__vp_size = NTG_XY_UNSET;
    drawing->__vp_offset = NTG_XY_UNSET;
}

void __ntg_object_drawing_deinit__(ntg_object_drawing* drawing)
{
    assert(drawing != NULL);

    drawing->__vp_size = NTG_XY_UNSET;
    drawing->__vp_offset = NTG_XY_UNSET;
}

ntg_object_drawing* ntg_object_drawing_new()
{
    ntg_object_drawing* new = (ntg_object_drawing*)malloc(
            sizeof(struct ntg_object_drawing));

    if(new == NULL) return NULL;

    __ntg_object_drawing_init__(new);

    return new;
}

void ntg_object_drawing_destroy(ntg_object_drawing* drawing)
{
    assert(drawing != NULL);

    __ntg_object_drawing_deinit__(drawing);

    free(drawing);
}

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing)
{
    assert(drawing != NULL);

    return ntg_cell_vgrid_get_size(&drawing->__data);
}

void ntg_object_drawing_set_size(ntg_object_drawing* drawing,
        struct ntg_xy size)
{
    assert(drawing != NULL);

    ntg_xy_size(&size);

    ntg_cell_vgrid_set_size(&drawing->__data, size, NULL);

    struct ntg_xy total_size = ntg_cell_vgrid_get_size(&drawing->__data);

    struct ntg_xy _vp_size = ntg_xy(
            (drawing->__vp_size.x <= total_size.x) ?
            drawing->__vp_size.x :
            total_size.x,

            (drawing->__vp_size.y <= total_size.y) ?
            drawing->__vp_size.y :
            total_size.y);

    ntg_object_drawing_set_vp_size(drawing, _vp_size);
}

struct ntg_xy ntg_object_drawing_get_vp_offset(const ntg_object_drawing* drawing)
{
    assert(drawing != NULL);

    return drawing->__vp_offset;
}

void ntg_object_drawing_set_vp_offset(ntg_object_drawing* drawing,
        struct ntg_xy vp_offset)
{
    assert(drawing != NULL);

    struct ntg_xy total_size = ntg_cell_vgrid_get_size(&drawing->__data);

    struct ntg_xy max_offset = ntg_xy_sub(total_size, drawing->__vp_size);

    vp_offset.x = (vp_offset.x <= max_offset.x) ? vp_offset.x : max_offset.x;
    vp_offset.y = (vp_offset.y <= max_offset.y) ? vp_offset.y : max_offset.y;

    drawing->__vp_offset = vp_offset;
}

struct ntg_xy ntg_object_drawing_get_vp_size(const ntg_object_drawing* drawing)
{
    assert(drawing != NULL);

    return drawing->__vp_size;
}

void ntg_object_drawing_set_vp_size(ntg_object_drawing* drawing,
        struct ntg_xy size)
{
    assert(drawing != NULL);

    struct ntg_xy total_size = ntg_cell_vgrid_get_size(&drawing->__data);

    ntg_xy_size(&size);

    if((size.x > total_size.x) || (size.y > total_size.y))
        assert(0);

    drawing->__vp_size = size;

    struct ntg_dxy extra = ntg_dxy_sub(
            ntg_dxy_from_xy(ntg_xy_add(drawing->__vp_offset, size)),
            ntg_dxy_from_xy(total_size));

    extra.x = (extra.x >= 0) ? extra.x : 0;
    extra.y = (extra.y >= 0) ? extra.y : 0;

    drawing->__vp_offset = ntg_xy_sub(drawing->__vp_offset,
                ntg_xy_from_dxy(extra));
}
