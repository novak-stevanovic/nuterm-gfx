#ifndef _NTG_BORDER_BOX_H_
#define _NTG_BORDER_BOX_H_

#include "core/object/ntg_object.h"

typedef struct ntg_border_box ntg_border_box;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_border_box_opts
{
    size_t placeholder;
};

struct ntg_border_box_opts ntg_border_box_opts_def();

struct ntg_border_box
{
    ntg_object __base;
    ntg_object *__north, *__east, *__south,
               *__west, *__center;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_border_box* ntg_border_box_new(ntg_entity_system* system);

void _ntg_border_box_init_(ntg_border_box* box);

ntg_object* ntg_border_box_get_north(ntg_border_box* box);
ntg_object* ntg_border_box_get_east(ntg_border_box* box);
ntg_object* ntg_border_box_get_south(ntg_border_box* box);
ntg_object* ntg_border_box_get_west(ntg_border_box* box);
ntg_object* ntg_border_box_get_center(ntg_border_box* box);

void ntg_border_box_set_north(ntg_border_box* box, ntg_object* north);
void ntg_border_box_set_east(ntg_border_box* box, ntg_object* east);
void ntg_border_box_set_south(ntg_border_box* box, ntg_object* south);
void ntg_border_box_set_west(ntg_border_box* box, ntg_object* west);
void ntg_border_box_set_center(ntg_border_box* box, ntg_object* center);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_border_box_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_border_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

void _ntg_border_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena);

void _ntg_border_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena);

#endif // _NTG_BORDER_BOX_H_
