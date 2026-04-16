#ifndef NTG_COLOR_BLOCK_H
#define NTG_COLOR_BLOCK_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

// TODO
struct ntg_color_block_hooks
{
    void (*on_color_chng_fn)(
            ntg_color_block* cb,
            struct nt_color old_color,
            struct nt_color new_color);
};

struct ntg_color_block
{
    ntg_object __base;
    struct nt_color __color;

    struct ntg_color_block_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_color_block_init(ntg_color_block* color_block, struct nt_color color);
void ntg_color_block_deinit(ntg_color_block* block);
void ntg_color_block_deinit_(void* _block);

void ntg_color_block_set_color(ntg_color_block* color_block, struct nt_color color);

#endif // NTG_COLOR_BLOCK_H
