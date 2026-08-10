#include "nt.h"
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

// TODO: Vertical optimized rendering?

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static int full_empty_render(ntg_default_renderer* renderer, struct ntg_xy size);

static int optimized_render(
        ntg_default_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy old_size,
        sarena* arena);

static int full_render(
        ntg_default_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        sarena* arena);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

void ntg_default_renderer_init(
        ntg_default_renderer* renderer,
        size_t buff_size,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!renderer || !buff_size)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    int _status;

    ntg_renderer_init_inherit(
            (ntg_renderer*)renderer,
            &NTG_DEFAULT_RENDERER_VTABLE,
            &_status);
    switch(_status)
    {
        case 0:
            break;
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    ntg_stage_drawing_init(&renderer->__backbuff, &_status);
    switch(_status)
    {
        case 0:
            break;
        default:
            ntg_renderer_deinit(ntg_rnd(renderer));
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    renderer->__buff = malloc(buff_size);
    if(!renderer->__buff)
    {
        ntg_stage_drawing_deinit(&renderer->__backbuff);
        ntg_renderer_deinit(ntg_rnd(renderer));
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    renderer->__old_size = ntg_xy(0, 0);
    renderer->__buff_size = buff_size;
}

void ntg_default_renderer_deinit(ntg_default_renderer* renderer)
{
    if(!renderer) return;

    ntg_stage_drawing_deinit(&renderer->__backbuff);
    free(renderer->__buff);

    renderer->__backbuff = (ntg_stage_drawing) {0};
    renderer->__old_size = ntg_xy(0, 0);
    renderer->__buff = NULL;
    renderer->__buff_size = 0;

    ntg_renderer_deinit(ntg_rnd(renderer));
}

void ntg_default_renderer_deinit_void(void* _renderer)
{
    if(!_renderer) return;

    ntg_default_renderer_deinit(_renderer);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

void ntg_default_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena,
        int* out_status)
{
    ntg_set_out(out_status, 0);
    if(!_renderer)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    int _status;

    ntg_default_renderer* renderer = (ntg_default_renderer*)_renderer;
    struct ntg_xy size = (stage_drawing ? ntg_stage_drawing_get_size(stage_drawing) : ntg_xy(0, 0));
    bool resize = !(ntg_xy_are_eql(renderer->__old_size, size));

    struct ntg_xy old_size;
    old_size = ntg_stage_drawing_get_size(&renderer->__backbuff);
    struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
    ntg_stage_drawing_set_size(&renderer->__backbuff, size, size_cap, &_status);
    if(_status)
        ntg_vreturn(out_status, NTG_ERR_RENDER_FAIL);

    int rval = 0;
    bool buff_disable = false;

    nt_buffer_enable(renderer->__buff, renderer->__buff_size, &_status);
    switch(_status)
    {
        case 0:
            buff_disable = true;
            break;
        default:
            rval = NTG_ERR_RENDER_FAIL_RCVR;
            break;
    }
    if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;

    if(stage_drawing == NULL)
    {
        _status = full_empty_render(renderer, size);
        if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;
    }
    else
    {
        if(resize)
        {
            nt_erase_screen(&_status);
            if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;

            nt_erase_scrollback(&_status);
            if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;

            _status = full_render(renderer, stage_drawing, size, arena);
            if(_status) rval = _status;
        }
        else
        {
            _status = optimized_render(renderer, stage_drawing, size, old_size, arena);
            if(_status) rval = _status;
        }
    }

    renderer->__old_size = size;

    if(buff_disable)
        nt_buffer_disable(NT_BUFF_FLUSH);

    ntg_set_out(out_status, rval);
}

void ntg_default_renderer_deinit_fn(ntg_renderer* _renderer)
{
    ntg_default_renderer_deinit((ntg_default_renderer*)_renderer);
}

const struct ntg_renderer_vtable NTG_DEFAULT_RENDERER_VTABLE = {
    .render_fn = ntg_default_renderer_render_fn,
    .deinit_fn = ntg_default_renderer_deinit_fn
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static int full_empty_render(ntg_default_renderer* renderer, struct ntg_xy size)
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

    int _status = 0;
    int rval = 0;

    nt_erase_screen(&_status);
    if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;
    nt_erase_scrollback(&_status);
    if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;

    return rval;
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
        if(nt_gfx_are_eql(pos_gfx, it_cell.gfx))
            counter++;
        else
            break;
    }

    return counter;
}

static int optimized_render(
        ntg_default_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy old_size,
        sarena* arena)
{
    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_RENDER_FAIL;

    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_RENDER_FAIL;

    int rval = 0;

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    struct ntg_cell it_bb_cell;
    int _status;
    size_t _uc_len;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x;)
        {
            it_draw_cell = ntg_stage_drawing_get(drawing, ntg_xy(j, i));

            if((i < old_size.y) && (j < old_size.x))
            {
                it_bb_cell = ntg_stage_drawing_get(&renderer->__backbuff, ntg_xy(j, i));
                if(ntg_cell_are_eql(it_bb_cell, it_draw_cell))
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
            uc_utf32_to_utf8(row32_buff, k, row_buff,
                    row_buff_cap, 0, NULL, &_uc_len,
                    &_status);
            if(!_status)
            {
                nt_cursor_move(j, i, &_status);
                if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;
                nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx, &_status);
                if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;
            }
            else
                rval = NTG_ERR_RENDER_FAIL_RCVR;
            j += k;
        }
    }

    return rval;
}

static int full_render(
        ntg_default_renderer* renderer,
        const ntg_stage_drawing* drawing,
        struct ntg_xy size,
        sarena* arena)
{
    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_RENDER_FAIL;

    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_RENDER_FAIL;

    int rval = 0;

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    int _status;
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

            uc_utf32_to_utf8(row32_buff, k, row_buff,
                    row_buff_cap, 0, NULL, &_uc_len,
                    &_status);
            if(!_status)
            {
                nt_cursor_move(j, i, &_status);
                if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;
                nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx, &_status);
                if(_status) rval = NTG_ERR_RENDER_FAIL_RCVR;
            }
            else
                rval = NTG_ERR_RENDER_FAIL_RCVR;
            j += k;
        }
    }

    return rval;
}
