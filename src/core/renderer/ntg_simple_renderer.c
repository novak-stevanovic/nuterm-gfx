#include <assert.h>
#include "nt.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/stage/shared/ntg_stage_drawing.h"

void __ntg_def_renderer_init__(ntg_def_renderer* renderer, void* data)
{
    assert(renderer != NULL);

    __ntg_renderer_init__((ntg_renderer*)renderer,
            __ntg_def_renderer_render_fn, data);
}

void __ntg_def_renderer_deinit__(ntg_def_renderer* renderer)
{
    assert(renderer != NULL);

    __ntg_renderer_deinit__((ntg_renderer*)renderer);
}

void __ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size)
{
    assert(_renderer != NULL);

    if(stage_drawing == NULL)
    {
        nt_erase_screen(NULL);
        nt_erase_scrollback(NULL);
    }
    else
    {
        size_t i, j;
        struct ntg_rcell it_drawing_rcell;
        for(i = 0; i < size.y; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                it_drawing_rcell = *(ntg_stage_drawing_at(stage_drawing, ntg_xy(j, i)));
                nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL);
            }
        }
    }
}
