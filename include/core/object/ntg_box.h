#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "core/object/ntg_object.h"

typedef struct ntg_box ntg_box;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void __ntg_box_init__(
        ntg_box* box,
        ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        ntg_on_unfocus_fn on_unfocus_fn,
        void* data);
void __ntg_box_deinit__(ntg_box* box);

ntg_orientation ntg_box_get_orientation(const ntg_box* box);
ntg_alignment ntg_box_get_primary_alignment(const ntg_box* box);
ntg_alignment ntg_box_get_secondary_alignment(const ntg_box* box);
size_t ntg_box_get_spacing(const ntg_box* box);
void ntg_box_set_background(ntg_box* box, ntg_cell background);

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_spacing(ntg_box* box, size_t spacing);
ntg_cell ntg_box_get_background(const ntg_box* box);

void ntg_box_add_child(ntg_box* box, ntg_object* child);
void ntg_box_rm_child(ntg_box* box, ntg_object* child);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_box
{
    ntg_object __base;

    ntg_orientation __orientation;
    ntg_alignment __primary_alignment,
                  __secondary_alignment;

    size_t __spacing;
};

struct ntg_measure_out __ntg_box_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena);

void __ntg_box_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out_sizes,
        sarena* arena);

void __ntg_box_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out_positions,
        sarena* arena);

#endif // _NTG_BOX_H_
