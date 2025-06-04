#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "core/fwd/ntg_scene_fwd.h"

#include "object/fwd/ntg_object_fwd.h"
#include "shared/ntg_xy.h"

ntg_scene_t* ntg_scene_new();
void ntg_scene_destroy(ntg_scene_t* scene);

void ntg_scene_layout(ntg_scene_t* scene, struct ntg_xy size);

ntg_object_t* ntg_scene_get_root(const ntg_scene_t* scene);
void ntg_scene_set_root(ntg_scene_t* scene, ntg_object_t* new_root);

const ntg_scene_content_t* ntg_scene_get_content(const ntg_scene_t* scene);

/* -------------------------------------------------------------------------- */

const struct ntg_cell_base* ntg_scene_content_at(const ntg_scene_content_t* content,
        struct ntg_xy pos);

struct ntg_xy ntg_scene_content_get_size(const ntg_scene_content_t* content);

#endif // _NTG_SCENE_H_
