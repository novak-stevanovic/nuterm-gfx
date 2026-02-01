#ifndef NTG_BBOX_H
#define NTG_BBOX_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_bbox_opts
{
    size_t placeholder;
};

struct ntg_bbox_opts ntg_bbox_opts_def();

enum ntg_bbox_pos
{
    NTG_BBOX_NORTH = 0,
    NTG_BBOX_EAST,
    NTG_BBOX_SOUTH,
    NTG_BBOX_WEST,
    NTG_BBOX_CENTER
};

struct ntg_bbox
{
    ntg_object __base;

    ntg_object* _children[5];
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_bbox_init(ntg_bbox* box);
void ntg_bbox_deinit(ntg_bbox* box);

void ntg_bbox_set(
        ntg_bbox* box,
        ntg_object* object,
        enum ntg_bbox_pos pos);

#endif // NTG_BBOX_H
