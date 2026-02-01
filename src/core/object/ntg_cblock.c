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

void ntg_cblock_set_color(ntg_cblock* cblock, nt_color color)
{
    assert(cblock != NULL);

    cblock->__color = color;

    ntg_object_add_dirty((ntg_object*)cblock, NTG_OBJECT_DIRTY_DRAW);
}

void ntg_cblock_init(ntg_cblock* cblock)
{
    assert(cblock != NULL);

    struct ntg_object_layout_ops object_data = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_object_hooks hooks = {0};

    ntg_object_init((ntg_object*)cblock, object_data,
            hooks, &NTG_TYPE_CBLOCK);

    cblock->__color = NT_COLOR_DEFAULT;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_cblock_deinit(ntg_cblock* block)
{
    block->__color = NT_COLOR_DEFAULT;
    ntg_object_deinit((ntg_object*)block);
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
    ntg_cblock* cblock = (ntg_cblock*)_block;
    struct ntg_xy size = ntg_object_get_size_cont(_block);

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    ntg_vcell_bg(cblock->__color),
                    ntg_xy(j, i));
        }
    }
}
