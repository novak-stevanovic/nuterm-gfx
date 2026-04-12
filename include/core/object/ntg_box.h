#ifndef NTG_BOX_H
#define NTG_BOX_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_box_opts
{
    ntg_orient orient;
    ntg_align prim_align;
    ntg_align sec_align;
    size_t spacing;
};

struct ntg_box_opts ntg_box_opts_def();

struct ntg_box
{
    ntg_object __base;

    struct ntg_box_opts _opts;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_box_init(ntg_box* box);
void ntg_box_deinit(ntg_box* box);
void ntg_box_deinit_(void* _box);

void ntg_box_set_opts(ntg_box* box, const struct ntg_box_opts* opts);

const struct ntg_object_vec* ntg_box_get_children(const ntg_box* box);
void ntg_box_add_child(ntg_box* box, ntg_object* child);
void ntg_box_rm_child(ntg_box* box, ntg_object* child);

#endif // NTG_BOX_H
