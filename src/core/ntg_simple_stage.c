#include "core/ntg_simple_stage.h"
#include "nt.h"
#include <assert.h>
#include <stdlib.h>

#define BUFF_CAP 10000

static void __render_fn(ntg_stage* _stage, struct ntg_xy size);

void __ntg_simple_stage_init__(ntg_simple_stage* stage,
        ntg_stage_process_key_fn process_key_fn)
{
    assert(stage != NULL);
    assert(process_key_fn != NULL);

    __ntg_stage_init__((ntg_stage*)stage, process_key_fn, __render_fn);

    stage->__buff = nt_charbuff_new(BUFF_CAP);
    assert(stage->__buff != NULL);
}

void __ntg_simple_stage_deinit__(ntg_simple_stage* stage)
{
    assert(stage != NULL);

    __ntg_stage_deinit__((ntg_stage*)stage);

    nt_charbuff_destroy(stage->__buff);
}

ntg_simple_stage* ntg_simple_stage_new(ntg_stage_process_key_fn process_key_fn)
{
    assert(process_key_fn != NULL);

    ntg_simple_stage* new = (ntg_simple_stage*)malloc(sizeof(struct ntg_simple_stage));
    assert(new != NULL);

    __ntg_simple_stage_init__(new, process_key_fn);

    return new;
}

void ntg_simple_stage_destroy(ntg_simple_stage* stage)
{
    assert(stage != NULL);

    __ntg_simple_stage_deinit__(stage);

    free(stage);
}

static void __render_fn(ntg_stage* _stage, struct ntg_xy size)
{
    assert(_stage->_active_scene != NULL);

    ntg_simple_stage* stage = (ntg_simple_stage*)_stage;

    ntg_scene_layout(_stage->_active_scene, size);

    const ntg_scene_drawing* drawing = &_stage->_active_scene->_drawing;
    size_t i, j;
    struct ntg_rcell it_cell;
    nt_status _status;
    nt_buffer_enable(stage->__buff);
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = *(ntg_scene_drawing_at(drawing, ntg_xy(j, i)));
            nt_write_char_at(it_cell.codepoint, it_cell.gfx, j, i, NULL, NULL);
        }
    }
    nt_buffer_disable(NT_BUFF_FLUSH);
}
