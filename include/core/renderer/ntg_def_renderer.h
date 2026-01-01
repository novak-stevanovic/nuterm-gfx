#ifndef _NTG_DB_RENDERER_H_
#define _NTG_DB_RENDERER_H_

#include "core/renderer/ntg_renderer.h"

typedef struct ntg_def_renderer ntg_def_renderer;
typedef struct ntg_cell_vecgrid ntg_cell_vecgrid;
typedef struct nt_charbuff nt_charbuff;
typedef struct ntg_loop ntg_loop;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_renderer
{
    ntg_renderer __base;

    ntg_stage_drawing* __backbuff;

    struct ntg_xy __old_size;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_renderer* ntg_def_renderer_new(ntg_entity_system* system);
void _ntg_def_renderer_init_(ntg_def_renderer* renderer);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_renderer_deinit_fn(ntg_entity* entity);

void _ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing);

#endif // _NTG_DB_RENDERER_H_
