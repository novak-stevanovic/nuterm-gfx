#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#define DEFAULT_SIZE 1

static struct ntg_object_measure measure_fn(
        const ntg_object* _block,
        ntg_orient orient,
        void* layout_ch,
        sarena* arena);

static void draw_fn(
        const ntg_object* _block,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_color_block_set_color(ntg_color_block* color_block, struct nt_color color)
{
    if(!color_block) return;

    struct nt_color old_color = color_block->_color;

    if(nt_color_are_equal(old_color, color))
        return;

    color_block->_color = color;

    ntg_object_mark_dirty((ntg_object*)color_block, NTG_OBJECT_DIRTY_DRAW);

    if(color_block->hooks.on_color_chng_fn)
        color_block->hooks.on_color_chng_fn(color_block, old_color, color);
}

void ntg_color_block_init(
        ntg_color_block* color_block,
        struct nt_color color,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!color_block)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    struct ntg_object_vtable vtable = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .fixup_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn,
        .rm_child_fn = NULL
    };

    int _status;

    ntg_object_init(
            (ntg_object*)color_block,
            &vtable,
            &NTG_TYPE_COLOR_BLOCK,
            &_status);

    color_block->hooks = (struct ntg_color_block_hooks) {0};

    color_block->_color = NT_COLOR_DEFAULT;

    ntg_color_block_set_color(color_block, color);
}

void ntg_color_block_deinit(ntg_color_block* block)
{
    if(!block) return;

    block->_color = NT_COLOR_DEFAULT;
    ntg_object_deinit((ntg_object*)block);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_color_block_deinit_(void* _block)
{
    ntg_color_block_deinit(_block);
}

static struct ntg_object_measure measure_fn(
        const ntg_object* _block,
        ntg_orient orient,
        void* layout_ch,
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
        void* layout_ch,
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
                    ntg_vcell_bg(color_block->_color),
                    ntg_xy(j, i));
        }
    }
}
