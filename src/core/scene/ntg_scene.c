#include <stdlib.h>
#include <assert.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"

#define DEBUG 0

#define LAYER_LAYOUT_MAX_IT 10

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

struct ntg_scene_layer_node;

GENC_VECTOR_GENERATE(ntg_scene_layer_node_vec, struct ntg_scene_layer_node*, 1.25, NULL);

struct ntg_scene_layer_node
{
    ntg_object* root;
    const struct ntg_attach_policy* policy;

    struct ntg_scene_layer_node* parent;
    struct ntg_scene_layer_node_vec children;
};

struct ntg_scene_layout_data
{
    ntg_scene* scene;
    sarena* arena;
    bool repeat;
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TREE */
/* -------------------------------------------------------------------------- */

static struct ntg_scene_layer_node* 
tree_node_new(ntg_object* root, const struct ntg_attach_policy* policy);

static void tree_node_destroy(struct ntg_scene_layer_node* node);

static struct ntg_scene_layer_node* 
tree_find(struct ntg_scene_layer_node* root, ntg_object* data);

static void tree_add(
        struct ntg_scene_layer_node** root,
        struct ntg_scene_layer_node* node,
        struct ntg_scene_layer_node* parent);

static void
tree_rm(struct ntg_scene_layer_node** root, struct ntg_scene_layer_node* node);

static size_t tree_count(struct ntg_scene_layer_node* root);
// Make sure that capacity is equal to count at least
static void tree_get_roots_preorder(
        struct ntg_scene_layer_node* node,
        struct ntg_object** out_buff,
        size_t* counter);

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

static void
tree_node_layout(ntg_scene* scene, struct ntg_scene_layer_node* node, sarena* arena, int it);

static void
tree_layout(ntg_scene* scene, struct ntg_scene_layer_node* node, sarena* arena);

static void hmeasure_fn(ntg_object* object, void* _layout_data);
static void hconstrain_fn(ntg_object* object, void* _layout_data);
static void fixup_fn(ntg_object* object, void* _layout_data);
static void vmeasure_fn(ntg_object* object, void* _layout_data);
static void vconstrain_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(hmeasure_tree, hmeasure_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(hconstrain_tree, hconstrain_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(vmeasure_tree, vmeasure_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(vconstrain_tree, vconstrain_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(fixup_tree, fixup_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(arrange_tree, arrange_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(draw_tree, draw_fn);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_scene* scene)
{
    scene->_stage = NULL;
    scene->_size = ntg_xy(0, 0);
    scene->__tree_root = NULL;
    scene->__on_key_fn = ntg_scene_dispatch_key;
    scene->__on_mouse_fn = ntg_scene_dispatch_mouse;
    scene->data = NULL;
}

void ntg_scene_init(ntg_scene* scene)
{
    assert(scene != NULL);

    init_default(scene);

    // Layer tree
    struct ntg_scene_layer_node* sentinel;
    sentinel = tree_node_new(NULL, ntg_attach_policy_root());
    assert(sentinel);

    tree_add(&scene->__tree_root, sentinel, NULL);
}

void ntg_scene_deinit(ntg_scene* scene)
{
    assert(scene);

    if(scene->_stage)
        ntg_stage_set_scene(scene->_stage, NULL);

    tree_rm(&scene->__tree_root, scene->__tree_root);

    init_default(scene);
}

void ntg_scene_deinit_(void* _scene)
{
    ntg_scene_deinit(_scene);
}

void ntg_scene_mark_dirty(ntg_scene* scene)
{
    assert(scene);

    scene->_dirty = true;

    if(scene->_stage)
    {
        ntg_stage_mark_dirty(scene->_stage);
    }
}

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    struct ntg_xy old_size = scene->_size;
    if(!ntg_xy_are_equal(old_size, size))
    {
        scene->_size = size;
        ntg_scene_mark_dirty(scene);

        //if(old_size.x != size.x)
        // {
            // uint8_t dirty = NTG_OBJECT_DIRTY_HCONSTRAIN | NTG_OBJECT_DIRTY_
            ////tree_mark_dirty(scene->__tree_root, 
        //}
    }
}

void ntg_scene_layout(ntg_scene* scene, sarena* arena)
{
    assert(scene != NULL);

    tree_layout(scene, scene->__tree_root, arena);
}

/* -------------------------------------------------------------------------- */
/* LAYER */
/* -------------------------------------------------------------------------- */

void ntg_scene_attach_root(
        ntg_scene* scene,
        ntg_object* attacher_layer,
        ntg_object* base_layer,
        const struct ntg_attach_policy* policy)
{
    assert(scene);

    if(!policy)
        policy = ntg_attach_policy_root();

    if(!attacher_layer) return;

    if(attacher_layer->_parent)
    {
        ntg_object_detach(attacher_layer);
    }
    else
    {
        ntg_scene* old_scene = ntg_object_get_scene_(attacher_layer);

        if(old_scene == scene) return;

        if(old_scene)
        {
            ntg_scene_detach_root(old_scene, attacher_layer);
        }
    }

    struct ntg_scene_layer_node* base_node = base_layer ?
            tree_find(scene->__tree_root, base_layer) :
            scene->__tree_root;

    assert(base_node);

    struct ntg_scene_layer_node* new = tree_node_new(attacher_layer, policy);
    assert(new);

    tree_add(&scene->__tree_root, new, base_node);

    _ntg_object_root_set_scene(attacher_layer, scene);
    _ntg_scene_register_tree(scene, attacher_layer);

    ntg_scene_mark_dirty(scene); // just in case
    ntg_object_mark_dirty(attacher_layer, NTG_OBJECT_DIRTY_FULL);
}

void ntg_scene_detach_root(ntg_scene* scene, ntg_object* root)
{
    assert(scene);
    assert(root);

    ntg_scene* curr_scene = ntg_object_get_scene_(root);
    assert(curr_scene == scene);

    assert(scene == curr_scene);

    struct ntg_scene_layer_node* node = tree_find(scene->__tree_root, root);
    assert(node);

    tree_rm(&scene->__tree_root, node);

    ntg_scene_mark_dirty(scene); // just in case
}

size_t ntg_scene_get_root_count(const ntg_scene* scene)
{
    assert(scene);

    // Subtract 1 because of sentinel
    return tree_count(scene->__tree_root) - 1; 
}

void ntg_scene_get_roots_by_z(
        ntg_scene* scene,
        ntg_object** out_buff,
        size_t cap)
{
    assert(scene);
    assert(out_buff);

    size_t root_count = ntg_scene_get_root_count(scene);
    assert(cap >= root_count);

    if(root_count == 0) return;

    size_t counter = 0;
    tree_get_roots_preorder(scene->__tree_root, out_buff, &counter);

    // Remove sentinel(no need)
    // memmove(out_buff, out_buff + 1, root_count * sizeof(ntg_object*));

    size_t i, j;
    ntg_object* tmp;
    for(i = 0; i < root_count - 1; i++)
    {
        for(j = i + 1; j < root_count; j++)
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
}

void _ntg_scene_register(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
    ntg_scene_mark_dirty(scene); // just in case
}

void _ntg_scene_unregister(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    ntg_scene_mark_dirty(scene);
}

void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root)
{
    assert(scene != NULL);
    assert(root != NULL);

    _ntg_scene_register(scene, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_register_tree(scene, child);
    }
}

void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root)
{
    assert(scene != NULL);
    assert(root != NULL);

    _ntg_scene_unregister(scene, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_unregister_tree(scene, child);
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TREE */
/* -------------------------------------------------------------------------- */

static struct ntg_scene_layer_node* 
tree_node_new(ntg_object* root, const struct ntg_attach_policy* policy)
{
    struct ntg_scene_layer_node* new = malloc(sizeof(struct ntg_scene_layer_node));
    if(!new) return NULL;

    new->root = root;
    new->policy = policy;
    new->parent = NULL;
    ntg_scene_layer_node_vec_init(&new->children, 2, NULL);

    return new;
}

static void tree_node_destroy(struct ntg_scene_layer_node* node)
{
    ntg_scene_layer_node_vec_deinit(&node->children, NULL);

    (*node) = (struct ntg_scene_layer_node) {0};
    free(node);
}

static struct ntg_scene_layer_node*
tree_find(struct ntg_scene_layer_node* root_node, ntg_object* data)
{
    if(root_node->root == data) return root_node;

    size_t i;
    struct ntg_scene_layer_node* it_found;
    for(i = 0; i < root_node->children.size; i++)
    {
        it_found = tree_find(root_node->children.data[i], data);
        if(it_found) return it_found;
    }

    return NULL;
}

static void tree_add(
        struct ntg_scene_layer_node** root_node,
        struct ntg_scene_layer_node* node,
        struct ntg_scene_layer_node* parent)
{
    // If sentinel, assert that root doesn't exist
    if(!parent) assert(!(*root_node));

    if(parent)
        ntg_scene_layer_node_vec_pushb(&parent->children, node, NULL);

    node->parent = parent;

    if(!parent) *root_node = node;
}

static void 
tree_rm(struct ntg_scene_layer_node** root_node, struct ntg_scene_layer_node* node)
{
    size_t i;
    for(i = 0; i < node->children.size; i++)
    {
        tree_rm(root_node, node->children.data[i]);
    }

    struct ntg_scene_layer_node* parent = node->parent;

    if(parent)
        ntg_scene_layer_node_vec_rm(&parent->children, node, NULL);
    else
        *root_node = NULL;

    if(node->root)
        _ntg_object_root_set_scene(node->root, NULL);
    tree_node_destroy(node);
}

static size_t tree_count(struct ntg_scene_layer_node* root)
{
    size_t sum = 1;

    size_t i;
    for(i = 0; i < root->children.size; i++)
    {
        sum += tree_count(root->children.data[i]);
    }

    return sum;
}

static void tree_get_roots_preorder(
        struct ntg_scene_layer_node* node,
        struct ntg_object** out_buff,
        size_t* counter)
{
    if(node->root)
    {
        out_buff[*counter] = node->root;
        (*counter)++;
    }

    size_t i;
    for(i = 0; i < node->children.size; i++)
    {
        tree_get_roots_preorder(node->children.data[i], out_buff, counter);
    }
}

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

static void
tree_node_layout(ntg_scene* scene, struct ntg_scene_layer_node* node, sarena* arena, int it)
{
    if(!node) return;

    ntg_object* root = node->root;
    const struct ntg_attach_policy* policy = node->policy;

    // Sentinel node just returns
    if(!root) return;

    ntg_object* base_layer = node->parent->root;
    struct ntg_scene_layout_data layout_data = {
        .scene = scene,
        .arena = arena,
        .repeat = false
    };

    // H MEASURE

    if(it == 0)
        hmeasure_tree(root, &layout_data);

    // H CONSTRAIN
    
    size_t hsize = 0;
    if(policy->constrain_fn)
    {
        struct ntg_attach_constrain_ctx hconstrain_ctx = {
            .base_layer = base_layer,
            .attacher_layer = root,
            .scene_size = scene->_size.x
        };
        hsize = policy->constrain_fn(policy->data,
                NTG_ORIENT_H, &hconstrain_ctx, arena);

        hsize = _clamp_size(0, hsize, scene->_size.x);
    }

    if(root->_size.x != hsize)
    {
        _ntg_object_root_set_hsize(root, hsize);
    }

    hconstrain_tree(root, &layout_data);

    // V MEASURE

    vmeasure_tree(root, &layout_data);

    // V CONSTRAIN
    
    size_t vsize = 0;
    if(policy->constrain_fn)
    {
        struct ntg_attach_constrain_ctx vconstrain_ctx = {
            .base_layer = base_layer,
            .attacher_layer = root,
            .scene_size = scene->_size.y
        };
        vsize = policy->constrain_fn(policy->data,
                NTG_ORIENT_V, &vconstrain_ctx, arena);

        vsize = _clamp_size(0, vsize, scene->_size.y);
    }

    if(root->_size.y != vsize)
    {
        _ntg_object_root_set_vsize(root, vsize);
    }

    vconstrain_tree(root, &layout_data);

    struct ntg_xy size = ntg_xy(hsize, vsize);
    
    // FIX UP

    fixup_tree(root, &layout_data);

    if(layout_data.repeat)
    {
        tree_node_layout(scene, node, arena, it + 1);
    }
    else
    {
        struct ntg_xy pos = ntg_xy(0, 0);
        if(policy->arrange_fn)
        {
            struct ntg_attach_arrange_ctx arrange_ctx = {
                .base_layer = base_layer,
                .size = size,
                .scene_size = scene->_size
            };

            pos = policy->arrange_fn(policy->data, &arrange_ctx, arena);
            pos = ntg_xy_pos_clamp(pos, size, scene->_size);

            pos.x -= _ssub_size(pos.x + size.x, scene->_size.x);
            pos.y -= _ssub_size(pos.y + size.y, scene->_size.y);
        }

        if(!ntg_xy_are_equal(root->_pos, pos))
        {
            _ntg_object_root_set_pos(root, pos);
        }

        arrange_tree(root, &layout_data);
        draw_tree(root, &layout_data);
    }
}

static void
tree_layout(ntg_scene* scene, struct ntg_scene_layer_node* root, sarena* arena)
{
    tree_node_layout(scene, root, arena, 0);

    size_t i;
    for(i = 0; i < root->children.size; i++)
    {
        tree_layout(scene, root->children.data[i], arena);
    }
}

static void hmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_HMEASURE)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M1 | %p", object);
        _ntg_object_hmeasure(object, arena);

        _ntg_object_clean(object, NTG_OBJECT_DIRTY_HMEASURE);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(HM)", object);
    }
}

static void hconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_HCONSTRAIN)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C1 | %p", object);
        _ntg_object_hconstrain(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_HCONSTRAIN);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(HC)", object);
    }
}

static void vmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_VMEASURE)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M2 | %p", object);
        _ntg_object_vmeasure(object, arena);

        _ntg_object_clean(object, NTG_OBJECT_DIRTY_VMEASURE);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(VM)", object);
    }
}

static void vconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_VCONSTRAIN)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C2 | %p", object);
        _ntg_object_vconstrain(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_VCONSTRAIN);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(VC)", object);
    }
}

static void fixup_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(_ntg_object_fixup(object, arena))
    {
        ntg_log_log("Object: %p demanded repeat", object);
        layout_data->repeat = true;
    }
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_ARRANGE)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | A | %p", object);
        _ntg_object_arrange(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_ARRANGE);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(AR)", object);
    }
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_DRAW)
    {
        if(DEBUG) ntg_log_log("NTG_DEF_SCENE | D | %p", object);
        _ntg_object_draw(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_DRAW);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(DR)", object);
    }
}
