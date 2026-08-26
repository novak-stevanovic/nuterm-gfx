#ifndef NTG_DB_RENDERER_H
#define NTG_DB_RENDERER_H

#include "shared/ntg_shared.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage_draw.h"

#define NTG_DB_RENDERER_TBUFF_SIZE_AUTO 100000

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_db_renderer_opts
{
    size_t term_buff_size;
};

NTG_API struct ntg_db_renderer_opts
ntg_db_renderer_opts_default(void);

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_db_renderer
{
    ntg_renderer _base;

    struct
    {
        struct ntg_cell* bbuff;
        struct ntg_xy bbuff_size;

        bool force_full_render;

        char* term_buff;
    } priv;

    struct
    {
        struct ntg_db_renderer_opts opts;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_db_renderer_init(
        ntg_db_renderer* renderer,
        const struct ntg_db_renderer_opts* opts);

NTG_API int
ntg_db_renderer_deinit(ntg_db_renderer* renderer);

NTG_API void
ntg_db_renderer_deinit_void(void* _renderer);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_db_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena);

NTG_API void
ntg_db_renderer_deinit_fn(ntg_entity* _renderer);

NTG_API extern const struct ntg_renderer_vtable NTG_DB_RENDERER_VTABLE;

#endif // NTG_DB_RENDERER_H
