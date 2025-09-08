#include <assert.h>
#include "core/ntg_stage.h"
#include "base/ntg_event_types.h"
#include "core/ntg_scene.h"
#include "nt.h"
#include "nt_charbuff.h"
#include "object/ntg_object.h"
#include "shared/ntg_xy.h"
#include "base/ntg_event_types.h"

#define CHARBUFF_CAP 100000

static ntg_stage_status __process_key_fn_default(ntg_stage* stage,
        struct nt_key_event key_event);

static void __full_empty_render(ntg_stage* stage);
static void __optimized_render(ntg_stage* stage);
static void __full_render(ntg_stage* stage);

/* -------------------------------------------------------------------------- */

void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = (process_key_fn != NULL) ?
        __process_key_fn_default : NULL;
    stage->_size = NTG_XY_UNSET;
    stage->__buff = nt_charbuff_new(CHARBUFF_CAP);
    __ntg_rcell_vgrid_init__(&stage->__back_buffer);

    __ntg_listenable_init__(&stage->__listenable);
}

void __ntg_stage_deinit__(ntg_stage* stage)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = NULL;
    stage->_size = NTG_XY_UNSET;
    nt_charbuff_destroy(stage->__buff);
    stage->__buff = NULL;
    __ntg_rcell_vgrid_deinit__(&stage->__back_buffer);

    __ntg_listenable_deinit__(&stage->__listenable);
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);

    if(stage->_active_scene == scene) return;

    struct ntg_scene_change data = {
        .old = stage->_active_scene,
        .new = scene
    };

    stage->_active_scene = scene;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_SCENE_CHANGE, stage, &data);
    ntg_listenable_raise(&stage->__listenable, &e);
}

void ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size)
{
    assert(stage != NULL);

    if(ntg_xy_are_equal(stage->_size, size)) return;

    struct ntg_size_change data = {
        .old = stage->_size,
        .new = size
    };

    stage->_size = size;

    if(stage->_active_scene != NULL)
    {
        stage->_size = size;

        if(stage->_active_scene != NULL)
            ntg_scene_set_size(stage->_active_scene, size);
    }

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_RESIZE, stage, &data);
    ntg_listenable_raise(&stage->__listenable, &e);
}

void ntg_stage_render(ntg_stage* stage, ntg_stage_render_mode render_mode)
{
    assert(stage != NULL);

    nt_buffer_enable(stage->__buff);

    if(stage->_active_scene != NULL)
    {
        ntg_scene_layout(stage->_active_scene);

        if(render_mode == NTG_STAGE_RENDER_MODE_OPTIMIZED)
            __optimized_render(stage);
        else
            __full_render(stage);
    }
    else
    {
        __full_empty_render(stage);
    }

    nt_buffer_disable(NT_BUFF_FLUSH);

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_RENDER, stage, NULL);
    ntg_listenable_raise(&stage->__listenable, &e);
}

ntg_stage_status ntg_stage_feed_key_event(ntg_stage* stage,
        struct nt_key_event key_event)
{
    assert(stage != NULL);

    return stage->__process_key_fn(stage, key_event);
}

void ntg_stage_listen(ntg_stage* stage, struct ntg_event_sub sub)
{
    assert(stage != NULL);

    ntg_listenable_listen(&stage->__listenable, sub);
}

void ntg_stage_stop_listening(ntg_stage* stage, void* subscriber)
{
    assert(stage != NULL);

    ntg_listenable_stop_listening(&stage->__listenable, subscriber);
}

ntg_listenable* _ntg_stage_get_listenable(ntg_stage* stage)
{
    assert(stage != NULL);

    return &stage->__listenable;
}

/* -------------------------------------------------------------------------- */

static ntg_stage_status __process_key_fn_default(ntg_stage* stage,
        struct nt_key_event key_event)
{
    assert(stage != NULL);

    if(key_event.type == NT_KEY_EVENT_UTF32)
    {
        switch(key_event.utf32_data.codepoint)
        {
            case 'q':
                 return NTG_STAGE_QUIT;
        }
    }

    if(stage->_active_scene != NULL)
        ntg_scene_feed_key_event(stage->_active_scene, key_event);

    return NTG_STAGE_CONTINUE;
}

static void __full_empty_render(ntg_stage* stage)
{
    struct ntg_xy size = stage->_size;

    ntg_rcell_vgrid_set_size(&stage->__back_buffer, size, NULL);

    size_t i, j;
    struct ntg_rcell it_drawing_rcell, *it_back_buffer_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_drawing_rcell = ntg_rcell_default();
            it_back_buffer_rcell = ntg_rcell_vgrid_at_(&stage->__back_buffer,
                    ntg_xy(j, i));

            (*it_back_buffer_rcell) = it_drawing_rcell;
            nt_write_char_at(it_drawing_rcell.codepoint, it_drawing_rcell.gfx, j, i, NULL, NULL);
        }
    }
}

static void __optimized_render(ntg_stage* stage)
{
    struct ntg_xy size = stage->_size;

    const ntg_scene_drawing* drawing = &(stage->_active_scene->_drawing);

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

static void __full_render(ntg_stage* stage)
{
    struct ntg_xy size = stage->_size;

    const ntg_scene_drawing* drawing = &(stage->_active_scene->_drawing);

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
