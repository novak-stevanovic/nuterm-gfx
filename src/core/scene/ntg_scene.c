#include <stdlib.h>
#include <assert.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"

#define DEBUG 1

#define LAYER_LAYOUT_MAX_IT 10

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

struct layer_node;

GENC_VECTOR_GENERATE(layer_node_vec, struct layer_node*, 1.25, NULL);

struct layer_node_data
{
    ntg_scene_layer* layer;
    struct ntg_dock_plc policy;
};

struct layer_node
{
    struct layer_node_data data;

    struct layer_node* parent;
    struct layer_node_vec children;
};

struct ntg_scene_priv
{
    struct layer_node* layer_root;
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void dock_plc_root_fn(
        void* _data,
        struct ntg_dock_ctx ctx,
        size_t* out_size,
        size_t* out_pos,
        sarena* arena);

static void dock_plc_anchor_fn(
        void* _data,
        struct ntg_dock_ctx ctx,
        size_t* out_size,
        size_t* out_pos,
        sarena* arena);

static void
node_layer_layout(ntg_scene* scene, struct layer_node* node, sarena* arena, int it);

static void
layer_tree_layout(ntg_scene* scene, struct layer_node* node, sarena* arena);

static struct layer_node* layer_node_new(struct layer_node_data data);
static void layer_node_destroy(struct layer_node* node);

static struct layer_node*
layer_tree_find(struct layer_node* root, ntg_scene_layer* layer);

static void layer_tree_add(
        struct layer_node** root,
        struct layer_node* node,
        struct layer_node* parent);

static void layer_tree_rm(struct layer_node** root, struct layer_node* node);
static size_t layer_tree_count(struct layer_node* root);

// Make sure that capacity is equal to count at least
static void layer_tree_get_layers_preorder(
        struct layer_node* root,
        struct ntg_scene_layer** out_buff);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_scene* scene)
{
    (*scene) = (ntg_scene) {0};
}

void ntg_scene_init(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__priv = malloc(sizeof(struct ntg_scene_priv));
    assert(scene->__priv);

    init_default(scene);

    // Layer tree
    struct layer_node_data data = {
        .layer = NULL,
        .policy = ntg_dock_plc_root()
    };
    struct layer_node* sentinel = layer_node_new(data);
    assert(sentinel);

    layer_tree_add(&scene->__priv->layer_root, sentinel, NULL);

    scene->__on_key_fn = ntg_scene_dispatch_key;
    scene->__on_mouse_fn = ntg_scene_dispatch_mouse;
}

void ntg_scene_deinit(ntg_scene* scene)
{
    assert(scene);

    if(scene->_stage)
        ntg_stage_set_scene(scene->_stage, NULL);

    layer_tree_rm(&scene->__priv->layer_root, scene->__priv->layer_root);

    init_default(scene);

    if(scene->__priv) free(scene->__priv);
}

void ntg_scene_deinit_(void* _scene)
{
    ntg_scene_deinit(_scene);
}

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene);

    if(!ntg_xy_are_equal(scene->_size, size))
    {
        scene->_size = size;
        ntg_scene_mark_dirty(scene);
    }
}

void ntg_scene_mark_dirty(ntg_scene* scene)
{
    assert(scene);

    scene->_dirty = true;
}

void ntg_scene_layout(ntg_scene* scene, sarena* arena)
{
    assert(scene != NULL);

    layer_tree_layout(scene, scene->__priv->layer_root, arena);
}

/* -------------------------------------------------------------------------- */
/* LAYER */
/* -------------------------------------------------------------------------- */

void ntg_scene_dock_layer(
        ntg_scene* scene,
        ntg_scene_layer* layer,
        ntg_scene_layer* base,
        struct ntg_dock_plc policy)
{
    assert(scene);
    assert(layer);
    assert(policy.dock_fn);

    if(layer->_scene == scene) return;

    if(layer->_scene)
    {
        ntg_scene_undock_layer(layer->_scene, layer);
    }

    struct layer_node_data data = {
        .layer = layer,
        .policy = policy
    };

    struct layer_node* base_node = base ?
            layer_tree_find(scene->__priv->layer_root, base) :
            scene->__priv->layer_root;

    assert(base_node);

    struct layer_node* new = layer_node_new(data);
    assert(new);

    layer_tree_add(&scene->__priv->layer_root, new, base_node);

    ntg_scene_mark_dirty(scene);
    _ntg_scene_layer_set_scene(layer, scene);
}

void ntg_scene_undock_layer(ntg_scene* scene, ntg_scene_layer* layer)
{
    assert(scene);
    assert(layer);
    assert(scene == layer->_scene);

    struct layer_node* node = layer_tree_find(scene->__priv->layer_root, layer);
    assert(node);

    ntg_scene_mark_dirty(scene);

    layer_tree_rm(&scene->__priv->layer_root, node);

    _ntg_scene_layer_set_scene(layer, NULL);
}

size_t ntg_scene_get_layer_count(const ntg_scene* scene)
{
    assert(scene);

    // Subtract 1 because of sentinel
    return layer_tree_count(scene->__priv->layer_root) - 1; 
}

void ntg_scene_get_layers_by_z(
        ntg_scene* scene,
        ntg_scene_layer** out_buff,
        size_t cap)
{
    assert(scene);
    assert(out_buff);

    size_t layer_count = ntg_scene_get_layer_count(scene);
    assert(cap >= layer_count);

    if(layer_count == 0) return;
    
    layer_tree_get_layers_preorder(scene->__priv->layer_root, out_buff);

    // Remove sentinel
    memmove(out_buff, out_buff + 1, layer_count * sizeof(ntg_scene_layer*));

    size_t i, j;
    ntg_scene_layer* tmp;
    for(i = 0; i < layer_count - 1; i++)
    {
        for(j = i + 1; j < layer_count; j++)
        {
            if(out_buff[j]->_z_index > out_buff[i]->_z_index)
            {
                tmp = out_buff[i];
                out_buff[i] = out_buff[j];
                out_buff[j] = tmp;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* DOCK POLICY */
/* -------------------------------------------------------------------------- */

struct ntg_dock_plc ntg_dock_plc_root()
{
    return (struct ntg_dock_plc) {
        .data = NULL,
        .dock_fn = dock_plc_root_fn,
        .free_fn = NULL
    };
};

struct ntg_dock_plc
ntg_dock_plc_anchor(struct ntg_dock_plc_anchor_dt dt)
{
    assert(dt.dock);
    assert(dt.base);

    struct ntg_dock_plc_anchor_dt* dt_cpy = malloc(sizeof(*dt_cpy));
    assert(dt_cpy);

    (*dt_cpy) = dt;

    return (struct ntg_dock_plc) {
        .dock_fn = dock_plc_anchor_fn,
        .data = dt_cpy,
        .free_fn = free,
    };
}

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key)
{
    assert(scene);

    return false;
}

bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    assert(scene);

    return false;
}

void ntg_scene_set_on_key_fn(
        ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key))
{
    assert(scene);

    scene->__on_key_fn = on_key_fn;
}

bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key)
{
    assert(scene);

    if(scene->__on_key_fn)
        return scene->__on_key_fn(scene, key);
    else
        return false;
}

void ntg_scene_set_on_mouse_fn(
        ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse))
{
    assert(scene);

    assert(scene);

    scene->__on_mouse_fn = on_mouse_fn;
}

bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    assert(scene);

    if(scene->__on_mouse_fn)
        return scene->__on_mouse_fn(scene, mouse);
    else
        return false;
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_scene_clean(ntg_scene* scene)
{
    assert(scene);

    scene->_dirty = false;
}

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage)
{
    assert(scene != NULL);

    if(scene->_stage == stage) return;

    scene->_stage = stage;
    ntg_scene_mark_dirty(scene);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void dock_plc_root_fn(void* _data,
        struct ntg_dock_ctx ctx,
        size_t* out_size,
        size_t* out_pos,
        sarena* arena)
{
    (*out_size) = ctx.scene_size;
    (*out_pos) = 0;
}

static void dock_plc_anchor_fn(void* _data,
        struct ntg_dock_ctx ctx,
        size_t* out_size,
        size_t* out_pos,
        sarena* arena)
{
    struct ntg_dock_plc_anchor_dt* data = _data;
}

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);
void ntg_scene_mark_dirty(ntg_scene* scene);

static void
layer_tree_layout(ntg_scene* scene, struct layer_node* node, sarena* arena)
{
    node_layer_layout(scene, node, arena, 0);

    size_t i;
    for(i = 0; i < node->children.size; i++)
    {
        layer_tree_layout(scene, node->children.data[i], arena);
    }
}

static void
node_layer_layout(ntg_scene* scene, struct layer_node* node, sarena* arena, int it)
{
    ntg_scene_layer* layer = node->data.layer;

    // Sentinel node just returns
    if(!layer) return;

    if(!layer->_dirty)
    {
        ntg_log_log("LAYER OPTIMIZE");
        return;
    }

    ntg_scene_mark_dirty(scene);

    _ntg_scene_layer_hmeasure(layer, arena);

    struct ntg_dock_ctx hdock_ctx = {
        .orient = NTG_ORIENT_H,
        .layer_min_size = layer->_min_size.x,
        .layer_nat_size = layer->_nat_size.x,
        .layer_max_size = layer->_max_size.x,
        .scene_size = scene->_size.x
    };

    size_t _hsize = 0, _hpos = 0;
    node->data.policy.dock_fn(node->data.policy.data,
            hdock_ctx, &_hsize, &_hpos, arena);

    _hsize = _min2_size(scene->_size.x, _hsize);
    _hpos = (_hpos + _hsize <= scene->_size.x) ? _hpos : scene->_size.x - _hsize;
    _ntg_scene_layer_set_hsize(layer, _hsize);
    _ntg_scene_layer_set_hpos(layer, _hpos);

    _ntg_scene_layer_hconstrain(layer, arena);

    _ntg_scene_layer_vmeasure(layer, arena);

    struct ntg_dock_ctx vdock_ctx = {
        .orient = NTG_ORIENT_V,
        .layer_min_size = layer->_min_size.y,
        .layer_nat_size = layer->_nat_size.y,
        .layer_max_size = layer->_max_size.y,
        .scene_size = scene->_size.y
    };

    size_t _vsize = 0, _vpos = 0;
    node->data.policy.dock_fn(node->data.policy.data,
            vdock_ctx, &_vsize, &_vpos, arena);

    _vsize = _min2_size(scene->_size.x, _vsize);
    _vpos = (_vpos + _vsize <= scene->_size.x) ? _vpos : scene->_size.x - _vsize;
    _ntg_scene_layer_set_vsize(layer, _vsize);
    _ntg_scene_layer_set_vpos(layer, _vpos);

    _ntg_scene_layer_vconstrain(layer, arena);

    bool repeat = _ntg_scene_layer_post_constrain(layer, arena);
    if(repeat && (it < LAYER_LAYOUT_MAX_IT))
        node_layer_layout(scene, node, arena, it + 1);
    else
    {
        _ntg_scene_layer_arrange(layer, arena);
        _ntg_scene_layer_draw(layer, arena);
        
        _ntg_scene_layer_clean(layer);
    }
}

static struct layer_node* layer_node_new(struct layer_node_data data)
{
    struct layer_node* new = malloc(sizeof(struct layer_node));
    if(!new) return NULL;

    new->data = data;
    new->parent = NULL;
    layer_node_vec_init(&new->children, 2, NULL);

    return new;
}

static void layer_node_destroy(struct layer_node* node)
{
    layer_node_vec_deinit(&node->children, NULL);
    if(node->data.policy.free_fn)
        node->data.policy.free_fn(node->data.policy.data);

    ntg_scene_layer* layer = node->data.layer;
    if(layer)
    {
        _ntg_scene_layer_set_scene(layer, NULL);
    }

    (*node) = (struct layer_node) {0};
    free(node);
}

static struct layer_node*
layer_tree_find(struct layer_node* root, ntg_scene_layer* layer)
{
    if(root->data.layer == layer) return root;

    size_t i;
    struct layer_node* it_found;
    for(i = 0; i < root->children.size; i++)
    {
        it_found = layer_tree_find(root->children.data[i], layer);
        if(it_found) return it_found;
    }

    return NULL;
}

static void layer_tree_add(
        struct layer_node** root,
        struct layer_node* node,
        struct layer_node* parent)
{
    // If sentinel, assert that root doesn't exist
    if(!parent) assert(!(*root));

    if(parent)
        layer_node_vec_pushb(&parent->children, node, NULL);

    node->parent = parent;

    if(!parent) *root = node;
}

static void layer_tree_rm(struct layer_node** root, struct layer_node* node)
{
    size_t i;
    for(i = 0; i < node->children.size; i++)
    {
        layer_tree_rm(root, node->children.data[i]);
    }

    struct layer_node* parent = node->parent;

    if(parent)
        layer_node_vec_rm(&parent->children, node, NULL);
    else
        *root = NULL;

    layer_node_destroy(node);
}

static size_t layer_tree_count(struct layer_node* root)
{
    size_t sum = 1;

    size_t i;
    for(i = 0; i < root->children.size; i++)
    {
        sum += layer_tree_count(root->children.data[i]);
    }

    return sum;
}

static void layer_tree_get_layers_preorder(
        struct layer_node* root,
        struct ntg_scene_layer** out_buff)
{
    (*out_buff) = root->data.layer;

    size_t i;
    for(i = 0; i < root->children.size; i++)
    {
        layer_tree_get_layers_preorder(root->children.data[i], out_buff + 1);
    }
}
