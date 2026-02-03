#ifndef NTG_BOX_H
#define NTG_BOX_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_box_opts
{
    ntg_orient orient;
    ntg_align palign;
    ntg_align salign;
    size_t spacing;
};

struct ntg_box_opts ntg_box_opts_def();

struct ntg_box
{
    ntg_object __base;

    struct ntg_box_opts _opts;
    ntg_object_vec _children;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_box_init(ntg_box* box);
void ntg_box_deinit(ntg_box* box);
void ntg_box_deinit_(void* _box);

struct ntg_box_opts ntg_box_get_opts(const ntg_box* box);
void ntg_box_set_opts(ntg_box* box, struct ntg_box_opts opts);

void ntg_box_add_child(ntg_box* box, ntg_object* child);
void ntg_box_rm_child(ntg_box* box, ntg_object* child);

#endif // NTG_BOX_H
