#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"
#define DEFAULT_SIZE 1

static struct ntg_object_measure measure_fn(
        const ntg_object* _block,
        ntg_orient,
        void* lctx,
        sarena* arena);

static void draw_fn(
        const ntg_object* _block,
        ntg_object_tmp_drawing* out_drawing,
        void* lctx,
        sarena* arena);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    color_block->__color = color;

    ntg_object_add_dirty((ntg_object*)color_block, NTG_OBJECT_DIRTY_DRAW);
}

void ntg_color_block_init(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    struct ntg_object_layout_ops object_data = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_object_hooks hooks = {0};

    ntg_object_init((ntg_object*)color_block, object_data,
            hooks, &NTG_TYPE_COLOR_BLOCK);

    color_block->__color = NT_COLOR_DEFAULT;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_color_block_deinit(ntg_color_block* block)
{
    block->__color = NT_COLOR_DEFAULT;
    ntg_object_deinit((ntg_object*)block);
}

void ntg_color_block_deinit_(void* _block)
{
    ntg_color_block_deinit(_block);
}

static struct ntg_object_measure measure_fn(
        const ntg_object* _block,
        ntg_orient orient,
        void* lctx,
        sarena* arena)
{
    return (struct ntg_object_measure) {
        .min_size = DEFAULT_SIZE,
        .nat_size = DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
}

static void draw_fn(
        const ntg_object* _block,
        ntg_object_tmp_drawing* out_drawing,
        void* lctx,
        sarena* arena)
{
    ntg_color_block* color_block = (ntg_color_block*)_block;
    struct ntg_xy size = ntg_object_get_size_cont(_block);

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    ntg_vcell_bg(color_block->__color),
                    ntg_xy(j, i));
        }
    }
}
