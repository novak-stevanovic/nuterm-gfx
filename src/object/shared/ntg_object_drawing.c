#include <stdlib.h>
#include <assert.h>

#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
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

void ntg_object_drawing_scroll(ntg_object_drawing* drawing,
        struct ntg_dxy vp_offset_diff)
{
    struct ntg_dxy total = ntg_dxy_add(
            vp_offset_diff,
            ntg_dxy_from_xy(drawing->__vp_offset));

    struct ntg_xy total_xy = ntg_xy_from_dxy(
            ntg_dxy_clamp(ntg_dxy(0, 0), total, NTG_DXY_MAX));

    ntg_object_drawing_set_vp_offset(drawing, total_xy);
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

    ntg_xy_size_(&size);

    ntg_cell_vgrid_set_size(&drawing->__data, size, NULL);

    // struct ntg_xy total_size = ntg_cell_vgrid_get_size(&drawing->__data);

    struct ntg_xy _vp_size = ntg_xy(
            _min2_size(drawing->__vp_size.x, size.x),
            _min2_size(drawing->__vp_size.y, size.y));

    if(!ntg_xy_are_equal(_vp_size, drawing->__vp_size))
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

    vp_offset.x = _min2_size(vp_offset.x, max_offset.x);
    vp_offset.y = _min2_size(vp_offset.y, max_offset.y);

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

    ntg_xy_size_(&size);

    /* vp_size must not be greater than size */

    if((size.x > total_size.x) || (size.y > total_size.y))
        assert(0);

    drawing->__vp_size = size;

    /* If offset + vp_size > size, make the offset smaller */

    struct ntg_dxy extra = ntg_dxy_sub(
            ntg_dxy_from_xy(ntg_xy_add(drawing->__vp_offset, size)),
            ntg_dxy_from_xy(total_size));

    extra.x = (extra.x >= 0) ? extra.x : 0;
    extra.y = (extra.y >= 0) ? extra.y : 0;

    drawing->__vp_offset = ntg_xy_sub(drawing->__vp_offset,
                ntg_xy_from_dxy(extra));
}
