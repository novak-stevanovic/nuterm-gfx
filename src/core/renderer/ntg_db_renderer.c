#include "nt.h"
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

// TODO: Vertical optimized rendering?

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static int full_empty_render(
        ntg_db_renderer* renderer, 
        struct ntg_cell* new_bbuff,
        struct ntg_xy size);

static int optimized_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        struct ntg_xy size,
        sarena* arena);

static int full_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        struct ntg_xy size,
        sarena* arena);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

struct ntg_db_renderer_opts ntg_db_renderer_opts_default(void)
{
    return (struct ntg_db_renderer_opts) {
        .term_buff_size = NTG_DB_RENDERER_TBUFF_SIZE_AUTO
    };
}

int ntg_db_renderer_init(
        ntg_db_renderer* renderer,
        const struct ntg_db_renderer_opts* opts)
{
    if(!renderer)
        return NTG_ERR_INV_ARG;

    struct ntg_db_renderer_opts opts_final =
            (opts ? (*opts) : ntg_db_renderer_opts_default());

    int _status;

    _status = ntg_renderer_init_inherit(
            ntg_rnd(renderer),
            &NTG_DB_RENDERER_VTABLE,
            &NTG_TYPE_DB_RENDERER);
    if(_status != 0)
        return _status;

    renderer->priv.term_buff = malloc(opts_final.term_buff_size);
    if(!renderer->priv.term_buff)
    {
        ntg_renderer_deinit(ntg_rnd(renderer));
        return NTG_ERR_ALLOC_FAIL;
    }

    renderer->priv.bbuff = NULL;
    renderer->priv.bbuff_size = ntg_xy(0, 0);
    renderer->priv.force_full_render = false;
    renderer->ro.opts = opts_final;
    return 0;
}

int ntg_db_renderer_deinit(ntg_db_renderer* renderer)
{
    if(!renderer) return NTG_ERR_INV_ARG;

    free(renderer->priv.bbuff);
    renderer->priv.bbuff = NULL;
    renderer->priv.bbuff_size = ntg_xy(0, 0);
    renderer->priv.force_full_render = false;
    renderer->priv.term_buff = NULL;
    renderer->ro.opts = ntg_db_renderer_opts_default();

    ntg_renderer_deinit(ntg_rnd(renderer));

    return 0;
}

void ntg_db_renderer_deinit_void(void* _renderer)
{
    if(!_renderer) return;

    ntg_db_renderer_deinit(_renderer);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_db_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena)
{
    if(!_renderer)
        return NTG_ERR_INV_ARG;

    int _status;

    ntg_db_renderer* renderer = (ntg_db_renderer*)_renderer;
    struct ntg_xy size = (stage_drawing ?
            stage_drawing->ro.size : 
            ntg_xy(0, 0));
    bool resize = !(ntg_xy_are_eql(renderer->priv.bbuff_size, size));
    bool full_render_req = resize || renderer->priv.force_full_render;

    struct ntg_cell* new_bbuff = malloc(sizeof(struct ntg_cell) * size.x * size.y);

    _status = nt_buffer_enable(renderer->priv.term_buff, renderer->ro.opts.term_buff_size);
    if(_status)
        return _status;

    int rval = 0;

    if(stage_drawing == NULL)
    {
        rval = full_empty_render(renderer, new_bbuff, size);
    }
    else if(full_render_req)
    {
        _status = full_render(renderer, stage_drawing, new_bbuff, size, arena);
        if(_status && !rval) rval = _status;
    }
    else
    {
        rval = optimized_render(renderer, stage_drawing, new_bbuff, size, arena);
        if(_status && !rval) rval = _status;
    }

    nt_buffer_disable(NT_BUFF_FLUSH, NULL);

    free(renderer->priv.bbuff);
    renderer->priv.bbuff = new_bbuff;
    renderer->priv.bbuff_size = size;

    renderer->priv.force_full_render = (rval != 0);

    return rval;
}

void ntg_db_renderer_deinit_fn(ntg_entity* _renderer)
{
    ntg_db_renderer_deinit((ntg_db_renderer*)_renderer);
}

const struct ntg_renderer_vtable NTG_DB_RENDERER_VTABLE = {
    .base.deinit_fn = ntg_db_renderer_deinit_fn,
    .render_fn = ntg_db_renderer_render_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#define bbuff_set(bbuff, pos, size, vcell) \
    if(bbuff) bbuff[size.x * pos.y + pos.x] = vcell;

#define bbuff_get(bbuff, pos, size) \
    (bbuff ? bbuff[size.x * pos.y + pos.x] : ntg_cell_default());

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static int full_empty_render(
        ntg_db_renderer* renderer,
        struct ntg_cell* new_bbuff,
        struct ntg_xy size)
{
    ntg_log_log("RENDER: FULL EMPTY");
    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            bbuff_set(new_bbuff, ntg_xy(j, i), size, ntg_cell_default());
        }
    }

    nt_erase_screen();
    nt_erase_scrollback();

    return 0;
}

static inline size_t fwd_equal_gfx_search(
        const ntg_stage_draw* drawing,
        struct nt_gfx pos_gfx,
        struct ntg_xy pos,
        size_t row_size)
{
    size_t j;
    struct ntg_cell it_cell;
    size_t counter = 1;
    for(j = pos.x + 1; j < row_size; j++)
    {
        it_cell = ntg_stage_draw_get(drawing, ntg_xy(j, pos.y));
        if(nt_gfx_are_eql(pos_gfx, it_cell.gfx))
            counter++;
        else
            break;
    }

    return counter;
}

static int optimized_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        struct ntg_xy size,
        sarena* arena)
{
    ntg_log_log("RENDER: OPTIMIZED");

    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_ALLOC_FAIL;

    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_ALLOC_FAIL;

    struct ntg_xy bbuff_size = renderer->priv.bbuff_size;
    struct ntg_cell* bbuff = renderer->priv.bbuff;

    nt_erase_scrollback();

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    struct ntg_cell it_bbuff_cell;
    int _status;
    size_t _uc_len;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x;)
        {
            it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy(j, i));

            if((i < bbuff_size.y) && (j < bbuff_size.x))
            {
                it_bbuff_cell = bbuff_get(bbuff, ntg_xy(j, i), size);
                if(ntg_cell_are_eql(it_bbuff_cell, it_draw_cell))
                {
                    j++;
                    continue;
                }
            }

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell.gfx, ntg_xy(j, i), size.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy(j + k, i));
                row32_buff[k] = it_draw_cell.cp;

                bbuff_set(new_bbuff, ntg_xy(j + k, i), size, it_draw_cell);
            }
            _status = uc_utf32_to_utf8( row32_buff, k, row_buff, row_buff_cap, 0, NULL, &_uc_len);

            nt_cursor_move(j, i);

            nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx);
            j += k;
        }
    }

    return 0;
}

static int full_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        struct ntg_xy size,
        sarena* arena)
{
    ntg_log_log("RENDER: FULL");

    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_ALLOC_FAIL;

    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_ALLOC_FAIL;

    nt_erase_scrollback();

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    int _status;
    size_t _uc_len;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x;)
        {
            it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy(j, i));

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell.gfx, ntg_xy(j, i), size.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy(j + k, i));
                row32_buff[k] = it_draw_cell.cp;

                bbuff_set(new_bbuff, ntg_xy(j + k, i), size, it_draw_cell);
            }

            _status = uc_utf32_to_utf8(row32_buff, k, row_buff, row_buff_cap, 0, NULL, &_uc_len);

            nt_cursor_move(j, i);

            nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx);
            j += k;
        }
    }

    return 0;
}
