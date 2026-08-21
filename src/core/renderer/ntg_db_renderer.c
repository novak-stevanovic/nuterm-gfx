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

static int erase(ntg_db_renderer* renderer);
static int full_empty_render(ntg_db_renderer* renderer, struct ntg_xy size);

static int optimized_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
        struct ntg_xy size,
        struct ntg_xy old_size,
        sarena* arena);

static int full_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
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
        .erase_mode = NTG_DB_RENDERER_ERASE_SCROLL,
        .term_buff_size = NTG_DB_RENDERER_TERM_SIZE_AUTO
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

    if((opts_final.erase_mode < NTG_DB_RENDERER_ERASE_SCROLL) ||
            (opts_final.erase_mode > NTG_DB_RENDERER_ERASE_NONE) ||
            (opts_final.term_buff_size == 0))
        return NTG_ERR_INV_ARG;

    int _status;

    _status = ntg_renderer_init_inherit(
            (ntg_renderer*)renderer,
            &NTG_DB_RENDERER_VTABLE);
    if(_status != 0)
        return _status;

    _status = ntg_stage_draw_init(&renderer->priv.backbuff);
    if(_status != 0)
    {
        ntg_renderer_deinit(ntg_rnd(renderer));
        return _status;
    }

    renderer->priv.term_buff = malloc(opts_final.term_buff_size);
    if(!renderer->priv.term_buff)
    {
        ntg_stage_draw_deinit(&renderer->priv.backbuff);
        ntg_renderer_deinit(ntg_rnd(renderer));
        return NTG_ERR_ALLOC_FAIL;
    }

    renderer->priv.old_size = ntg_xy(0, 0);
    renderer->priv.force_full_render = false;
    renderer->ro.opts = opts_final;
    return 0;
}

int ntg_db_renderer_deinit(ntg_db_renderer* renderer)
{
    if(!renderer) return NTG_ERR_INV_ARG;

    ntg_stage_draw_deinit(&renderer->priv.backbuff);
    free(renderer->priv.term_buff);

    renderer->priv.backbuff = (ntg_stage_draw) {0};
    renderer->priv.old_size = ntg_xy(0, 0);
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
            ntg_stage_draw_get_size(stage_drawing) :
            ntg_xy(0, 0));
    bool resize = !(ntg_xy_are_eql(renderer->priv.old_size, size));
    bool full_render_req = resize || renderer->priv.force_full_render;

    struct ntg_xy old_size;
    old_size = ntg_stage_draw_get_size(&renderer->priv.backbuff);
    _status = ntg_stage_draw_set_size(&renderer->priv.backbuff, size);
    if(_status)
    {
        renderer->priv.force_full_render = true;
        return _status;
    }

    _status = nt_buffer_enable(renderer->priv.term_buff, renderer->ro.opts.term_buff_size);
    if(_status)
    {
        renderer->priv.force_full_render = true;
        return _status;
    }

    int rval = 0;

    if(stage_drawing == NULL)
    {
        rval = full_empty_render(renderer, size);
    }
    else if(full_render_req)
    {
        _status = erase(renderer);
        if(_status && !rval) rval = _status;

        _status = full_render(renderer, stage_drawing, size, arena);
        if(_status && !rval) rval = _status;
    }
    else
    {
        rval = optimized_render(renderer, stage_drawing, size, old_size, arena);
    }

    renderer->priv.old_size = size;

    nt_buffer_disable(NT_BUFF_FLUSH, NULL);

    renderer->priv.force_full_render = (rval != 0);

    return rval;
}

void ntg_db_renderer_deinit_fn(ntg_renderer* _renderer)
{
    ntg_db_renderer_deinit((ntg_db_renderer*)_renderer);
}

const struct ntg_renderer_vtable NTG_DB_RENDERER_VTABLE = {
    .render_fn = ntg_db_renderer_render_fn,
    .deinit_fn = ntg_db_renderer_deinit_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static int erase(ntg_db_renderer* renderer)
{
    if(renderer->ro.opts.erase_mode == NTG_DB_RENDERER_ERASE_NONE)
        return 0;

    if(renderer->ro.opts.erase_mode == NTG_DB_RENDERER_ERASE_ALL)
    {
        int status = nt_erase_screen();
        if(status != 0)
            return status;
    }

    return nt_erase_scrollback();
}

static int full_empty_render(ntg_db_renderer* renderer, struct ntg_xy size)
{
    ntg_log_log("RENDER: FULL EMPTY");
    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_stage_draw_set(
                    &renderer->priv.backbuff,
                    ntg_cell_default(),
                    ntg_xy(j, i));
        }
    }

    return erase(renderer);
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
        struct ntg_xy size,
        struct ntg_xy old_size,
        sarena* arena)
{
    ntg_log_log("RENDER: OPTIMIZED");

    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_ALLOC_FAIL;

    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_ALLOC_FAIL;

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
            it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy(j, i));

            if((i < old_size.y) && (j < old_size.x))
            {
                it_bb_cell = ntg_stage_draw_get(&renderer->priv.backbuff, ntg_xy(j, i));
                if(ntg_cell_are_eql(it_bb_cell, it_draw_cell))
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

                ntg_stage_draw_set(&renderer->priv.backbuff, it_draw_cell, ntg_xy(j + k, i));
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
        struct ntg_xy size,
        sarena* arena)
{
    ntg_log_log("RENDER: FULL");

    uint32_t* row32_buff = sarena_malloc(arena, size.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_ALLOC_FAIL;

    size_t row_buff_cap = size.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_ALLOC_FAIL;

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

            ntg_stage_draw_set(&renderer->priv.backbuff, it_draw_cell, ntg_xy(j, i));

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell.gfx, ntg_xy(j, i), size.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy(j + k, i));
                row32_buff[k] = it_draw_cell.cp;

                ntg_stage_draw_set(&renderer->priv.backbuff, it_draw_cell, ntg_xy(j + k, i));
            }

            _status = uc_utf32_to_utf8(row32_buff, k, row_buff, row_buff_cap, 0, NULL, &_uc_len);

            nt_cursor_move(j, i);

            nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx);
            j += k;
        }
    }

    return 0;
}
