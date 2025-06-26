#include <assert.h>
#include "core/ntg_stage.h"
#include "base/ntg_cell.h"
#include "core/ntg_scene.h"
#include "object/ntg_object.h"
#include "shared/ntg_log.h"
#include "shared/ntg_xy.h"
#include "nt.h"
#include "core/ntg_scene_drawing.h"

bool ntg_stage_process_key_fn_def(ntg_stage* stage,
        struct nt_key_event key_event)
{
    assert(stage != NULL);

    ntg_scene* scene = stage->_active_scene;

    return ntg_scene_feed_key_event(scene, key_event);
}

void ntg_stage_process_resize_fn_def(ntg_stage* stage,
        struct nt_resize_event resize_event)
{
    assert(stage != NULL);

    ntg_scene* scene = stage->_active_scene;

    // TODO
    // return ntg_scene_feed_resize_event(scene, resize_event);
}

void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn,
        ntg_stage_process_resize_fn process_resize_fn,
        ntg_stage_render_fn render_fn)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = process_key_fn;
    stage->__process_resize_fn = process_resize_fn;
    stage->__render_fn = render_fn;
}

void __ntg_stage_deinit__(ntg_stage* stage)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = NULL;
    stage->__process_resize_fn = NULL;
    stage->__render_fn = NULL;
}

ntg_scene* ntg_stage_get_scene(ntg_stage* stage)
{
    assert(stage != NULL);

    return stage->_active_scene;
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);

    stage->_active_scene = scene;
}

void ntg_stage_render(ntg_stage* stage)
{
    assert(stage != NULL);

    stage->__render_fn(stage);
}

bool ntg_stage_feed_key_event(ntg_stage* stage, struct nt_key_event key_event)
{
    assert(stage != NULL);

    return stage->__process_key_fn(stage, key_event);

}
void ntg_stage_feed_resize_event(ntg_stage* stage,
        struct nt_resize_event resize_event)
{
    assert(stage != NULL);

    stage->__process_resize_fn(stage, resize_event);
}

// void ntg_stage_render()
// {
//     if(_active_scene == NULL) return;
//
//     size_t _width, _height;
//     nt_get_term_size(&_width, &_height);
//
//     ntg_scene_layout(_active_scene, ntg_xy(_width, _height));
//
//     const ntg_scene_drawing* drawing = ntg_scene_get_drawing(_active_scene);
//     struct ntg_xy size = ntg_scene_drawing_get_size(drawing);
//     size_t i, j;
//     struct ntg_rcell it_cell;
//     nt_status _status;
//     nt_buffer_enable(_nt_buff);
//     for(i = 0; i < size.y; i++)
//     {
//         for(j = 0; j < size.x; j++)
//         {
//             it_cell = *(ntg_scene_drawing_at(drawing, ntg_xy(j, i)));
//             nt_write_char_at(it_cell.codepoint, it_cell.gfx, j, i, NULL, NULL);
//         }
//     }
//     nt_buffer_disable(NT_BUFF_FLUSH);
// }
