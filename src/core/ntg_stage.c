#include <assert.h>
#include "core/ntg_stage.h"
#include "base/ntg_cell.h"
#include "core/ntg_scene.h"
#include "shared/ntg_log.h"
#include "shared/ntg_xy.h"
#include "nt.h"
#include "core/ntg_scene_drawing.h"

static ntg_scene* _active_scene = NULL;

#define NT_BUFF_CAP 50000
static nt_charbuff* _nt_buff;

void __ntg_stage_init__()
{
    _nt_buff = nt_charbuff_new(NT_BUFF_CAP);
}

void __ntg_stage_deinit__()
{
    nt_charbuff_destroy(_nt_buff);
}

void ntg_stage_set_scene(ntg_scene* scene)
{
    _active_scene = scene;
}

void ntg_stage_render()
{
    if(_active_scene == NULL) return;

    size_t _width, _height;
    nt_get_term_size(&_width, &_height);

    ntg_scene_layout(_active_scene, ntg_xy(_width, _height));

    const ntg_scene_drawing* drawing = ntg_scene_get_drawing(_active_scene);
    struct ntg_xy size = ntg_scene_drawing_get_size(drawing);
    size_t i, j;
    struct ntg_rcell it_cell;
    nt_status _status;
    nt_buffer_enable(_nt_buff);
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = *(ntg_scene_drawing_at(drawing, ntg_xy(j, i)));
            nt_write_char_at(it_cell.codepoint, it_cell.gfx, j, i, NULL, NULL);
            // nt_write_char_at(66, NT_GFX_DEFAULT, j, i, NULL, &_status);
        }
    }
    nt_buffer_disable(NT_BUFF_FLUSH);
}
