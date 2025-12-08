#ifndef _NTG_BORDER_BOX_H_
#define _NTG_BORDER_BOX_H_

#include "core/object/ntg_object.h"

typedef struct ntg_border_box ntg_border_box;

#define NTG_BORDER_BOX(box_ptr) ((ntg_border_box*)(box_ptr))

struct ntg_border_box
{
    ntg_object __base;
    ntg_object *__north, *__east, *__south,
               *__west, *__center;
};

void __ntg_border_box_init__(
        ntg_border_box* box,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        ntg_on_unfocus_fn on_unfocus_fn,
        void* data);
void __ntg_border_box_deinit__(ntg_border_box* box);

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

struct ntg_measure_out __ntg_border_box_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena);

void __ntg_border_box_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out_sizes,
        sarena* arena);

void __ntg_border_box_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out_positions,
        sarena* arena);

#endif // _NTG_BORDER_BOX_H_
