#ifndef _NTG_APP_RENDERER_H_
#define _NTG_APP_RENDERER_H_

#include "shared/ntg_xy.h"

typedef struct ntg_app_renderer ntg_app_renderer;
typedef struct ntg_scene_drawing ntg_scene_drawing;
typedef enum ntg_app_render_mode ntg_app_render_mode;

typedef void (*ntg_app_render_fn)(
        ntg_app_renderer* renderer,
        const ntg_scene_drawing* scene_drawing,
        struct ntg_xy size,
        ntg_app_render_mode mode);

struct ntg_app_renderer
{
    ntg_app_render_fn __render_fn;
};

enum ntg_app_render_mode
{
    NTG_APP_RENDER_MODE_OPTIMIZED,
    NTG_APP_RENDER_MODE_FULL
};

void __ntg_app_renderer_init__(ntg_app_renderer* renderer,
        ntg_app_render_fn render_fn);
void __ntg_app_renderer_deinit__(ntg_app_renderer* renderer);

void ntg_app_renderer_render(
        ntg_app_renderer* renderer,
        const ntg_scene_drawing* scene_drawing,
        struct ntg_xy size,
        ntg_app_render_mode mode);

#endif // _NTG_APP_RENDERER_H_
