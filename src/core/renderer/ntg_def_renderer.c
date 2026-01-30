#include "nt.h"
#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_def_renderer_init(ntg_def_renderer* renderer)
{
    assert(renderer != NULL);

    ntg_renderer_init((ntg_renderer*)renderer, _ntg_def_renderer_render_fn);

    ntg_stage_drawing_init(&renderer->__backbuff);

    renderer->__old_size = ntg_xy(0, 0);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size);

static void optimized_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size);

static void full_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size);

/* -------------------------------------------------------------------------- */

void ntg_def_renderer_deinit(ntg_def_renderer* renderer)
{
    ntg_stage_drawing_deinit(&renderer->__backbuff);

    renderer->__old_size = ntg_xy(0, 0);

    ntg_renderer_deinit((ntg_renderer*)renderer);
}

void _ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena)
{
    assert(_renderer != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)_renderer;
    struct ntg_xy size = ntg_stage_drawing_get_size(stage_drawing);
    bool resize = !(ntg_xy_are_equal(renderer->__old_size, size));

    void* buffer = sarena_malloc(arena, 20000);
    nt_status _status;
    nt_buffer_enable(buffer, 20000, &_status);
    assert(_status == NT_SUCCESS);

    if(stage_drawing == NULL)
    {
        full_empty_render(renderer, size);
    }
    else
    {
        if(resize)
            full_render(renderer, stage_drawing, size);
        else
            optimized_render(renderer, stage_drawing, size);
    }

    renderer->__old_size = size;

    nt_buffer_disable(NT_BUFF_FLUSH);
}

/* -------------------------------------------------------------------------- */

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size)
{
    struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
    ntg_stage_drawing_set_size(&renderer->__backbuff, size, size_cap);

    size_t i, j;
    struct ntg_cell it_drawing_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_stage_drawing_set(
                    &renderer->__backbuff,
                    ntg_cell_default(),
                    ntg_xy(j, i));
        }
    }

    nt_erase_screen(NULL);
    nt_erase_scrollback(NULL);
}

static void optimized_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size)
{
    struct ntg_xy old_back_buffer_size;
    old_back_buffer_size = ntg_stage_drawing_get_size(&renderer->__backbuff);
    struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
    ntg_stage_drawing_set_size(&renderer->__backbuff, size, size_cap);

    size_t i, j;
    struct ntg_cell it_drawing_cell;
    struct ntg_cell it_backbuff_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_cell = ntg_stage_drawing_get(drawing, ntg_xy(j, i));

            if((i < old_back_buffer_size.y) && (j < old_back_buffer_size.x))
            {
                it_backbuff_cell = ntg_stage_drawing_get(&renderer->__backbuff, ntg_xy(j, i));
                if(ntg_cell_are_equal(it_backbuff_cell, it_drawing_cell))
                {
                    continue;
                }
            }

            ntg_stage_drawing_set(&renderer->__backbuff, it_drawing_cell, ntg_xy(j, i));
            nt_write_char_at(it_drawing_cell.codepoint, it_drawing_cell.gfx, j, i, NULL);
        }
    }
}

static void full_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size)
{
    struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
    ntg_stage_drawing_set_size(&renderer->__backbuff, size, size_cap);

    size_t i, j;
    struct ntg_cell it_drawing_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_cell = ntg_stage_drawing_get(drawing, ntg_xy(j, i));

            ntg_stage_drawing_set(&renderer->__backbuff, it_drawing_cell, ntg_xy(j, i));

            nt_write_char_at(it_drawing_cell.codepoint, it_drawing_cell.gfx, j, i, NULL);
        }
    }
}
