#ifndef NTG_DEF_PADDING_H
#define NTG_DEF_PADDING_H

#include "core/object/decorator/ntg_decorator.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_padding
{
    ntg_decorator __base;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_padding* ntg_def_padding_new(ntg_entity_system* system);
void ntg_def_padding_init(ntg_def_padding* def_padding);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_padding_deinit_fn(ntg_entity* entity);

void _ntg_def_padding_draw_fn(
        const ntg_object* _padding,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_DEF_PADDING_H
