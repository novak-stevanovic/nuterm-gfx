#include <assert.h>
#include "nt.h"
#include "core/app/ntg_simple_app_renderer.h"
#include "core/stage/shared/ntg_stage_drawing.h"

void __ntg_simple_app_renderer_init__(ntg_simple_app_renderer* renderer)
{
    assert(renderer != NULL);

    __ntg_app_renderer_init__((ntg_app_renderer*)renderer, __ntg_simple_app_render_fn);
}

void __ntg_simple_app_renderer_deinit__(ntg_simple_app_renderer* renderer)
{
    assert(renderer != NULL);

    __ntg_app_renderer_deinit__((ntg_app_renderer*)renderer);
}

void __ntg_simple_app_render_fn(
        ntg_app_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size)
{
    assert(_renderer != NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_stage_drawing_at(stage_drawing, ntg_xy(j, i)));
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL, NULL);
        }
    }
}
