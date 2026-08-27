#ifndef NTG_DB_RENDERER_H
#define NTG_DB_RENDERER_H

#include "shared/ntg_shared.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage_draw.h"

#define NTG_DB_RENDERER_TBUFF_SIZE_AUTO 50000

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_db_renderer_opts
{
    size_t term_buff_size;
};

static const struct ntg_db_renderer_opts NTG_DB_RENDERER_OPTS_ZERO = {0};

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

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_renderer_vtable NTG_DB_RENDERER_VTABLE;

NTG_API bool
ntg_db_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena);

NTG_API void
ntg_db_renderer_deinit_fn(ntg_entity* _renderer);

#endif // NTG_DB_RENDERER_H
