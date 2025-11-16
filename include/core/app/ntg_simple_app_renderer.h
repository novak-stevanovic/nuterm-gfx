#ifndef _NTG_DB_APP_RENDERER_H_
#define _NTG_DB_APP_RENDERER_H_

#include "core/app/ntg_app_renderer.h"

typedef struct ntg_simple_app_renderer ntg_simple_app_renderer;
typedef struct ntg_rcell_vgrid ntg_rcell_vgrid;
typedef struct nt_charbuff nt_charbuff;

struct ntg_simple_app_renderer
{
    ntg_app_renderer __base;
};

void __ntg_simple_app_renderer_init__(ntg_simple_app_renderer* renderer);
void __ntg_simple_app_renderer_deinit__(ntg_simple_app_renderer* renderer);

void __ntg_simple_app_render_fn(
        ntg_app_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size);

#endif // _NTG_DB_APP_RENDERER_H_
