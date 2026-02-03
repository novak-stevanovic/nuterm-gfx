#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

#define DEBUG 0

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

struct layout_data
{
    ntg_scene_layer* layer;
    sarena* arena;
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void init_default(ntg_scene_layer* layer);
static void hmeasure_fn(ntg_object* object, void* _layout_data);
static void hconstrain_fn(ntg_object* object, void* _layout_data);
static void vmeasure_fn(ntg_object* object, void* _layout_data);
static void vconstrain_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(hmeasure_tree, hmeasure_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(hconstrain_tree, hconstrain_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(vmeasure_tree, vmeasure_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(vconstrain_tree, vconstrain_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(arrange_tree, arrange_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(draw_tree, draw_fn);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_scene_layer_init(ntg_scene_layer* layer)
{
    assert(layer);

    init_default(layer);
}

void ntg_scene_layer_deinit(ntg_scene_layer* layer)
{
    assert(layer);

    if(layer->_root)
    {
        ntg_scene_layer_set_root(layer, NULL);
    }

    init_default(layer);
}

void ntg_scene_layer_deinit_(void* _layer)
{
    ntg_scene_layer_deinit(_layer);
}

ntg_object* ntg_scene_layer_hit_test(
        ntg_scene_layer* layer,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos)
{
    assert(layer);

    if(!layer->_root) return NULL;

    return ntg_object_hit_test(layer->_root, pos, out_local_pos);
}

void ntg_scene_layer_set_root(ntg_scene_layer* layer, ntg_object* root)
{
    assert(layer);

    if(layer->_root == root) return;
    if(root != NULL) assert(!root->_parent);

    ntg_object* old_root = layer->_root;

    if(old_root != NULL)
    {
        _ntg_object_root_set_scene_layer(old_root, NULL);
    }

    if(root != NULL)
    {
        _ntg_object_root_set_scene_layer(root, layer);
    }

    layer->_root = root;
    layer->__recompose = true;
}

void ntg_scene_layer_set_z_index(ntg_scene_layer* layer, int z_index)
{
    assert(layer);

    layer->_z_index = z_index;
}

struct ntg_dxy
ntg_scene_layer_map_to_scene(
        const ntg_scene_layer* layer,
        struct ntg_dxy pos)
{
    assert(layer);

    return ntg_dxy_add(ntg_dxy_from_xy(layer->_pos), pos);
}

struct ntg_dxy 
ntg_scene_layer_map_from_scene(
        const ntg_scene_layer* layer,
        struct ntg_dxy pos)
{
    assert(layer);
    
    return ntg_dxy_sub(pos, ntg_dxy_from_xy(layer->_pos));
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_scene_layer_hmeasure(ntg_scene_layer* layer, sarena* arena)
{
    assert(layer);

    if(!layer->_root)
    {
        layer->_min_size.x = 0;
        layer->_nat_size.x = 0;
        layer->_max_size.x = 0;
        return;
    }

    struct layout_data data = {
        .layer = layer,
        .arena = arena
    };

    hmeasure_tree(layer->_root, &data);

    layer->_min_size.x = layer->_root->_min_size.x;
    layer->_nat_size.x = layer->_root->_nat_size.x;
    layer->_max_size.x = layer->_root->_max_size.x;
}

void _ntg_scene_layer_hconstrain(ntg_scene_layer* layer, sarena* arena)
{
    assert(layer);

    if(!layer->_root) return;

    struct layout_data data = {
        .layer = layer,
        .arena = arena
    };
    
    _ntg_object_root_set_hsize(layer->_root, layer->_size.x);

    hconstrain_tree(layer->_root, &data);
}

void _ntg_scene_layer_vmeasure(ntg_scene_layer* layer, sarena* arena)
{
    assert(layer);

    if(!layer->_root)
    {
        layer->_min_size.y = 0;
        layer->_nat_size.y = 0;
        layer->_max_size.y = 0;
        return;
    }

    struct layout_data data = {
        .layer = layer,
        .arena = arena
    };

    vmeasure_tree(layer->_root, &data);

    layer->_min_size.y = layer->_root->_min_size.y;
    layer->_nat_size.y = layer->_root->_nat_size.y;
    layer->_max_size.y = layer->_root->_max_size.y;
}

void _ntg_scene_layer_vconstrain(ntg_scene_layer* layer, sarena* arena)
{
    assert(layer);

    if(!layer->_root) return;

    struct layout_data data = {
        .layer = layer,
        .arena = arena
    };

    _ntg_object_root_set_vsize(layer->_root, layer->_size.y);

    vconstrain_tree(layer->_root, &data);
}

void _ntg_scene_layer_arrange(ntg_scene_layer* layer, sarena* arena)
{
    assert(layer);

    if(!layer->_root) return;

    struct layout_data data = {
        .layer = layer,
        .arena = arena
    };

    arrange_tree(layer->_root, &data);
}

void _ntg_scene_layer_draw(ntg_scene_layer* layer, sarena* arena)
{
    assert(layer);

    if(!layer->_root) return;

    struct layout_data data = {
        .layer = layer,
        .arena = arena
    };

    draw_tree(layer->_root, &data);
}

void _ntg_scene_layer_set_hsize(ntg_scene_layer* layer, size_t size)
{
    assert(layer);

    layer->_size.x = size;
}

void _ntg_scene_layer_set_hpos(ntg_scene_layer* layer, size_t pos)
{
    assert(layer);

    layer->_pos.x = pos;
}

void _ntg_scene_layer_set_vsize(ntg_scene_layer* layer, size_t size)
{
    assert(layer);

    layer->_size.y = size;
}

void _ntg_scene_layer_set_vpos(ntg_scene_layer* layer, size_t pos)
{
    assert(layer);

    layer->_pos.y = pos;
}

void _ntg_scene_layer_set_scene(ntg_scene_layer* layer, ntg_scene* scene)
{
    assert(layer);

    layer->_scene = scene;

    if(scene) layer->__recompose = true;
}

void _ntg_scene_layer_register(ntg_scene_layer* layer, ntg_object* object)
{
    assert(layer != NULL);
    assert(object != NULL);

    object->dirty = true;
    _ntg_object_lctx_init(object);
}

void _ntg_scene_layer_unregister(ntg_scene_layer* layer, ntg_object* object)
{
    assert(layer != NULL);
    assert(object != NULL);

    _ntg_object_lctx_deinit(object);
}

void _ntg_scene_layer_register_tree(ntg_scene_layer* layer, ntg_object* root)
{
    assert(layer != NULL);
    assert(root != NULL);

    _ntg_scene_layer_register(layer, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_layer_register_tree(layer, child);
    }
}

void _ntg_scene_layer_unregister_tree(ntg_scene_layer* layer, ntg_object* root)
{
    assert(layer != NULL);
    assert(root != NULL);

    _ntg_scene_layer_unregister(layer, root);
    
    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_layer_unregister_tree(layer, child);
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void init_default(ntg_scene_layer* layer)
{
    (*layer) = (ntg_scene_layer) {0};
}

static void hmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = (struct layout_data*)_layout_data;
    sarena* arena = layout_data->arena; 

    if(object->dirty & NTG_OBJECT_DIRTY_MEASURE)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M1 | %p", object);
        _ntg_object_hmeasure(object, arena);
    }
}

static void hconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = (struct layout_data*)_layout_data;
    sarena* arena = layout_data->arena; 

    if(object->dirty & NTG_OBJECT_DIRTY_CONSTRAIN)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C1 | %p", object);
        _ntg_object_hconstrain(object, arena);
        layout_data->layer->__recompose = true;
    }
}

static void vmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = (struct layout_data*)_layout_data;
    sarena* arena = layout_data->arena; 

    if(object->dirty & NTG_OBJECT_DIRTY_MEASURE)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M2 | %p", object);
        _ntg_object_vmeasure(object, arena);

        object->dirty &= ~NTG_OBJECT_DIRTY_MEASURE;
    }
}

static void vconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = (struct layout_data*)_layout_data;
    sarena* arena = layout_data->arena; 

    if(object->dirty & NTG_OBJECT_DIRTY_CONSTRAIN)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C2 | %p", object);
        _ntg_object_vconstrain(object, arena);

        object->dirty &= ~NTG_OBJECT_DIRTY_CONSTRAIN;
        layout_data->layer->__recompose = true;
    }
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = (struct layout_data*)_layout_data;
    sarena* arena = layout_data->arena; 

    if(object->dirty & NTG_OBJECT_DIRTY_ARRANGE)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | A | %p", object);
        _ntg_object_arrange(object, arena);

        object->dirty &= ~NTG_OBJECT_DIRTY_ARRANGE;
        layout_data->layer->__recompose = true;
    }
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = (struct layout_data*)_layout_data;
    sarena* arena = layout_data->arena; 

    if(object->dirty & NTG_OBJECT_DIRTY_DRAW)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | D | %p", object);
        _ntg_object_draw(object, arena);

        object->dirty &= ~NTG_OBJECT_DIRTY_DRAW;
        layout_data->layer->__recompose = true;
    }
}
