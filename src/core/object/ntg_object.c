#include <limits.h>
#include <stdlib.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_object_size_map
{
    ntg_object** keys;
    size_t* vals;

    size_t size;
};

struct ntg_object_pos_map
{
    ntg_object** keys;
    struct ntg_xy* vals;

    size_t size;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LAYOUT OBJECT INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_object_size_map* map,
        const ntg_object_vec* children,
        sarena* arena);

static int pos_map_init(
        ntg_object_pos_map* map,
        const ntg_object_vec* children,
        sarena* arena);

static int tmp_drawing_init(
        struct ntg_object_tmp_drawing* drawing,
        struct ntg_xy size,
        struct ntg_vcell base_bg,
        sarena* arena);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static struct ntg_object_measure incorporate_user_measure(
        struct ntg_object_measure measure,
        size_t user_min_size,
        size_t user_max_size,
        size_t user_grow);
    
static void get_dcr_size(
        ntg_object_dcr_enable enable,
        size_t we_pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t we_out_size[2],
        sarena* arena);

static bool vconstrain_border(ntg_object* object, sarena* arena);
static bool vconstrain_padding(ntg_object* object, sarena* arena);
static void calculate_border_hsize(ntg_object* object,
        sarena* arena, size_t* out_n, size_t* out_s);
static void calculate_border_vsize(ntg_object* object,
        sarena* arena, size_t* out_w, size_t* out_e);
static void calculate_padding_hsize(ntg_object* object,
        sarena* arena, size_t* out_n, size_t* out_s);
static void calculate_padding_vsize(ntg_object* object,
        sarena* arena, size_t* out_w, size_t* out_e);

static void draw_optimized(ntg_object* object, sarena* arena);
static void draw_unoptimized(ntg_object* object, sarena* arena);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_border_opts ntg_border_opts_def()
{
    return (struct ntg_border_opts) {
        .style = ntg_border_style_def(),
        .pref_size = ntg_insets(0, 0, 0, 0),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN
    };
}

bool ntg_border_opts_are_eql(
        const struct ntg_border_opts* opts1,
        const struct ntg_border_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->enable == opts2->enable) &&
            (ntg_insets_are_eql(opts1->pref_size, opts2->pref_size)) &&
            (opts1->style == opts2->style));
}

struct ntg_padding_opts ntg_padding_opts_def()
{
    return (struct ntg_padding_opts) {
        .pref_size = ntg_insets(0, 0, 0, 0),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN
    };
}

bool ntg_padding_opts_are_eql(
        const struct ntg_padding_opts* opts1,
        const struct ntg_padding_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->enable == opts2->enable) &&
    ntg_insets_are_eql(opts1->pref_size, opts2->pref_size));
}

struct ntg_layout_opts ntg_layout_opts_def()
{
    struct ntg_xy min_size, max_size, grow;
    min_size = ntg_xy(NTG_OBJECT_MIN_SIZE_UNSET, NTG_OBJECT_MIN_SIZE_UNSET);
    max_size = ntg_xy(NTG_OBJECT_MAX_SIZE_UNSET, NTG_OBJECT_MAX_SIZE_UNSET);
    grow = ntg_xy(NTG_OBJECT_GROW_UNSET, NTG_OBJECT_GROW_UNSET);
    return (struct ntg_layout_opts) {
        .min_cont_size = min_size,
        .max_cont_size = max_size,
        .grow = grow,
        .z_index = NTG_OBJECT_Z_INDEX_UNSET
    };
}

bool ntg_layout_opts_are_eql(
        const struct ntg_layout_opts* opts1,
        const struct ntg_layout_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return (ntg_xy_are_eql(opts1->min_cont_size, opts2->min_cont_size) &&
            ntg_xy_are_eql(opts1->max_cont_size, opts2->max_cont_size) &&
            ntg_xy_are_eql(opts1->grow, opts2->grow) &&
            opts1->z_index == opts2->z_index);
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_object_vdeinit(ntg_object* object)
{
    if(!object) return;

    if(object->__vtable->deinit_fn)
        object->__vtable->deinit_fn(object);
}

/* ------------------------------------------------------ */
/* OBJECT TREE */
/* ------------------------------------------------------ */

static void count_fn(ntg_object* object, void* _counter)
{
    if(!object) return;

    size_t* counter = _counter;
    (*counter)++;
}

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(count_tree, count_fn);

size_t ntg_object_get_tree_size(const ntg_object* root)
{
    if(!root) return 0;

    size_t counter = 0;
    count_tree((ntg_object*)root, &counter);

    return counter;
}

const ntg_object* ntg_object_get_root(const ntg_object* object)
{
    if(!object) return NULL;

    const ntg_object *it_obj = object, *it_root, *it_base;
    while(true)
    {
        it_root = ntg_object_get_layer_root(it_obj);
        it_base = it_root->_base;
        it_obj = it_base;

        if(!it_base) break;
    }

    return it_root;
}

ntg_object* ntg_object_get_root_(ntg_object* object)
{
    if(!object) return NULL;

    return (ntg_object*)ntg_object_get_root(object);
}

const ntg_object* ntg_object_get_layer_root(const ntg_object* object)
{
    if(!object) return NULL;

    while(object->_parent) object = object->_parent;
    return object;
}

ntg_object* ntg_object_get_layer_root_(ntg_object* object)
{
    if(!object) return NULL;

    return (ntg_object*)ntg_object_get_layer_root(object);
}

ntg_scene* ntg_object_get_scene_(ntg_object* object)
{
    if(!object) return NULL;

    ntg_object* root = ntg_object_get_root_(object);

    return root->__scene;
}

const ntg_scene* ntg_object_get_scene(const ntg_object* object)
{
    if(!object) return NULL;

    const ntg_object* root = ntg_object_get_root(object);

    return root->__scene;
}

bool ntg_object_is_true_root(const ntg_object* object)
{
    if(!object) return false;

    return ((!object->_parent) && (!object->_base));
}

bool ntg_object_is_root(const ntg_object* object)
{
    if(!object) return false;

    return (!object->_parent);
}

bool ntg_object_is_only_layer_root(const ntg_object* object)
{
    if(!object) return false;

    return (!ntg_object_is_true_root(object) && (ntg_object_is_root(object)));
}

bool ntg_object_is_focused(const ntg_object* object)
{
    if(!object) return false;

    const ntg_scene* scene = ntg_object_get_scene(object);
    if(!scene) return false;

    return (scene->_fm->_focused == object);
}

bool ntg_object_is_descendant(
        const ntg_object* object,
        const ntg_object* descendant)
{
    if(!object || !descendant) return false;

    const ntg_object* it = descendant;
    while(it)
    {
        if(it == object) return true;
        it = it->_parent;
    }

    return false;
}

bool ntg_object_is_descendant_eq(
        const ntg_object* object,
        const ntg_object* descendant)
{
    if(!object || !descendant) return false;

    return ((object == descendant) ||
    ntg_object_is_descendant(object, descendant));
}

size_t ntg_object_get_children_by_z(
        const ntg_object* object,
        ntg_object** out_buff,
        size_t cap)
{
    if(!object) return 0;

    const ntg_object_vec* children = &object->_children;
    if(children->size == 0) return 0;

    if(out_buff)
    {
        if(cap < children->size) return 0;

        size_t i, j;

        for(i = 0; i < children->size; i++)
            out_buff[i] = children->data[i];

        ntg_object* tmp_obj;
        for(i = 0; i < children->size - 1; i++)
        {
            for(j = i + 1; j < children->size; j++)
            {
                if((out_buff[j])->_layout_opts.z_index < (out_buff[i])->_layout_opts.z_index)
                {
                    tmp_obj = out_buff[i];
                    out_buff[i] = out_buff[j];
                    out_buff[j] = tmp_obj;
                }
            }
        }
    }

    return children->size;
}

ntg_object* ntg_object_hit_test(
        ntg_object* object,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos,
        ntg_object_hit_result* out_hit)
{
    if(!object) return NULL;

    if(!ntg_xy_is_in_rectagle(pos, ntg_xy(0, 0), object->_size))
    {
        if(out_local_pos) *out_local_pos = ntg_xy(0, 0);
        return NULL;
    }

    int curr_z = INT_MIN;
    ntg_object* best_obj = object;
    if(out_local_pos) *out_local_pos = pos;

    size_t i;
    struct ntg_xy it_child_local;
    ntg_object_hit_result _hit;
    struct ntg_dxy it_adj_pos;
    ntg_object* it_child;
    ntg_object* it_hit;
    for(i = 0; i < object->_children.size; i++)
    {
        it_child = object->_children.data[i];
        it_adj_pos = ntg_dxy_sub(ntg_dxy_from_xy(pos), ntg_dxy_from_xy(it_child->_pos));

        if(ntg_dxy_is_in_rectagle(it_adj_pos, ntg_dxy(0, 0), ntg_dxy_from_xy(it_child->_size)))
        {
            it_hit = ntg_object_hit_test(it_child, ntg_xy_from_dxy(it_adj_pos), &it_child_local, &_hit);

            if(it_hit && (it_child->_layout_opts.z_index > curr_z))
            {
                best_obj = it_hit;
                curr_z = it_child->_layout_opts.z_index;
                
                if(out_local_pos) (*out_local_pos) = it_child_local;

                if(out_hit)
                {
                    struct ntg_insets padding_size = object->_padding.size;
                    struct ntg_xy cont_size = ntg_object_get_size_cont(object);
                    // struct ntg_xy size = ntg_object_get_size(object);

                    // if((pos.x >= size.x) || (pos.y >= size.y))
                        // return NULL;

                    if((pos.x > (padding_size.w + cont_size.x)) ||
                        (pos.y > (padding_size.n + cont_size.y)))
                    {
                        (*out_hit) = NTG_OBJECT_HIT_BORD;
                    }
                    else if((pos.x > cont_size.x) || (pos.y > cont_size.y))
                    {
                        (*out_hit) = NTG_OBJECT_HIT_PAD;
                    }
                    else
                    {
                        (*out_hit) = NTG_OBJECT_HIT_CONT;
                    }
                }
            }
        }
    }
    return best_obj;
}

void ntg_object_detach(ntg_object* object)
{
    if(!object) return;

    ntg_object* parent = object->_parent;
    if(parent == NULL) return;

    ntg_scene* scene = ntg_object_get_scene_(object);

    ntg_object_vec_rm(&parent->_children, object, NULL);

    object->_parent = NULL;

    if(parent->__vtable->rm_child_fn)
        parent->__vtable->rm_child_fn(parent, object);

    if(scene)
        _ntg_scene_rm_object_tree(scene, object);

    ntg_object_mark_dirty(parent, NTG_OBJECT_DIRTY_FULL);

    if(object->hooks.on_child_rm_fn)
        object->hooks.on_child_rm_fn(parent, object);

    if(object->hooks.on_parent_rm_fn)
        object->hooks.on_parent_rm_fn(object, parent);

    if(scene)
        _ntg_scene_unregister_tree(scene, object);
}

void ntg_object_anchor(
        ntg_object* base,
        ntg_object* root,
        const struct ntg_anchor_policy* policy,
        int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    if(!base || !root || !policy || (base == root))
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(base->_anchored.size >= NTG_OBJECT_MAX_ANCHORED)
        ntg_vreturn(out_status, NTG_ERR_MAX_ANCHORED);

    if(root->_parent)
    {
        ntg_object_detach(root);
    }

    if(root->_base)
    {
        ntg_object_unanchor(root);
    }

    ntg_object_vec_pushb(&base->_anchored, root, &_status);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

    root->_base = base;
    root->_anchor_policy = policy;

    ntg_scene* scene = ntg_object_get_scene_(root);

    if(scene)
        _ntg_scene_add_object_tree(scene, root);

    if(base->hooks.on_anchored_add_fn)
        base->hooks.on_anchored_add_fn(base, root);

    if(root->hooks.on_base_set_fn)
        root->hooks.on_base_set_fn(root, base);

    if(scene)
        _ntg_scene_register_tree(scene, root);
}

void ntg_object_unanchor(ntg_object* root)
{
    if(!root || !root->_base)
        return;

    ntg_object* base = root->_base;

    ntg_scene* scene = ntg_object_get_scene_(root);

    ntg_object_vec_rm(&root->_base->_anchored, root, NULL);
    root->_base = NULL;
    root->_anchor_policy = NULL;

    if(base->hooks.on_anchored_rm_fn)
        base->hooks.on_anchored_rm_fn(base, root);

    if(scene)
        _ntg_scene_rm_object_tree(scene, root);

    if(root->hooks.on_base_rm_fn)
        root->hooks.on_base_rm_fn(root, base);

    if(scene)
        _ntg_scene_unregister_tree(scene, root);
}

void ntg_object_remove_from_scene(ntg_object* object)
{
    if(!object) return;

    ntg_object* parent = object->_parent;
    if(parent)
    {
        ntg_object_detach(object);
        return;
    }

    ntg_object* base = object->_base;
    if(base)
    {
        ntg_object_unanchor(object);
        return;
    }

    if(ntg_object_is_true_root(object) && ntg_object_get_scene(object))
    {
        ntg_scene_set_root(ntg_object_get_scene_(object), NULL, NULL);
        return;
    }
}

/* ------------------------------------------------------ */
/* CONTROL */
/* ------------------------------------------------------ */

void ntg_object_set_layout_opts(
        ntg_object* object,
        const struct ntg_layout_opts* opts)
{
    if(!object) return;

    struct ntg_layout_opts old_opts = object->_layout_opts;
    struct ntg_layout_opts new_opts = (opts ? (*opts) : ntg_layout_opts_def());

    if(ntg_layout_opts_are_eql(&old_opts, &new_opts))
        return;

    object->_layout_opts = new_opts;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->hooks.on_layout_opts_chng_fn)
        object->hooks.on_layout_opts_chng_fn(object, &old_opts, &object->_layout_opts);
}

void ntg_object_set_border_opts(
        ntg_object* object,
        const struct ntg_border_opts* opts)
{
    if(!object) return;

    struct ntg_border_opts old_opts = object->_border.opts;
    struct ntg_border_opts new_opts = (opts ? (*opts) : ntg_border_opts_def());

    if(ntg_border_opts_are_eql(&old_opts, &new_opts))
        return;

    object->_border.opts = new_opts;
    if(!object->_border.opts.style)
        object->_border.opts.style = ntg_border_style_def();

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->hooks.on_border_opts_chng_fn)
        object->hooks.on_border_opts_chng_fn(object, &old_opts, &object->_border.opts);
}

void ntg_object_set_padding_opts(
        ntg_object* object,
        const struct ntg_padding_opts* opts)
{
    if(!object) return;

    struct ntg_padding_opts old_opts = object->_padding.opts;
    struct ntg_padding_opts new_opts = (opts ? (*opts) : ntg_padding_opts_def());

    if(ntg_padding_opts_are_eql(&old_opts, &new_opts))
        return;

    object->_padding.opts = new_opts;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->hooks.on_padding_opts_chng_fn)
        object->hooks.on_padding_opts_chng_fn(object, &old_opts, &object->_padding.opts);
}

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

struct ntg_dxy ntg_object_map_to_ancestor(
        const ntg_object* object,
        const ntg_object* ancestor,
        struct ntg_dxy point)
{
    if(!object)
        return NTG_DXY_MAX;

    if(object == ancestor)
        return point;

    struct ntg_dxy out = point;

    const ntg_object* it = object;
    while(it != NULL && it != ancestor)
    {
        out = ntg_dxy_add(out, ntg_dxy_from_xy(it->_pos));
        it = it->_parent;
    }

    return out;
}

struct ntg_dxy ntg_object_map_to_descendant(
        const ntg_object* object,
        const ntg_object* descendant,
        struct ntg_dxy point)
{
    if(!descendant)
        return NTG_DXY_MAX;

    if(object == descendant)
        return point;

    struct ntg_dxy desc_pos = ntg_object_map_to_ancestor(
            descendant, object, ntg_dxy(0, 0));

    return ntg_dxy_sub(point, desc_pos);
}

struct ntg_dxy 
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point)
{
    if(!object)
        return NTG_DXY_MAX;

    return ntg_object_map_to_ancestor(object, NULL, point);
}

struct ntg_dxy 
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point)
{
    if(!object)
        return NTG_DXY_MAX;

    return ntg_object_map_to_descendant(NULL, object, point);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_object_feed_key(ntg_object* object, const struct ntg_object_key* event)
{
    if(!object) return false;
    if(!event) return false;
    if(!event->target) return false;

    bool consumed = false;

    if(object->__vtable->process_key_fn)
    {
        consumed = object->__vtable->process_key_fn(object, event);
    }

    if(object->hooks.on_key_fn)
    {
        object->hooks.on_key_fn(object, event);
    }

    return consumed;
}

bool ntg_object_feed_mouse(ntg_object* object, const struct ntg_object_mouse* event)
{
    if(!object) return false;
    if(!object->_clickable) return false;
    if(!event) return false;
    if(!event->target) return false;

    struct ntg_xy size = ntg_object_get_size(object);
    if((event->mouse.x >= size.x) || (event->mouse.y >= size.y))
        return false;

    struct ntg_xy pos = ntg_xy(event->mouse.x, event->mouse.y);
    ntg_object_hit_result _hit_result;
    ntg_object_hit_test(object, pos, NULL, &_hit_result);

    if(!(object->_clickable == NTG_OBJECT_CLICKABLE_BORDER) &&
        (_hit_result == NTG_OBJECT_HIT_BORD))
    {
        return false;
    }

    bool consumed = false;

    if(object->__vtable->process_mouse_fn)
    {
        consumed = object->__vtable->process_mouse_fn(object, event);
    }

    if(object->hooks.on_mouse_fn)
    {
        object->hooks.on_mouse_fn(object, event);
    }

    return consumed;
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_object* object)
{
    (*object) = (ntg_object) {0};

    object->_layout_opts = ntg_layout_opts_def();

    object->_border.opts = ntg_border_opts_def();
    object->_padding.opts = ntg_padding_opts_def();
    object->_anchor_policy = ntg_anchor_policy_root();

    object->__base_bg = ntg_vcell_def();

    object->_clickable = NTG_OBJECT_UNCLICKABLE;
    object->_focusable = NTG_OBJECT_UNFOCUSABLE;
}

void ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!object)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_OBJECT))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    if(!vtable ||!vtable->deinit_fn)
        ntg_vreturn(out_status, NTG_ERR_BAD_VTABLE);

    init_default(object);

    int _status;

    ntg_object_vec_init(&object->_children, 2, &_status);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }
    ntg_object_vec_init(&object->_anchored, 2, &_status);
    if(_status != 0)
    {
        ntg_object_vec_deinit(&object->_children, NULL);
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

    object->_type = type;

    object->__vtable = vtable;

    ntg_object_drawing_init(&object->_drawing);
}

void ntg_object_deinit(ntg_object* object)
{
    if(!object) return;

    ntg_scene* scene = ntg_object_get_scene_(object);
    
    if(ntg_object_is_true_root(object) && scene)
    {
        ntg_scene_set_root(scene, NULL, NULL);
    }

    if(object->_parent)
    {
        ntg_object_detach(object);
    }

    if(object->_base)
    {
        ntg_object_unanchor(object);
    }

    while(object->_children.size > 0)
    {
        ntg_object_detach(object->_children.data[0]);
    }

    ntg_object_vec_deinit(&object->_children, NULL);
    ntg_object_vec_deinit(&object->_anchored, NULL);
    ntg_object_drawing_deinit(&object->_drawing);

    init_default(object);
}

void ntg_object_attach(ntg_object* parent, ntg_object* child, int* out_status)
{
    ntg_init_status(out_status);

    int _status;

    if(!parent || !child || (child == parent))
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(parent->_children.size >= NTG_OBJECT_MAX_CHILDREN)
        ntg_vreturn(out_status, NTG_ERR_MAX_CHILDREN);

    if(child->_parent != NULL)
        ntg_object_detach(child);

    if(child->__scene) 
        ntg_scene_set_root(child->__scene, NULL, NULL);

    if(child->_base)
        ntg_object_unanchor(child);

    ntg_scene* scene = ntg_object_get_scene_(parent);

    ntg_object_vec_pushb(&parent->_children, child, &_status);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

    child->_parent = parent;

    if(scene)
        _ntg_scene_add_object_tree(scene, child);

    if(parent->hooks.on_child_add_fn)
        parent->hooks.on_child_add_fn(parent, child);

    if(child->hooks.on_parent_set_fn)
        child->hooks.on_parent_set_fn(child, parent);

    if(scene)
        _ntg_scene_register_tree(scene, child);
}

void ntg_object_set_base_bg(ntg_object* object, struct ntg_vcell base_bg)
{
    if(!object) return;

    object->__base_bg = base_bg;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW | NTG_OBJECT_DIRTY_RENDER);
}

void ntg_object_set_focusable(ntg_object* object, ntg_object_focusable_mode mode)
{
    if(!object) return;

    if(object->_focusable == NTG_OBJECT_FOCUSABLE)
    {
        const ntg_scene* scene = ntg_object_get_scene(object);
        if(scene)
        {
            ntg_focus_manager* fm = scene->_fm;
            if(fm && (fm->_focused == object))
                ntg_focus_manager_request_focus(fm, NULL);
        }
    }

    object->_focusable = mode;
}

void ntg_object_set_clickable(ntg_object* object, ntg_object_clickable_mode mode)
{
    if(!object) return;

    object->_clickable = mode;
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_object_root_set_scene(ntg_object* object, ntg_scene* scene)
{
    if(!object || object->_parent || object->_base)
        return;

    object->__scene = scene;
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* MEASURE PHASE */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* CONSTRAIN PHASE */
/* ------------------------------------------------------ */

size_t ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object)
{
    if(!map || !object)
        return NTG_SIZE_MAX;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
            return map->vals[i];
    }

    return NTG_SIZE_MAX;
}

void ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size)
{
    if(!map || !object) return;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
        {
            map->vals[i] = size;
            break;
        }
    }
}

/* ------------------------------------------------------ */
/* ARRANGE PHASE */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_pos_map_get(
        const ntg_object_pos_map* map,
        const ntg_object* object)
{
    if(!map || !object)
        return NTG_XY_MAX;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
            return map->vals[i];
    }

    return NTG_XY_MAX;
}

void ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos)
{
    if(!map || !object) return;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
        {
            map->vals[i] = pos;
            break;
        }
    }
}

/* ------------------------------------------------------ */
/* DRAW PHASE */
/* ------------------------------------------------------ */

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_object_mark_dirty(ntg_object* object, uint8_t dirty)
{
    if(!object) return;

    object->_dirty |= dirty;

    ntg_scene* scene = ntg_object_get_scene_(object);
    if(scene)
        ntg_scene_mark_dirty(scene);
}

/* ------------------------------------------------------ */
/* MEASURE & SIZE HELPERS */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_min_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->_min_size;
}

struct ntg_xy ntg_object_get_nat_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->_nat_size;
}

struct ntg_xy ntg_object_get_max_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->_max_size;
}

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->_size;
}

struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    return (struct ntg_object_measure) {
        .min_size = ntg_xy_get(object->_min_size, orient),
        .nat_size = ntg_xy_get(object->_nat_size, orient),
        .max_size = ntg_xy_get(object->_max_size, orient),
        .grow = ntg_xy_get(object->_grow, orient)
    };
}

size_t ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient)
{
    if(!object)
        return 0;
        
    return ntg_xy_get(object->_size, orient);
}

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->_min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
        ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->_nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->_max_size, sub);
}

struct ntg_xy ntg_object_get_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets border_size = object->_border.size;
    struct ntg_insets padding_size = object->_padding.size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(border_size) + ntg_insets_hsum(padding_size),
        ntg_insets_vsum(border_size) + ntg_insets_vsum(padding_size)
    );
    return ntg_xy_sub(object->_size, sub);
}

struct ntg_object_measure
ntg_object_get_measure_cont(const ntg_object* object, ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    struct ntg_object_measure m = ntg_object_get_measure(object, orient);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding.opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient) +
            ntg_insets_sum(pref_padding_size, orient);

    m.min_size = _ssub_size(m.min_size, sub);
    m.nat_size = _ssub_size(m.nat_size, sub);
    m.max_size = _ssub_size(m.max_size, sub);

    return m;
}

size_t ntg_object_get_size_1d_cont(const ntg_object* object, ntg_orient orient)
{
    if(!object) return 0;

    size_t s = ntg_xy_get(object->_size, orient);

    struct ntg_insets border_size = object->_border.size;
    struct ntg_insets padding_size = object->_padding.size;

    size_t sub = ntg_insets_sum(border_size, orient) +
            ntg_insets_sum(padding_size, orient);

    return _ssub_size(s, sub);
}

size_t ntg_object_get_for_size_cont(const ntg_object* object, ntg_orient orient)
{
    if(!object) return 0;

    return (orient == NTG_ORIENT_H) ?
            NTG_SIZE_MAX :
            ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);
}

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size),
        ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_max_size, sub);
}

struct ntg_xy ntg_object_get_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets border_size = object->_border.size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(border_size),
            ntg_insets_vsum(border_size)
    );
    return ntg_xy_sub(object->_size, sub);
}

struct ntg_object_measure
ntg_object_get_measure_pad(const ntg_object* object, ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    struct ntg_object_measure m = ntg_object_get_measure(object, orient);

    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient);

    m.min_size = _ssub_size(m.min_size, sub);
    m.nat_size = _ssub_size(m.nat_size, sub);
    m.max_size = _ssub_size(m.max_size, sub);

    return m;
}

size_t ntg_object_get_size_1d_pad(const ntg_object* object, ntg_orient orient)
{
    if(!object) return 0;

    size_t s = ntg_xy_get(object->_size, orient);

    struct ntg_insets border_size = object->_border.size;

    size_t sub = ntg_insets_sum(border_size, orient);

    return _ssub_size(s, sub);
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_object_hmeasure(ntg_object* object, sarena* arena, int* out_status)
{
    ntg_init_status(out_status);

    if(!object || !arena)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    struct ntg_object_measure measure = {0};
    if(object->__vtable->measure_fn)
    {
        measure = object->__vtable->measure_fn(object,
                NTG_ORIENT_H, object->layout_cache, arena);

        size_t extra = ntg_insets_hsum(object->_padding.opts.pref_size) +
                ntg_insets_hsum(object->_border.opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->_layout_opts.min_cont_size.x + extra,
                object->_layout_opts.max_cont_size.x + extra,
                object->_layout_opts.grow.x);

        if(measure.min_size == extra)
            measure.min_size = 0;

        if(measure.nat_size == extra)
            measure.nat_size = 0;

        if(measure.max_size == extra)
            measure.max_size = 0;
    }

    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_H);

    if(!ntg_object_measure_are_eql(measure, old))
    {
        object->_min_size.x = measure.min_size;
        object->_nat_size.x = measure.nat_size;
        object->_max_size.x = measure.max_size;
        object->_grow.x = measure.grow;
        ntg_object_mark_dirty(object,
                NTG_OBJECT_DIRTY_HCONSTRAIN |
                NTG_OBJECT_DIRTY_VMEASURE);
        if(object->_parent)
        {
            ntg_object_mark_dirty(object->_parent,
                    NTG_OBJECT_DIRTY_MEASURE |
                    NTG_OBJECT_DIRTY_CONSTRAIN);
        }
    }
}

void _ntg_object_hconstrain(ntg_object* object, sarena* arena, int* out_status)
{
    ntg_init_status(out_status);

    if(!object || !arena)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    size_t cont_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);
    size_t size = ntg_object_get_size_1d(object, NTG_ORIENT_H);
    object->__old_cont_size.x = cont_size;
    object->__old_size.x = size;

    if(object->__skip_hborder)
    {
        object->_border.size.w = 0;
        object->_border.size.e = 0;
    }
    else
    {
        size_t w, e;
        calculate_border_hsize(object, arena, &w, &e);

        object->_border.size.w = w;
        object->_border.size.e = e;
    }
    if(object->__skip_hpadding)
    {
        object->_padding.size.w = 0;
        object->_padding.size.e = 0;
    }
    else
    {
        size_t w, e;
        calculate_padding_hsize(object, arena, &w, &e);

        object->_padding.size.w = w;
        object->_padding.size.e = e;
    }
    
    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN |
            NTG_OBJECT_DIRTY_ARRANGE | NTG_OBJECT_DIRTY_DRAW);

    if(object->_children.size == 0) return;

    int _status;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);

    ntg_object* it_child;
    size_t it_old_size;
    if(content_size == 0)
    {
        for(i = 0; i < object->_children.size; i++)
        {
            it_child = object->_children.data[i];
            it_old_size = it_child->_size.x;

            if(it_old_size != 0)
            {
                it_child->_size.x = 0;
                ntg_object_mark_dirty(it_child, NTG_OBJECT_DIRTY_HCONSTRAIN);
            }
        }

        return;
    }
    ntg_object_size_map map;
    _status = size_map_init(&map, &object->_children, arena);
    if(_status) return;

    if(object->__vtable->constrain_fn)
    {
        object->__vtable->constrain_fn(object, NTG_ORIENT_H,
                &map, object->layout_cache, arena);
    }

    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = map.vals[i];
        it_old_size = it_child->_size.x;

        if(it_old_size != it_size)
        {
            ntg_object_mark_dirty(it_child,
                    NTG_OBJECT_DIRTY_HCONSTRAIN |
                    NTG_OBJECT_DIRTY_VMEASURE |
                    NTG_OBJECT_DIRTY_VCONSTRAIN);

            it_size = _min2_size(content_size, it_size);

            it_child->_size.x = it_size;
            it_child->__skip_hborder = false;
            it_child->__skip_hpadding = false;
        }
    }
}

void _ntg_object_vmeasure(ntg_object* object, sarena* arena, int* out_status)
{
    ntg_init_status(out_status);

    if(!object || !arena)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    struct ntg_object_measure measure = {0};
    if(object->__vtable->measure_fn)
    {
        measure = object->__vtable->measure_fn(object,
                NTG_ORIENT_V, object->layout_cache, arena);

        size_t extra = ntg_insets_vsum(object->_padding.opts.pref_size) +
                ntg_insets_vsum(object->_border.opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->_layout_opts.min_cont_size.y + extra,
                object->_layout_opts.max_cont_size.y + extra,
                object->_layout_opts.grow.y);

        if(measure.min_size == extra)
            measure.min_size = 0;

        if(measure.nat_size == extra)
            measure.nat_size = 0;

        if(measure.max_size == extra)
            measure.max_size = 0;
    }

    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_V);

    if(!ntg_object_measure_are_eql(measure, old))
    {
        object->_min_size.y = measure.min_size;
        object->_nat_size.y = measure.nat_size;
        object->_max_size.y = measure.max_size;
        object->_grow.y = measure.grow;

        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN);
        if(object->_parent)
        {
            ntg_object_mark_dirty(object,
                    NTG_OBJECT_DIRTY_VMEASURE |
                    NTG_OBJECT_DIRTY_VCONSTRAIN);
        }
    }
}

void _ntg_object_vconstrain(ntg_object* object, sarena* arena, int* out_status)
{
    ntg_init_status(out_status);

    if(!object || !arena)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    size_t cont_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);
    size_t size = ntg_object_get_size_1d(object, NTG_ORIENT_V);
    object->__old_cont_size.y = cont_size;
    object->__old_size.y = size;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_ARRANGE | NTG_OBJECT_DIRTY_DRAW);
    bool ret = vconstrain_border(object, arena) || vconstrain_padding(object, arena);
    if(ret) return;

    if(object->_children.size == 0) return;

    int _status;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);

    ntg_object* it_child;
    size_t it_old_size;
    if(content_size == 0)
    {
        for(i = 0; i < object->_children.size; i++)
        {
            it_child = object->_children.data[i];
            it_old_size = it_child->_size.y;

            if(it_old_size != 0)
            {
                it_child->_size.y = 0;
                ntg_object_mark_dirty(it_child, NTG_OBJECT_DIRTY_VCONSTRAIN);
            }
        }

        return;
    }

    ntg_object_size_map map;
    _status = size_map_init(&map, &object->_children, arena);
    if(_status) return;

    if(object->__vtable->constrain_fn)
    {
        object->__vtable->constrain_fn(object, NTG_ORIENT_V,
                &map, object->layout_cache, arena);
    }

    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = map.vals[i];
        it_old_size = it_child->_size.y;

        it_size = _min2_size(content_size, it_size);

        if(it_old_size != it_size)
        {
            ntg_object_mark_dirty(it_child, NTG_OBJECT_DIRTY_VCONSTRAIN);

            it_child->_size.y = it_size;
        }
    }
}

bool _ntg_object_fixup(ntg_object* object, sarena* arena)
{
    if(!object || !arena)
        return false;

    bool repeat_dcr = object->__repeat;
    bool repeat_fn = false;
    if(object->__vtable->fixup_fn)
    {
        repeat_fn = object->__vtable->fixup_fn(object,
                object->layout_cache, arena);;
    }

    if(repeat_dcr || repeat_fn)
    {
        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_HCONSTRAIN);
    }

    object->__repeat = false;

    struct ntg_xy cont_size = ntg_object_get_size_cont(object);
    struct ntg_xy old_cont_size = object->__old_cont_size;
    if(!ntg_xy_are_eql(cont_size, old_cont_size))
    {
        if(object->__vtable->cont_resize_fn)
            object->__vtable->cont_resize_fn(object, old_cont_size, cont_size);

        if(object->hooks.on_cont_resize_fn)
            object->hooks.on_cont_resize_fn(object, old_cont_size, cont_size);
    }
    struct ntg_xy size = ntg_object_get_size(object);
    struct ntg_xy old_size = object->__old_size;
    if(!ntg_xy_are_eql(size, old_size))
    {
        if(object->__vtable->resize_fn)
            object->__vtable->resize_fn(object, old_size, size);

        if(object->hooks.on_resize_fn)
            object->hooks.on_resize_fn(object, old_size, size);
    }

    return (repeat_dcr || repeat_fn);
}

void _ntg_object_arrange(ntg_object* object, sarena* arena, int* out_status)
{
    ntg_init_status(out_status);

    if(!object || !arena)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(object->_children.size == 0) return;

    int _status;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);

    if(content_size == 0)
    {
        for(i = 0; i < object->_children.size; i++)
        {
            object->_children.data[i]->_pos = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);
        }
        return;
    }

    ntg_object_pos_map map;
    _status = pos_map_init(&map, &object->_children, arena);
    if(_status) return;

    if(object->__vtable->arrange_fn)
        object->__vtable->arrange_fn(object, &map, object->layout_cache, arena);

    struct ntg_xy dcr_sum = ntg_xy(
            object->_border.size.w + object->_padding.size.w,
            object->_border.size.n + object->_padding.size.n);

    ntg_object* it_child;
    struct ntg_xy it_pos;
    
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_pos = map.vals[i];
        

        it_pos = ntg_xy_add(it_pos, dcr_sum);

        it_pos = ntg_xy_pos_clamp(it_pos, it_child->_size, object->_size);

        it_child->_pos.x = it_pos.x;
        it_child->_pos.y = it_pos.y;
    }
}

void _ntg_object_draw(ntg_object* object, sarena* arena, int* out_status)
{
    ntg_init_status(out_status);

    if(!object || !arena)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    

    int _status;
    
    const ntg_scene* scene = ntg_object_get_scene_(object);
    ntg_object_drawing_set_size(&object->_drawing, object->_size, scene->_size, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    if(ntg_xy_size_is_zero(object->_size))
        return;

    if(ntg_insets_hsum(object->_border.size) || ntg_insets_vsum(object->_border.size))
        draw_unoptimized(object, arena);
    else
        draw_optimized(object, arena);

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_RENDER);
}

void _ntg_object_root_set_hsize(ntg_object* object, size_t size)
{
    if(!object || object->_parent)
        return;

    size_t cont_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);
    object->__old_cont_size.x = cont_size;

    size_t old = object->_size.x;
    object->__old_size.x = old;

    if(old != size)
    {
        ntg_object_mark_dirty(object,
                NTG_OBJECT_DIRTY_HCONSTRAIN |
                NTG_OBJECT_DIRTY_VMEASURE |
                NTG_OBJECT_DIRTY_VCONSTRAIN);

        object->_size.x = size;
        object->__skip_hborder = false;
        object->__skip_hpadding = false;
    }
}

void _ntg_object_root_set_vsize(ntg_object* object, size_t size)
{
    if(!object || object->_parent)
        return;

    size_t cont_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);
    object->__old_cont_size.y = cont_size;

    size_t old = object->_size.y;
    object->__old_size.y = old;

    if(old != size)
    {
        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN);

        object->_size.y = size;
    }
}

void _ntg_object_root_set_pos(ntg_object* object, struct ntg_xy pos)
{
    if(!object || object->_parent)
        return;

    object->_pos = pos;
}

void _ntg_object_clean(ntg_object* object, uint8_t dirty)
{
    if(!object) return;

    object->_dirty &= ~dirty;
}

void _ntg_object_on_scene_change(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    object->__skip_hborder = false;
    object->__skip_hpadding = false;
    object->__repeat = false;
}

void _ntg_object_focus(ntg_object* object, ntg_object* old_focused)
{
    if(!object) return;

    if(object->__vtable->focus_fn)
        object->__vtable->focus_fn(object, old_focused);

    if(object->hooks.on_focus_fn)
        object->hooks.on_focus_fn(object, old_focused);
}

void _ntg_object_unfocus(ntg_object* object, ntg_object* new_focused)
{
    if(!object) return;

    if(object->__vtable->unfocus_fn)
        object->__vtable->unfocus_fn(object, new_focused);

    if(object->hooks.on_unfocus_fn)
        object->hooks.on_unfocus_fn(object, new_focused);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LAYOUT OBJECT INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_object_size_map* map,
        const ntg_object_vec* children,
        sarena* arena)
{
    if(children->size == 0)
    {
        map->keys = NULL;
        map->vals = NULL;
        map->size = 0;
        return 0;
    }

    map->size = children->size;
    map->keys = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
    if(!map->keys) return NTG_ERR_ALLOC_FAIL;
    map->vals = sarena_malloc(arena, sizeof(size_t) * children->size);
    if(!map->vals) return NTG_ERR_ALLOC_FAIL;
    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = 0;
    }

    return 0;
}

static int pos_map_init(
        ntg_object_pos_map* map,
        const ntg_object_vec* children,
        sarena* arena)
{
    if(children->size == 0)
    {
        map->keys = NULL;
        map->vals = NULL;
        map->size = 0;
        return 0;
    }

    map->size = children->size;
    map->keys = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
    if(!map->keys) return NTG_ERR_ALLOC_FAIL;
    map->vals = sarena_malloc(arena, sizeof(struct ntg_xy) * children->size);
    if(!map->keys) return NTG_ERR_ALLOC_FAIL;

    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = ntg_xy(0, 0);
    }

    return 0;
}

static int tmp_drawing_init(
        struct ntg_object_tmp_drawing* drawing,
        struct ntg_xy size,
        struct ntg_vcell base_bg,
        sarena* arena)
{
    if(!drawing || !arena)
        return NTG_ERR_INVALID_ARG;

    drawing->data = NULL;
    drawing->size = ntg_xy(0, 0);

    drawing->data = sarena_malloc(arena, 1 +
        sizeof(struct ntg_vcell) * size.x * size.y);
    if(!drawing->data) return NTG_ERR_ALLOC_FAIL;
    drawing->size = size;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(drawing, base_bg, ntg_xy(j, i));
        }
    }

    return 0;
}

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static struct ntg_object_measure incorporate_user_measure(
        struct ntg_object_measure measure,
        size_t user_min_size,
        size_t user_max_size,
        size_t user_grow)
{
    user_min_size = _min2_size(user_min_size, user_max_size);
 
    if(user_max_size < measure.min_size)
        measure.min_size = user_max_size;
    else
        measure.min_size = _max2_size(measure.min_size, user_min_size);
 
    if(user_min_size > measure.max_size)
        measure.max_size = user_min_size;
    else
        measure.max_size = _min2_size(measure.max_size, user_max_size);
 
    measure.nat_size = _clamp_size(
            measure.min_size,
            measure.nat_size,
            measure.max_size);
 
    measure.grow = (user_grow != NTG_OBJECT_GROW_UNSET) ?
            user_grow : measure.grow;

    return measure;
}

static void get_dcr_size(
        ntg_object_dcr_enable enable,
        size_t pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t out_size[2],
        sarena* arena)
{
    size_t extra;

    
    
    

    if(enable == NTG_OBJECT_DCR_ENABLE_MIN)
        extra = _ssub_size(size, inner_measure.min_size);
    else if(enable == NTG_OBJECT_DCR_ENABLE_NAT)
        extra = _ssub_size(size, inner_measure.nat_size);
    else extra = size;

    out_size[0] = 0;
    out_size[1] = 0;
    ntg_sap_cap_round_robin(pref_size, NULL, out_size, extra, 2, arena, NULL);
}

static void calculate_border_hsize(ntg_object* object,
        sarena* arena, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->_border.opts.pref_size.w;
    we_pref_size[1] = object->_border.opts.pref_size.e;
    size_t _sizes[2] = {0};

    get_dcr_size(
            object->_border.opts.enable,
            we_pref_size,
            object->_size.x,
            ntg_object_get_measure_pad(object, NTG_ORIENT_H),
            _sizes,
            arena);

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
}

static bool vconstrain_border(ntg_object* object, sarena* arena)
{
    struct ntg_insets border_size = object->_border.size;
    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    size_t n, s;
    calculate_border_vsize(object, arena, &n, &s);

    struct ntg_insets tmp_border_size = border_size;
    tmp_border_size.n = n;
    tmp_border_size.s = s;

    bool hborder_missing = 
            (ntg_insets_hsum(pref_border_size) > 0) &&
            (ntg_insets_hsum(tmp_border_size) == 0);
    bool vborder_missing = 
            (ntg_insets_vsum(pref_border_size) > 0) &&
            (ntg_insets_vsum(tmp_border_size) == 0);

    if(!hborder_missing && vborder_missing)
    {
        
        object->__skip_hborder = true;
        object->__repeat = true;
        return true;
    }
    else if(hborder_missing && !vborder_missing)
    {
        if(object->__skip_hborder)
        {
            object->__skip_hborder = false;
            object->__repeat = true;
            return true;
        }
        else
        {
            object->_border.size.n = 0;
            object->_border.size.s = 0;
        }
    }
    else if(hborder_missing && vborder_missing) 
    {
        object->_border.size.n = 0;
        object->_border.size.s = 0;
    }
    else 
    {
        object->_border.size.n = n;
        object->_border.size.s = s;
    }

    return false;
}

static bool vconstrain_padding(ntg_object* object, sarena* arena)
{
    struct ntg_insets padding_size = object->_padding.size;
    struct ntg_insets pref_padding_size = object->_padding.opts.pref_size;

    size_t n, s;
    calculate_padding_vsize(object, arena, &n, &s);

    struct ntg_insets tmp_padding_size = padding_size;
    tmp_padding_size.n = n;
    tmp_padding_size.s = s;

    bool hpadding_missing = 
            (ntg_insets_hsum(pref_padding_size) > 0) &&
            (ntg_insets_hsum(tmp_padding_size) == 0);
    bool vpadding_missing = 
            (ntg_insets_vsum(pref_padding_size) > 0) &&
            (ntg_insets_vsum(tmp_padding_size) == 0);

    if(!hpadding_missing && vpadding_missing)
    {
        
        object->__skip_hpadding = true;
        object->__repeat = true;
        return true;
    }
    else if(hpadding_missing && !vpadding_missing)
    {
        if(object->__skip_hpadding)
        {
            object->__skip_hpadding = false;
            object->__repeat = true;
            return true;
        }
        else
        {
            object->_padding.size.n = 0;
            object->_padding.size.s = 0;
        }
    }
    else if(hpadding_missing && vpadding_missing) 
    {
        object->_padding.size.n = 0;
        object->_padding.size.s = 0;
    }
    else 
    {
        object->_padding.size.n = n;
        object->_padding.size.s = s;
    }

    return false;
}


static void calculate_border_vsize(ntg_object* object,
        sarena* arena, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->_border.opts.pref_size.n;
    ns_pref_size[1] = object->_border.opts.pref_size.s;
    size_t _sizes[2] = {0};

    get_dcr_size(
            object->_border.opts.enable,
            ns_pref_size, 
            object->_size.y,
            ntg_object_get_measure_pad(object, NTG_ORIENT_V),
            _sizes,
            arena);

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
}

static void calculate_padding_hsize(ntg_object* object,
        sarena* arena, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->_padding.opts.pref_size.w;
    we_pref_size[1] = object->_padding.opts.pref_size.e;
    size_t _sizes[2] = {0};

    get_dcr_size(
            object->_padding.opts.enable,
            we_pref_size,  
            ntg_object_get_size_pad(object).x,
            ntg_object_get_measure_cont(object, NTG_ORIENT_H),
            _sizes,
            arena);

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
}

static void calculate_padding_vsize(ntg_object* object,
        sarena* arena, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->_padding.opts.pref_size.n;
    ns_pref_size[1] = object->_padding.opts.pref_size.s;

    size_t _sizes[2] = {0};

     get_dcr_size(
             object->_padding.opts.enable,
             ns_pref_size,  
             ntg_object_get_size_pad(object).y,
             ntg_object_get_measure_cont(object, NTG_ORIENT_V),
             _sizes,
             arena);

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
}

static void draw_optimized(ntg_object* object, sarena* arena)
{
    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = ntg_object_drawing_get_size(&object->_drawing);

    struct ntg_vcell bg = object->__base_bg;
    struct ntg_insets psize = object->_padding.size;

    struct ntg_object_tmp_drawing content_drawing;
    if(tmp_drawing_init(&content_drawing, content_size, bg, arena) != 0)
        return;

    size_t i, j;

    
    if(object->__vtable->draw_fn)
    {
        object->__vtable->draw_fn(object, &content_drawing, object->layout_cache, arena);
    }

    struct ntg_vcell it_src_cell;

    struct ntg_xy offset = ntg_xy(psize.w, psize.n); 
    struct ntg_xy ji;

    

    for(i = 0; i < object_size.y; i++)
    {
        for(j = 0; j < object_size.x; j++)
        {
            ji = ntg_xy(j, i);
            ntg_object_drawing_set(&object->_drawing, bg, ji);
        }
    }

    for(i = 0; i < content_size.y; i++)
    {
        for(j = 0; j < content_size.x; j++)
        {
            ji = ntg_xy(j, i);

            it_src_cell = ntg_object_tmp_drawing_get(&content_drawing, ji);
            ntg_object_drawing_set(&object->_drawing, it_src_cell, ntg_xy_add(offset, ji));
        }
    }
}

static void draw_unoptimized(ntg_object* object, sarena* arena)
{
    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = ntg_object_drawing_get_size(&object->_drawing);

    struct ntg_vcell bg = object->__base_bg;
    struct ntg_insets bsize = object->_border.size;
    struct ntg_insets psize = object->_padding.size;
    const struct ntg_border_style* border_style = object->_border.opts.style;

    struct ntg_object_tmp_drawing content_drawing;
    if(tmp_drawing_init(&content_drawing, content_size, bg, arena) != 0)
        return;

    struct ntg_object_tmp_drawing object_drawing;
    if(tmp_drawing_init(&object_drawing, object_size, bg, arena) != 0)
        return;

    
    if(border_style->draw_fn)
    {
        border_style->draw_fn(border_style->data, object_size, bsize, &object_drawing);
    }

    size_t i, j;

    
    for(i = bsize.n; i < (object_size.y - bsize.s); i++)
    {
        for(j = bsize.w; j < (object_size.x - bsize.e); j++)
        {
            ntg_object_tmp_drawing_set(&object_drawing, bg, ntg_xy(j, i));
        }
    }

    
    if(object->__vtable->draw_fn)
    {
        object->__vtable->draw_fn(object, &content_drawing, object->layout_cache, arena);
    }

    struct ntg_vcell it_src_cell;

    

    struct ntg_xy offset = ntg_xy(bsize.w + psize.w, bsize.n + psize.n); 
    struct ntg_xy ji;
    for(i = 0; i < content_size.y; i++)
    {
        for(j = 0; j < content_size.x; j++)
        {
            ji = ntg_xy(j, i);

            it_src_cell = ntg_object_tmp_drawing_get(&content_drawing, ji);

            ntg_object_tmp_drawing_set(
                    &object_drawing,
                    it_src_cell,
                    ntg_xy_add(offset, ji));
        }
    }

    

    for(i = 0; i < object_size.y; i++)
    {
        for(j = 0; j < object_size.x; j++)
        {
            ji = ntg_xy(j, i);

            it_src_cell = ntg_object_tmp_drawing_get(&object_drawing, ji);
            ntg_object_drawing_set(&object->_drawing, it_src_cell, ji);
        }
    }
}
