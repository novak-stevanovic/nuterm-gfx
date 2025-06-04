#ifndef __NTG_SCENE_H__
#define __NTG_SCENE_H__

#include "core/fwd/ntg_scene_fwd.h"
#include "shared/ntg_xy.h"

ntg_scene_content_t* ntg_scene_get_content_(ntg_scene_t* scene);

struct ntg_cell_base* ntg_scene_content_at_(ntg_scene_content_t* content,
        struct ntg_xy pos);

#endif // __NTG_SCENE_H__
