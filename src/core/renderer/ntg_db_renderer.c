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
        struct ntg_cell* new_bbuff,
        ntg_xy size);

static int optimized_render(
        ntg_db_renderer* renderer,
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        ntg_xy size,
        sarena* arena);

static int full_render(
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        ntg_xy size,
        sarena* arena);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_db_renderer_init(
        ntg_db_renderer* renderer,
        const struct ntg_db_renderer_init_opts* opts)
{
    if(!renderer)
        return NTG_ERR_INV_ARG;

    int status = ntg_renderer_init_inherit(
            ntg_rnd(renderer),
            &NTG_DB_RENDERER_VTABLE,
            &NTG_TYPE_DB_RENDERER);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(renderer);

    struct ntg_db_renderer_init_opts opts_final = {0};
    if(opts) opts_final = (*opts);
    if(opts_final.term_buff_size == 0)
        opts_final.term_buff_size = NTG_DB_RENDERER_TBUFSZ_AUTO;

    renderer->priv.term_buff_size = opts_final.term_buff_size;

    renderer->priv.term_buff = malloc(opts_final.term_buff_size);
    if(!renderer->priv.term_buff)
    {
        ntg_renderer_deinit(ntg_rnd(renderer));
        return NTG_ERR_ALLOC_FAIL;
    }

    return 0;
}

int ntg_db_renderer_deinit(ntg_db_renderer* renderer)
{
    if(!renderer) return NTG_ERR_INV_ARG;

    free(renderer->priv.bbuff);
    free(renderer->priv.term_buff);

    ntg_object_zero(renderer);
    ntg_renderer_deinit(ntg_rnd(renderer));

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_renderer_vtable NTG_DB_RENDERER_VTABLE = {
    .base.deinit_fn = ntg_db_renderer_deinit_fn,
    .render_fn = ntg_db_renderer_render_fn
};

void ntg_db_renderer_deinit_fn(ntg_object* _renderer)
{
    ntg_db_renderer_deinit((ntg_db_renderer*)_renderer);
}

bool ntg_db_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena)
{
    if(!_renderer) return true;

    int _status;

    ntg_db_renderer* renderer = (ntg_db_renderer*)_renderer;
    ntg_xy size = (stage_drawing ?
            stage_drawing->ro.size : 
            ntg_xy_new(0, 0));
    bool resize = !(ntg_xy_are_eql(renderer->priv.bbuff_size, size));
    bool full_render_req = resize || renderer->priv.force_full_render;

    size_t size_prod = size.ro.x * size.ro.y;
    bool new_bbuff = resize || !renderer->priv.bbuff;
    struct ntg_cell* write_bbuff;
    if(new_bbuff)
    {
        write_bbuff = size_prod ? malloc(sizeof(struct ntg_cell) * size_prod) : NULL;
        if(size_prod && !write_bbuff)
            return true;
    }
    else
    {
        write_bbuff = renderer->priv.bbuff;
    }

    _status = nt_buffer_enable(renderer->priv.term_buff, renderer->priv.term_buff_size);
    if(_status)
    {
        if(new_bbuff) free(write_bbuff);
        return true;
    }

    bool rval = 0;

    if(!stage_drawing || ntg_xy_is_zero_any(size))
    {
        _status = full_empty_render(write_bbuff, size);
        if(_status) rval = true;
    }
    else if(full_render_req)
    {
        _status = full_render(stage_drawing, write_bbuff, size, arena);
        if(_status) rval = true;
    }
    else
    {
        _status = optimized_render(renderer, stage_drawing, write_bbuff, size, arena);
        if(_status) rval = true;
    }

    nt_buffer_disable(NT_BUFF_FLUSH, NULL);

    if(new_bbuff)
    {
        free(renderer->priv.bbuff);
        renderer->priv.bbuff = write_bbuff;
        renderer->priv.bbuff_size = size;
    }

    renderer->priv.force_full_render = (rval != 0);

    return rval;
}


/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#define bbuff_set(bbuff, pos, size, vcell) \
    if(bbuff) bbuff[size.ro.x * pos.ro.y + pos.ro.x] = vcell;

#define bbuff_get(bbuff, pos, size) \
    (bbuff ? bbuff[size.ro.x * pos.ro.y + pos.ro.x] : NTG_CELL_ZERO)

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static int full_empty_render(
        struct ntg_cell* new_bbuff,
        ntg_xy size)
{
    ntg_log_log("RENDER: FULL EMPTY");
    size_t i, j;
    for(i = 0; i < size.ro.y; i++)
    {
        for(j = 0; j < size.ro.x; j++)
        {
            bbuff_set(new_bbuff, ntg_xy_new(j, i), size, ntg_renderer_cell_normalize(NTG_CELL_ZERO));
        }
    }

    nt_erase_screen();
    nt_erase_scrollback();

    return 0;
}

static inline size_t fwd_equal_gfx_search(
        const ntg_stage_draw* drawing,
        struct ntg_cell start_cell,
        ntg_xy pos,
        size_t row_size)
{
    start_cell = ntg_renderer_cell_normalize(start_cell);

    size_t j;
    struct ntg_cell it_cell;
    size_t counter = 1;
    for(j = pos.ro.x + 1; j < row_size; j++)
    {
        it_cell = ntg_stage_draw_get(drawing, ntg_xy_new(j, pos.ro.y));
        it_cell = ntg_renderer_cell_normalize(it_cell);

        if(ntg_cell_are_eql_render(start_cell, it_cell))
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
        ntg_xy size,
        sarena* arena)
{
    ntg_log_log("RENDER: OPTIMIZED");

    uint32_t* row32_buff = sarena_malloc(arena, size.ro.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_ALLOC_FAIL;

    size_t row_buff_cap = size.ro.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_ALLOC_FAIL;

    ntg_xy bbuff_size = renderer->priv.bbuff_size;
    struct ntg_cell* bbuff = renderer->priv.bbuff;

    nt_erase_scrollback();

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    struct ntg_cell it_bbuff_cell;
    int _status;
    size_t _uc_len;
    for(i = 0; i < size.ro.y; i++)
    {
        for(j = 0; j < size.ro.x;)
        {
            it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy_new(j, i));
            it_draw_cell = ntg_renderer_cell_normalize(it_draw_cell);

            if((i < bbuff_size.ro.y) && (j < bbuff_size.ro.x))
            {
                it_bbuff_cell = bbuff_get(bbuff, ntg_xy_new(j, i), size);
                if(ntg_cell_are_eql_render(it_bbuff_cell, it_draw_cell))
                {
                    bbuff_set(new_bbuff, ntg_xy_new(j, i), size, it_draw_cell);
                    j++;
                    continue;
                }
            }

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell, ntg_xy_new(j, i), size.ro.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy_new(j + k, i));
                it_draw_cell = ntg_renderer_cell_normalize(it_draw_cell);
                row32_buff[k] = it_draw_cell.cp;

                bbuff_set(new_bbuff, ntg_xy_new(j + k, i), size, it_draw_cell);
            }
            _status = uc_utf32_to_utf8(row32_buff, k, row_buff, row_buff_cap, 0, NULL, &_uc_len);

            nt_cursor_move(j, i);

            if(_status)
            {
                memset(row_buff, ' ', k);
                nt_write_str((const char*)row_buff, k, it_draw_cell.gfx);
            }
            else
            {
                nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx);
            }
            j += k;
        }
    }

    return 0;
}

static int full_render(
        const ntg_stage_draw* drawing,
        struct ntg_cell* new_bbuff,
        ntg_xy size,
        sarena* arena)
{
    ntg_log_log("RENDER: FULL");

    uint32_t* row32_buff = sarena_malloc(arena, size.ro.x * sizeof(uint32_t));
    if(!row32_buff) return NTG_ERR_ALLOC_FAIL;

    size_t row_buff_cap = size.ro.x * 4;
    uint8_t* row_buff = sarena_malloc(arena, row_buff_cap * sizeof(uint8_t));
    if(!row_buff) return NTG_ERR_ALLOC_FAIL;

    nt_erase_scrollback();

    size_t i, j, k;
    size_t it_opt;
    struct ntg_cell it_draw_cell;
    int _status;
    size_t _uc_len;
    for(i = 0; i < size.ro.y; i++)
    {
        for(j = 0; j < size.ro.x;)
        {
            it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy_new(j, i));
            it_draw_cell = ntg_renderer_cell_normalize(it_draw_cell);

            it_opt = fwd_equal_gfx_search(drawing, it_draw_cell, ntg_xy_new(j, i), size.ro.x);

            for(k = 0; k < it_opt; k++)
            {
                it_draw_cell = ntg_stage_draw_get(drawing, ntg_xy_new(j + k, i));
                it_draw_cell = ntg_renderer_cell_normalize(it_draw_cell);

                row32_buff[k] = it_draw_cell.cp;

                bbuff_set(new_bbuff, ntg_xy_new(j + k, i), size, it_draw_cell);
            }

            _status = uc_utf32_to_utf8(row32_buff, k, row_buff, row_buff_cap, 0, NULL, &_uc_len);

            nt_cursor_move(j, i);

            if(_status)
            {
                memset(row_buff, ' ', k);
                nt_write_str((const char*)row_buff, k, it_draw_cell.gfx);
            }
            else
            {
                nt_write_str((const char*)row_buff, _uc_len, it_draw_cell.gfx);
            }
            j += k;
        }
    }

    return 0;
}
