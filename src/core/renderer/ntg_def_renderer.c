#include <assert.h>
#include "core/renderer/ntg_def_renderer.h"
#include "base/ntg_cell.h"
#include "core/loop/ntg_loop.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "nt.h"
#include "nt_charbuff.h"
#include "base/event/ntg_event.h"
#include "shared/ntg_log.h"

#define CHARBUFF_CAP 100000

static void listenable_handler(void* subscriber, struct ntg_event event);

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size);

static void full_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size);

static void optimized_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size);

/* -------------------------------------------------------------------------- */

void _ntg_def_renderer_init_(ntg_def_renderer* renderer, ntg_loop* loop)
{
    assert(renderer != NULL);
    assert(loop != NULL);

    _ntg_renderer_init_((ntg_renderer*)renderer, __ntg_def_renderer_render_fn);

    renderer->__backbuff = ntg_rcell_vgrid_new();
    assert(renderer->__backbuff != NULL);
    renderer->__charbuff = nt_charbuff_new(CHARBUFF_CAP);
    assert(renderer->__charbuff != NULL);

    renderer->__resize = true;

    ntg_listenable_listen(ntg_loop_get_listenable(loop), renderer, listenable_handler);
    renderer->__loop = loop;
}

void _ntg_def_renderer_deinit_(ntg_def_renderer* renderer)
{
    assert(renderer != NULL);

    _ntg_renderer_deinit_((ntg_renderer*)renderer);

    ntg_rcell_vgrid_destroy(renderer->__backbuff);
    renderer->__backbuff = NULL;
    nt_charbuff_destroy(renderer->__charbuff);
    renderer->__charbuff = NULL;

    renderer->__resize = false;

    ntg_listenable_stop_listening(
            ntg_loop_get_listenable(renderer->__loop),
            renderer,
            listenable_handler);
    renderer->__loop = NULL;
}

void __ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size)
{
    assert(_renderer != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)_renderer;
    
    nt_buffer_enable(renderer->__charbuff);

    if(stage_drawing == NULL)
    {
        full_empty_render(renderer, size);
    }
    else
    {
        if(renderer->__resize)
        {
            full_render(renderer, stage_drawing, size);
            renderer->__resize = false; 
        }
        else
        {
            optimized_render(renderer, stage_drawing, size);
        }
    }

    nt_buffer_disable(NT_BUFF_FLUSH);
}

/* -------------------------------------------------------------------------- */

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size)
{
    ntg_rcell_vgrid_set_size(renderer->__backbuff, size, NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = ntg_rcell_default();
            it_back_buffer_rcell = ntg_rcell_vgrid_at_(renderer->__backbuff,
                    ntg_xy(j, i));

            (*it_back_buffer_rcell) = it_drawing_rcell;
        }
    }

    nt_erase_screen(NULL);
    nt_erase_scrollback(NULL);
}

static void optimized_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size)
{
    struct ntg_xy old_back_buffer_size = ntg_rcell_vgrid_get_size(renderer->__backbuff);
    ntg_rcell_vgrid_set_size(renderer->__backbuff, size, NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_stage_drawing_at(drawing, ntg_xy(j, i)));
            it_back_buffer_rcell = ntg_rcell_vgrid_at_(renderer->__backbuff,
                    ntg_xy(j, i));

            if((i < old_back_buffer_size.y) || (j < old_back_buffer_size.x))
            {
                if(ntg_rcell_are_equal(*it_back_buffer_rcell, it_drawing_rcell))
                    continue;
            }

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL);
        }
    }
}

static void full_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size)
{
    ntg_rcell_vgrid_set_size(renderer->__backbuff, size, NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_stage_drawing_at(drawing, ntg_xy(j, i)));
            it_back_buffer_rcell = ntg_rcell_vgrid_at_(renderer->__backbuff,
                    ntg_xy(j, i));

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL);
        }
    }
}

static void listenable_handler(void* _subscriber, struct ntg_event event)
{
    assert(_subscriber != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)_subscriber;

    if(event.type == NT_EVENT_RESIZE)
    {
        renderer->__resize = true;
    }
}
