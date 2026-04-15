#include "nt.h"
#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

// TODO: Vertical optimized rendering?

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_def_renderer_init(ntg_def_renderer* renderer)
{
    assert(renderer != NULL);

    struct ntg_renderer_vtable vtable = {0};
    vtable.render_fn = _ntg_def_renderer_render_fn;

    ntg_renderer_init((ntg_renderer*)renderer, &vtable);

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
        struct ntg_xy size,
        struct ntg_xy old_size,
        sarena* arena);

static void full_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        sarena* arena);

/* -------------------------------------------------------------------------- */

void ntg_def_renderer_deinit(ntg_def_renderer* renderer)
{
    ntg_stage_drawing_deinit(&renderer->__backbuff);

    renderer->__old_size = ntg_xy(0, 0);

    ntg_renderer_deinit((ntg_renderer*)renderer);
}

void ntg_def_renderer_deinit_(void* _renderer)
{
    ntg_def_renderer_deinit(_renderer);
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

    struct ntg_xy old_size;
    old_size = ntg_stage_drawing_get_size(&renderer->__backbuff);
    struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
    ntg_stage_drawing_set_size(&renderer->__backbuff, size, size_cap);

    void* buffer = sarena_malloc(arena, 50000);
    nt_status _status;
    nt_buffer_enable(buffer, 50000, &_status);
    assert(_status == NT_SUCCESS);

    if(stage_drawing == NULL)
    {
        full_empty_render(renderer, size);
    }
    else
    {
        if(resize)
            full_render(renderer, stage_drawing, size, arena);
        else
            optimized_render(renderer, stage_drawing, size, old_size, arena);
    }

    renderer->__old_size = size;

    nt_buffer_disable(NT_BUFF_FLUSH);
}

/* -------------------------------------------------------------------------- */

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size)
{
    size_t i, j;
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

static inline size_t fwd_equal_gfx_search(
        const ntg_stage_drawing* drawing,
        struct nt_gfx pos_gfx,
        struct ntg_xy pos,
        size_t row_size)
{
    size_t j;
    struct ntg_cell it_cell;
    size_t counter = 1;
    for(j = pos.x + 1; j < row_size; j++)
    {
        it_cell = ntg_stage_drawing_get(drawing, ntg_xy(j, pos.y));
        if(nt_gfx_are_equal(pos_gfx, it_cell.gfx))
            counter++;
        else
            break;
    }

    return counter;
}

static void optimized_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy old_size,
        sarena* arena)
{
    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    assert(row32_buff);
    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    assert(row_buff);

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    struct ntg_cell it_bb_cell;
    int _uc_status;
    nt_status _nt_status;
    size_t _uc_len;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x;)
        {
            it_draw_cell = ntg_stage_drawing_get(drawing, ntg_xy(j, i));

            if((i < old_size.y) && (j < old_size.x))
            {
                it_bb_cell = ntg_stage_drawing_get(&renderer->__backbuff, ntg_xy(j, i));
                if(ntg_cell_are_equal(it_bb_cell, it_draw_cell))
                {
                    j++;
                    continue;
                }
            }

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell.gfx, ntg_xy(j, i), size.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_drawing_get(drawing, ntg_xy(j + k, i));
                row32_buff[k] = it_draw_cell.cp;

                ntg_stage_drawing_set(&renderer->__backbuff, it_draw_cell, ntg_xy(j + k, i));
            }
            ntg_log_log("RENDER OPT: %d", k);

            uc_utf32_to_utf8(row32_buff, k, row_buff,
                    row_buff_cap, 0, NULL, &_uc_len,
                    &_uc_status);
            assert(_uc_status == UC_SUCCESS);

            nt_cursor_move(j, i, &_nt_status);
            // Every draw cell in this batch has the same gfx so we can just use the last one
            nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx, &_nt_status);
            assert(_nt_status == NT_SUCCESS);

            j += k;
        }
    }
}

static void full_render(
        ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        sarena* arena)
{
    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    assert(row32_buff);
    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    assert(row_buff);

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    int _uc_status;
    nt_status _nt_status;
    size_t _uc_len;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x;)
        {
            it_draw_cell = ntg_stage_drawing_get(drawing, ntg_xy(j, i));

            ntg_stage_drawing_set(&renderer->__backbuff, it_draw_cell, ntg_xy(j, i));

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell.gfx, ntg_xy(j, i), size.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_drawing_get(drawing, ntg_xy(j + k, i));
                row32_buff[k] = it_draw_cell.cp;

                ntg_stage_drawing_set(&renderer->__backbuff, it_draw_cell, ntg_xy(j + k, i));
            }
            ntg_log_log("RENDER OPT: %d",k);

            uc_utf32_to_utf8(row32_buff, k, row_buff,
                    row_buff_cap, 0, NULL, &_uc_len,
                    &_uc_status);
            assert(_uc_status == UC_SUCCESS);

            nt_cursor_move(j, i, &_nt_status);
            // Every draw cell in this batch has the same gfx so we can just use the last one
            nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx, &_nt_status);
            assert(_nt_status == NT_SUCCESS);

            j += k;
        }
    }
}
