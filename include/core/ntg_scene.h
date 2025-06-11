#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "object/ntg_object_fwd.h"
#include "shared/ntg_xy.h"

typedef struct ntg_scene ntg_scene_t;
typedef struct ntg_scene_drawing ntg_scene_drawing_t;

ntg_scene_t* ntg_scene_new();
void ntg_scene_destroy(ntg_scene_t* scene);

void ntg_scene_layout(ntg_scene_t* scene, struct ntg_xy size);

ntg_object_t* ntg_scene_get_root(const ntg_scene_t* scene);
void ntg_scene_set_root(ntg_scene_t* scene, ntg_object_t* new_root);

const ntg_scene_drawing_t* ntg_scene_get_drawing(const ntg_scene_t* scene);

/* -------------------------------------------------------------------------- */

const struct ntg_rcell* ntg_scene_drawing_at(const ntg_scene_drawing_t* drawing,
        struct ntg_xy pos);
struct ntg_xy ntg_scene_drawing_get_size(const ntg_scene_drawing_t* drawing);

ntg_scene_drawing_t* _ntg_scene_get_drawing(ntg_scene_t* scene);

struct ntg_rcell* _ntg_scene_drawing_at(ntg_scene_drawing_t* drawing,
        struct ntg_xy pos);

#endif // _NTG_SCENE_H_
