#include <assert.h>
#include "core/renderer/ntg_db_renderer.h"
#include "base/ntg_cell.h"
#include "core/loop/ntg_loop.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "nt.h"
#include "nt_charbuff.h"
#include "base/event/ntg_event.h"
#include "shared/ntg_log.h"

#define CHARBUFF_CAP 100000

static void __listenable_handler(void* subscriber, const ntg_event* event);

static void __full_empty_render(ntg_db_renderer* renderer, struct ntg_xy size);

static void __full_render(ntg_db_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size);

static void __optimized_render(ntg_db_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size);

/* -------------------------------------------------------------------------- */

void __ntg_db_renderer_init__(
        ntg_db_renderer* renderer,
        ntg_loop* loop,
        void* data)
{
    assert(renderer != NULL);
    assert(loop != NULL);

    __ntg_renderer_init__(
            (ntg_renderer*)renderer,
            __ntg_db_render_fn,
            data);

    renderer->__backbuff = ntg_rcell_vgrid_new();
    assert(renderer->__backbuff != NULL);
    renderer->__charbuff = nt_charbuff_new(CHARBUFF_CAP);
    assert(renderer->__charbuff != NULL);

    renderer->__resize = true;

    ntg_listenable_listen(ntg_loop_get_listenable(loop), renderer, __listenable_handler);
    renderer->__loop = loop;
}

void __ntg_db_renderer_deinit__(ntg_db_renderer* renderer)
{
    assert(renderer != NULL);

    __ntg_renderer_deinit__((ntg_renderer*)renderer);

    ntg_rcell_vgrid_destroy(renderer->__backbuff);
    renderer->__backbuff = NULL;
    nt_charbuff_destroy(renderer->__charbuff);
    renderer->__charbuff = NULL;

    renderer->__resize = false;

    ntg_listenable_stop_listening(
            ntg_loop_get_listenable(renderer->__loop),
            renderer,
            __listenable_handler);
    renderer->__loop = NULL;
}

void __ntg_db_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size)
{
    assert(_renderer != NULL);

    ntg_db_renderer* renderer = (ntg_db_renderer*)_renderer;
    
    nt_buffer_enable(renderer->__charbuff);

    if(stage_drawing == NULL)
    {
        __full_empty_render(renderer, size);
    }
    else
    {
        if(renderer->__resize)
        {
            __full_render(renderer, stage_drawing, size);
            renderer->__resize = false; 
        }
        else
        {
            __optimized_render(renderer, stage_drawing, size);
        }
    }

    nt_buffer_disable(NT_BUFF_FLUSH);
}

/* -------------------------------------------------------------------------- */

static void __full_empty_render(ntg_db_renderer* renderer, struct ntg_xy size)
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

static void __optimized_render(ntg_db_renderer* renderer,
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

static void __full_render(ntg_db_renderer* renderer,
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

static void __listenable_handler(void* _subscriber, const ntg_event* event)
{
    assert(_subscriber != NULL);
    assert(event != NULL);

    ntg_db_renderer* renderer = (ntg_db_renderer*)_subscriber;

    if(event->_type == NT_EVENT_RESIZE)
    {
        renderer->__resize = true;
    }
}
