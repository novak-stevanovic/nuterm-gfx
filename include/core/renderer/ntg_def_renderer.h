#ifndef _NTG_DB_RENDERER_H_
#define _NTG_DB_RENDERER_H_

#include "core/renderer/ntg_renderer.h"

typedef struct ntg_def_renderer ntg_def_renderer;
typedef struct ntg_rcell_vgrid ntg_rcell_vgrid;
typedef struct nt_charbuff nt_charbuff;
typedef struct ntg_loop ntg_loop;

/* Double-buffered app renderer that listens to ntg_app for resizes. When
 * a resize happens, the next render will not be optimized. */
struct ntg_def_renderer
{
    ntg_renderer __base;

    ntg_rcell_vgrid* __backbuff;
    nt_charbuff* __charbuff;

    ntg_loop* __loop;

    bool __resize;
};

/* Listenable must raise NTG_EVT_APP_RESIZE for the renderer to work correctly */
void __ntg_def_renderer_init__(
        ntg_def_renderer* renderer,
        ntg_loop* loop,
        void* data);
void __ntg_def_renderer_deinit__(ntg_def_renderer* renderer);

void __ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size);

#endif // _NTG_DB_RENDERER_H_
