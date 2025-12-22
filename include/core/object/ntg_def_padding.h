#ifndef _NTG_DEF_PADDING_H_
#define _NTG_DEF_PADDING_H_

#include "core/object/ntg_padding.h"

typedef struct ntg_def_padding ntg_def_padding;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

struct ntg_def_padding
{
    ntg_padding __base;

    ntg_cell __cell;
};

void _ntg_def_padding_init_(
        ntg_def_padding* def_padding,
        ntg_cell cell,
        struct ntg_padding_width init_width,
        ntg_entity_group* group,
        ntg_entity_system* system);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_padding_deinit_fn(ntg_entity* entity);

void _ntg_def_padding_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* _layout_data,
        sarena* arena);

#endif // _NTG_DEF_PADDING_H_
