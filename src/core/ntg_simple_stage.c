#include "core/ntg_simple_stage.h"
#include "nt.h"
#include <assert.h>
#include <stdlib.h>

#define BUFF_CAP 10000

static void __render_fn(ntg_stage* _stage, ntg_stage_render_mode render_mode);
static void __optimized_render(ntg_simple_stage* stage);
static void __full_render(ntg_simple_stage* stage);

void __ntg_simple_stage_init__(ntg_simple_stage* stage)
{
    assert(stage != NULL);

    __ntg_stage_init__((ntg_stage*)stage, __render_fn);

    stage->__buff = nt_charbuff_new(BUFF_CAP);
    __ntg_rcell_vgrid_init__(&stage->__back_buffer);

    assert(stage->__buff != NULL);
}

void __ntg_simple_stage_deinit__(ntg_simple_stage* stage)
{
    assert(stage != NULL);

    __ntg_stage_deinit__((ntg_stage*)stage);

    nt_charbuff_destroy(stage->__buff);
    __ntg_rcell_vgrid_deinit__(&stage->__back_buffer);
}

ntg_simple_stage* ntg_simple_stage_new()
{
    ntg_simple_stage* new = (ntg_simple_stage*)malloc(sizeof(struct ntg_simple_stage));
    assert(new != NULL);

    __ntg_simple_stage_init__(new);

    return new;
}

void ntg_simple_stage_destroy(ntg_simple_stage* stage)
{
    assert(stage != NULL);

    __ntg_simple_stage_deinit__(stage);

    free(stage);
}

static void __optimized_render(ntg_simple_stage* stage)
{
    ntg_stage* _stage = (ntg_stage*)stage;
    struct ntg_xy size = _stage->_size;

    const ntg_scene_drawing* drawing = &(_stage->_active_scene->_drawing);

    struct ntg_xy old_back_buffer_size = ntg_rcell_vgrid_get_size(&stage->__back_buffer);
    ntg_rcell_vgrid_set_size(&stage->__back_buffer, size, NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_scene_drawing_at(drawing, ntg_xy(j, i)));
            it_back_buffer_rcell = ntg_rcell_vgrid_at_(&stage->__back_buffer,
                    ntg_xy(j, i));

            if((i < old_back_buffer_size.y) || (j < old_back_buffer_size.x))
            {
                if(ntg_rcell_are_equal(*it_back_buffer_rcell, it_drawing_rcell))
                    continue;
            }

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL, NULL);
        }
    }
}

static void __full_render(ntg_simple_stage* stage)
{
    ntg_stage* _stage = (ntg_stage*)stage;
    struct ntg_xy size = _stage->_size;

    const ntg_scene_drawing* drawing = &(_stage->_active_scene->_drawing);

    ntg_rcell_vgrid_set_size(&stage->__back_buffer, size, NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = *(ntg_scene_drawing_at(drawing, ntg_xy(j, i)));
            it_back_buffer_rcell = ntg_rcell_vgrid_at_(&stage->__back_buffer,
                    ntg_xy(j, i));

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL, NULL);
        }
    }
}

static void __render_fn(ntg_stage* _stage, ntg_stage_render_mode render_mode)
{
    assert(_stage->_active_scene != NULL);

    ntg_simple_stage* stage = (ntg_simple_stage*)_stage;

    nt_buffer_enable(stage->__buff);

    if(render_mode == NTG_STAGE_RENDER_MODE_OPTIMIZED)
        __optimized_render(stage);
    else
        __full_render(stage);

    nt_buffer_disable(NT_BUFF_FLUSH);

}
