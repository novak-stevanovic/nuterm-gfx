#ifndef _NTG_DB_RENDERER_H_
#define _NTG_DB_RENDERER_H_

#include "core/renderer/ntg_renderer.h"

typedef struct ntg_simple_renderer ntg_def_renderer;
typedef struct ntg_rcell_vgrid ntg_rcell_vgrid;
typedef struct nt_charbuff nt_charbuff;

struct ntg_def_renderer
{
    ntg_renderer __base;
};

void __ntg_def_renderer_init__(ntg_def_renderer* renderer, void* data);
void __ntg_def_renderer_deinit__(ntg_def_renderer* renderer);

void __ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size);

#endif // _NTG_DB_RENDERER_H_
