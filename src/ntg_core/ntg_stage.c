#include "ntg_core/ntg_stage.h"
#include "ntg_base/fwd/ntg_cell_fwd.h"
#include "ntg_core/ntg_scene.h"
#include "ntg_shared/ntg_xy.h"
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

    const ntg_scene_content_t* content = ntg_scene_get_content(_active_scene);
    struct ntg_xy size = ntg_scene_content_get_size(content);
    size_t i, j;
    struct ntg_cell_base it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = *ntg_scene_content_at(content, NTG_XY(j, i));
            nt_write_char_at(it_cell.codepoint, it_cell.gfx, j, i, NULL, NULL);
        }
    }
}
