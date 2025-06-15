#ifndef _NTG_BOX_DEF_H_
#define _NTG_BOX_DEF_H_

#include "object/ntg_box.h"

#define __NTG_ALLOW_CONTAINER_DEF__
#include "object/def/ntg_container_def.h"
#undef __NTG_ALLOW_CONTAINER_DEF__

#define NTG_BOX_MAX_CHILDREN 50

struct ntg_box
{
    ntg_container_t _base;

    ntg_box_orientation_t _orientation;

    struct ntg_box_padding _padding;
    size_t _spacing;
};

void __ntg_box_init__(ntg_box_t* box, ntg_box_orientation_t orientation);
void __ntg_box_deinit__(ntg_box_t* box);

void _ntg_box_compute(const ntg_box_t* box,
        struct ntg_box_padding* out_padding_size,
        struct ntg_xy* out_content_offset,
        struct ntg_xy* out_content_size);

#endif // _NTG_BOX_DEF_H_
