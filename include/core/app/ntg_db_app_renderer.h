#ifndef _NTG_DB_APP_RENDERER_H_
#define _NTG_DB_APP_RENDERER_H_

#include "core/app/ntg_app_renderer.h"

typedef struct ntg_db_app_renderer ntg_db_app_renderer;
typedef struct ntg_rcell_vgrid ntg_rcell_vgrid;
typedef struct nt_charbuff nt_charbuff;

struct ntg_db_app_renderer
{
    ntg_app_renderer __base;

    ntg_rcell_vgrid* __backbuff;
    nt_charbuff* __charbuff;
};

void __ntg_db_app_renderer_init__(ntg_db_app_renderer* renderer);
void __ntg_db_app_renderer_deinit__(ntg_db_app_renderer* renderer);

void __ntg_db_app_render_fn(
        ntg_app_renderer* _renderer,
        const ntg_scene_drawing* scene_drawing,
        struct ntg_xy size,
        ntg_app_render_mode mode);

#endif // _NTG_DB_APP_RENDERER_H_
