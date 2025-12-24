#include <assert.h>
#include <stdlib.h>
#include "core/renderer/ntg_def_renderer.h"
#include "base/ntg_cell.h"
#include "core/loop/ntg_loop.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "nt.h"
#include "nt_charbuff.h"
#include "shared/ntg_log.h"

#define CHARBUFF_CAP 100000

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

static void event_handler(ntg_entity* _subscriber, struct ntg_event event);

ntg_def_renderer* ntg_def_renderer_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_RENDERER,
        .deinit_fn = _ntg_def_renderer_deinit_fn,
        .system = system,
    };

    ntg_def_renderer* new = (ntg_def_renderer*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_def_renderer_init_(ntg_def_renderer* renderer, ntg_loop* loop)
{
    assert(renderer != NULL);
    assert(loop != NULL);

    _ntg_renderer_init_((ntg_renderer*)renderer, _ntg_def_renderer_render_fn);

    renderer->__backbuff = ntg_stage_drawing_new();
    assert(renderer->__backbuff != NULL);
    renderer->__charbuff = nt_charbuff_new(CHARBUFF_CAP);
    assert(renderer->__charbuff != NULL);

    renderer->__resize = true;

    ntg_entity_observe((ntg_entity*)renderer, (ntg_entity*)loop, event_handler);
    renderer->__loop = loop;
}

static void event_handler(ntg_entity* _subscriber, struct ntg_event event)
{
    assert(_subscriber != NULL);

    ntg_log_log("Handler");

    ntg_def_renderer* renderer = (ntg_def_renderer*)_subscriber;

    if(event.type == NTG_EVENT_LOOPRSZ)
    {
        renderer->__resize = true;
    }
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size);
static void full_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size);
static void optimized_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size);

/* -------------------------------------------------------------------------- */

void _ntg_def_renderer_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)entity;

    ntg_stage_drawing_destroy(renderer->__backbuff);
    renderer->__backbuff = NULL;
    nt_charbuff_destroy(renderer->__charbuff);
    renderer->__charbuff = NULL;

    renderer->__resize = false;

    ntg_entity_stop_observing(
            (ntg_entity*)renderer,
            (ntg_entity*)renderer->__loop,
            event_handler);

    renderer->__loop = NULL;

    _ntg_renderer_deinit_fn(entity);
}

void _ntg_def_renderer_render_fn(
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
            ntg_log_log("full");
            full_render(renderer, stage_drawing, size);
            renderer->__resize = false; 
        }
        else
        {
            ntg_log_log("optimized");
            optimized_render(renderer, stage_drawing, size);
        }
    }

    nt_buffer_disable(NT_BUFF_FLUSH);
}

/* -------------------------------------------------------------------------- */

static void full_empty_render(ntg_def_renderer* renderer, struct ntg_xy size)
{
    ntg_stage_drawing_set_size(renderer->__backbuff, size);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = ntg_rcell_default();
            it_back_buffer_rcell = ntg_stage_drawing_at_(renderer->__backbuff,
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
    struct ntg_xy old_back_buffer_size = ntg_stage_drawing_get_size(renderer->__backbuff);
    ntg_stage_drawing_set_size(renderer->__backbuff, size);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    size_t counter = 0;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_stage_drawing_at(drawing, ntg_xy(j, i)));
            it_back_buffer_rcell = ntg_stage_drawing_at_(renderer->__backbuff, ntg_xy(j, i));

            if((i < old_back_buffer_size.y) && (j < old_back_buffer_size.x))
            {
                if(ntg_rcell_are_equal(*it_back_buffer_rcell, it_drawing_rcell))
                {
                    continue;
                }
            }

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL);
            counter++;
        }
    }

    ntg_log_log("NTG_DEF_RENDERER | optimized_render | counter - %ld", counter);
}

static void full_render(ntg_def_renderer* renderer,
        const ntg_stage_drawing* drawing, struct ntg_xy size)
{
    ntg_stage_drawing_set_size(renderer->__backbuff, size);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    size_t counter = 0;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_stage_drawing_at(drawing, ntg_xy(j, i)));
            it_back_buffer_rcell = ntg_stage_drawing_at_(renderer->__backbuff,
                    ntg_xy(j, i));

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL);
            counter++;
        }
    }
    ntg_log_log("NTG_DEF_RENDERER | full_render | counter - %ld", counter);
}

