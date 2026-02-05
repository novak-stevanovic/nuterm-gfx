#ifndef NTG_SCENE_LAYER_H
#define NTG_SCENE_LAYER_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_scene_layer
{
    ntg_object* _root;
    ntg_scene* _scene;

    struct ntg_xy _min_size, _nat_size, _max_size;
    struct ntg_xy _pos;
    struct ntg_xy _size;

    int _z_index;

    void* data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_scene_layer_init(ntg_scene_layer* layer);
void ntg_scene_layer_deinit(ntg_scene_layer* layer);
void ntg_scene_layer_deinit_(void* _layer);

ntg_object* ntg_scene_layer_hit_test(
        ntg_scene_layer* layer,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos);

void ntg_scene_layer_set_root(ntg_scene_layer* layer, ntg_object* root);
void ntg_scene_layer_set_z_index(ntg_scene_layer* layer, int z_index);

struct ntg_dxy 
ntg_scene_layer_map_to_scene(const ntg_scene_layer* layer, struct ntg_dxy pos);

struct ntg_dxy 
ntg_scene_layer_map_from_scene(const ntg_scene_layer* layer, struct ntg_dxy pos);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

void _ntg_scene_layer_hmeasure(ntg_scene_layer* layer, sarena* arena);
void _ntg_scene_layer_hconstrain(ntg_scene_layer* layer, sarena* arena);
void _ntg_scene_layer_vmeasure(ntg_scene_layer* layer, sarena* arena);
void _ntg_scene_layer_vconstrain(ntg_scene_layer* layer, sarena* arena);
bool _ntg_scene_layer_fixup(ntg_scene_layer* layer, sarena* arena);
void _ntg_scene_layer_arrange(ntg_scene_layer* layer, sarena* arena);
void _ntg_scene_layer_draw(ntg_scene_layer* layer, sarena* arena);

void _ntg_scene_layer_set_hsize(ntg_scene_layer* layer, size_t size);
void _ntg_scene_layer_set_hpos(ntg_scene_layer* layer, size_t pos);
void _ntg_scene_layer_set_vsize(ntg_scene_layer* layer, size_t size);
void _ntg_scene_layer_set_vpos(ntg_scene_layer* layer, size_t pos);

/* -------------------------------------------------------------------------- */
/* REGISTER & SETTER */
/* -------------------------------------------------------------------------- */

void _ntg_scene_layer_set_scene(ntg_scene_layer* layer, ntg_scene* scene);

void _ntg_scene_layer_register(ntg_scene_layer* layer, ntg_object* object);
void _ntg_scene_layer_unregister(ntg_scene_layer* layer, ntg_object* object);

void _ntg_scene_layer_register_tree(ntg_scene_layer* layer, ntg_object* root);
void _ntg_scene_layer_unregister_tree(ntg_scene_layer* layer, ntg_object* root);

#endif // NTG_SCENE_LAYER_H
