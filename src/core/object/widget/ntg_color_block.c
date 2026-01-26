#include "ntg.h"
#include <assert.h>
#define DEFAULT_SIZE 1

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    color_block->__color = color;

    ntg_entity_raise_event_((ntg_entity*)color_block, NTG_EVENT_OBJECT_DIFF, NULL);
}

ntg_color_block* ntg_color_block_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_COLOR_BLOCK,
        .deinit_fn = (ntg_entity_deinit_fn)ntg_color_block_deinit,
        .system = system
    };

    ntg_color_block* new = (ntg_color_block*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_color_block_init(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    struct ntg_widget_layout_ops widget_data = {
        .measure_fn = _ntg_color_block_measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = _ntg_color_block_draw_fn,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_widget_hooks hooks = {0};

    ntg_widget_init((ntg_widget*)color_block, widget_data, hooks);

    color_block->__color = NT_COLOR_DEFAULT;
}


/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_color_block_deinit(ntg_color_block* block)
{
    block->__color = NT_COLOR_DEFAULT;
    ntg_widget_deinit((ntg_widget*)block);
}

struct ntg_object_measure _ntg_color_block_measure_fn(
        const ntg_widget* _block,
        void* _ldata,
        ntg_orient orient,
        bool constrained,
        sarena* arena)
{
    return (struct ntg_object_measure) {
        .min_size = DEFAULT_SIZE,
        .nat_size = DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
}

void _ntg_color_block_draw_fn(
        const ntg_widget* _block,
        void* _ldata,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena)
{
    ntg_color_block* color_block = (ntg_color_block*)_block;
    struct ntg_xy size = ntg_widget_get_cont_size(_block);

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_vcell_bg(color_block->__color);
        }
    }
}
