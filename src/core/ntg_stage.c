#include "core/ntg_stage.h"
#include "base/fwd/ntg_cell_fwd.h"
#include "core/ntg_scene.h"
#include "shared/ntg_xy.h"
#include "nt.h"
#include <assert.h>

static ntg_scene_t* _active_scene = NULL;

void __ntg_stage_init__()
{
}

void __ntg_stage_deinit__()
{
}

void ntg_stage_set_scene(ntg_scene_t* scene)
{
    _active_scene = scene;
}

void ntg_stage_render()
{
    if(_active_scene == NULL) return;

    size_t _width, _height;
    nt_get_term_size(&_width, &_height);

    ntg_scene_layout(_active_scene, NTG_XY(_width, _height));

    const ntg_scene_drawing_t* drawing = ntg_scene_get_drawing(_active_scene);
    struct ntg_xy size = ntg_scene_drawing_get_size(drawing);
    size_t i, j;
    struct ntg_cell_base it_cell;
    nt_status_t _status;
    nt_buffer_enable();
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = *ntg_scene_drawing_at(drawing, NTG_XY(j, i));
            nt_write_char_at(it_cell.codepoint, it_cell.gfx, j, i, NULL, NULL);
            // nt_write_char_at(66, NT_GFX_DEFAULT, j, i, NULL, &_status);
        }
    }
    nt_buffer_disable();
}
