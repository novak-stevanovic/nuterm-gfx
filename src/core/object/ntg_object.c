#include <limits.h>
#include <stdlib.h>
#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

GENC_VECTOR_DEFINE(ntg_objptr_vec, ntg_object*, 1.5, )

static int
objptr_vec_rm_value(ntg_objptr_vec* vec, const ntg_object* object)
{
    if(!vec)
        return GENC_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == object)
            return ntg_objptr_vec_rm_at(vec, i);
    }

    return GENC_ERR_NO_DATA;
}

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

enum ntg_object_repeat_flag
{
    NTG_OBJECT_SKIP_HBORDER = (1u << 0),
    NTG_OBJECT_SKIP_HPADDING = (1u << 1),
    NTG_OBJECT_SPECIAL_REPEAT = (1u << 2)
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* LAYOUT OBJECT INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_object_size_map* map,
        const ntg_objptr_vec* children,
        sarena* arena);

static int pos_map_init(
        ntg_object_pos_map* map,
        const ntg_objptr_vec* children,
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
    
static int get_dcr_size(
        enum ntg_object_dcr_enable enable,
        size_t we_pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t we_out_size[2]);

static int vconstrain_border(ntg_object* object, bool* out_repeat);
static int vconstrain_padding(ntg_object* object, bool* out_repeat);
static int calculate_border_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e);
static int calculate_border_vsize(
        ntg_object* object, size_t* out_n, size_t* out_s);
static int calculate_padding_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e);
static int calculate_padding_vsize(
        ntg_object* object, size_t* out_n, size_t* out_s);

static int 
draw_optimized(ntg_object* object, sarena* arena, uint32_t* relayout);
static int 
draw_unoptimized(ntg_object* object, sarena* arena, uint32_t* relayout);
static void layout_reset(ntg_object* object);
static bool set_hmeasure_helper(ntg_object* object, struct ntg_object_measure measure);
static bool set_vmeasure_helper(ntg_object* object, struct ntg_object_measure measure);
static bool set_hsize_helper(ntg_object* object, size_t size);
static bool set_vsize_helper(ntg_object* object, size_t size);
static inline bool set_pos_helper(ntg_object* object, struct ntg_xy pos);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_border_opts ntg_border_opts_default(void)
{
    return (struct ntg_border_opts) {
        .style = &NTG_BORDER_STYLE_DEFAULT,
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

struct ntg_padding_opts ntg_padding_opts_default(void)
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

struct ntg_layout_opts ntg_layout_opts_default(void)
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

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

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

NTG_OBJECT_DEF_TRAVERSE_PREORDER(count_tree, count_fn)

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

    const ntg_objptr_vec* children = &object->_children;
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
        enum ntg_object_hit_result* out_hit)
{
    if(!object) return NULL;

    /* If not hit, return NULL */

    if(!ntg_xy_is_in_rectangle(pos, ntg_xy(0, 0), object->_size))
    {
        if(out_local_pos) *out_local_pos = ntg_xy(0, 0);
        return NULL;
    }

    int curr_z = INT_MIN;
    ntg_object* best_obj = object;

    /* Set `out_local_pos`, ancestors will choose the right one later */

    ntg_set_out(out_local_pos, pos);

    /* Set `out_hit`, ancestors will choose the right one later */

    struct ntg_insets padding_size = object->_padding_size;
    struct ntg_insets border_size = object->_border_size;
    struct ntg_xy cont_size = ntg_object_get_size_cont(object);

    if((pos.x > (padding_size.w + cont_size.x + padding_size.e)) ||
       (pos.x < border_size.w) ||
       (pos.y > (padding_size.n + cont_size.y + padding_size.s)) ||
       (pos.y < border_size.n))
    {
        ntg_set_out(out_hit, NTG_OBJECT_HIT_BORDER);
    }
    else if((pos.x >= cont_size.x) ||
            (pos.x < (border_size.w + padding_size.w)) ||
            (pos.y >= cont_size.y) ||
            (pos.y < (border_size.n + padding_size.n)))
    {
        ntg_set_out(out_hit, NTG_OBJECT_HIT_PADDING);
    }
    else
    {
        ntg_set_out(out_hit, NTG_OBJECT_HIT_CONT);
    }

    size_t i;
    struct ntg_xy it_child_local;
    enum ntg_object_hit_result _hit;
    struct ntg_dxy it_adj_pos;
    ntg_object* it_child;
    ntg_object* it_hit;
    for(i = 0; i < object->_children.size; i++)
    {
        it_child = object->_children.data[i];
        it_adj_pos = ntg_dxy_sub(ntg_dxy_from_xy(pos), ntg_dxy_from_xy(it_child->_pos));

        if(ntg_dxy_is_in_rectangle(it_adj_pos, ntg_dxy(0, 0), ntg_dxy_from_xy(it_child->_size)))
        {
            it_hit = ntg_object_hit_test(it_child, ntg_xy_from_dxy(it_adj_pos), &it_child_local, &_hit);

            if(it_hit && (it_child->_layout_opts.z_index > curr_z))
            {
                best_obj = it_hit;
                curr_z = it_child->_layout_opts.z_index;
                
                ntg_set_out(out_local_pos, it_child_local);
                ntg_set_out(out_hit, _hit);
            }
        }
    }
    return best_obj;
}

int ntg_object_detach(ntg_object* object)
{
    if(!object) return NTG_ERR_INV_ARG;

    ntg_object* parent = object->_parent;
    if(parent == NULL) return 0;

    ntg_scene* scene = ntg_object_get_scene_(object);

    objptr_vec_rm_value(&parent->_children, object);

    object->_parent = NULL;

    if(parent->__vtable->rm_child_fn)
        parent->__vtable->rm_child_fn(parent, object);

    if(scene)
        _ntg_scene_rm_object_tree(scene, object);

    ntg_object_mark_dirty(parent, NTG_OBJECT_DIRTY_FULL);

    if(object->__vtable->rm_parent_fn)
        object->__vtable->rm_parent_fn(object, parent);

    struct ntg_event_object_chldrm_dt chldrm_dt = { .child = object };
    struct ntg_event_object_prntrm_dt prntrm_dt = { .parent = parent };

    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_PRNTRM, object, &prntrm_dt));
            
    ntg_event_raise(
            &parent->_event_del,
           ntg_event_new(NTG_EVENT_OBJECT_CHLDRM, parent, &chldrm_dt));

    if(scene)
        _ntg_scene_unregister_tree(scene, object);

    return 0;
}

int ntg_object_anchor(
        ntg_object* base,
        ntg_object* root,
        const struct ntg_anchor_policy* policy)
{
    int _status;

    if(!base || !root || !policy || (base == root))
        return NTG_ERR_INV_ARG;

    if(base->_anchored.size >= NTG_OBJECT_MAX_ANCHORED)
        return NTG_ERR_MAX_ANCHORED;

    if(root->_parent)
    {
        ntg_object_detach(root);
    }

    if(root->_base)
    {
        ntg_object_unanchor(root);
    }

    _status = ntg_objptr_vec_pushb(&base->_anchored, root);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;
            case GENC_ERR_OVERFLOW:
                return NTG_ERR_OVERFLOW;

            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    root->_base = base;
    root->_anchor_policy = policy;

    ntg_scene* scene = ntg_object_get_scene_(root);

    if(scene)
        _ntg_scene_add_object_tree(scene, root);

    if(base->__vtable->add_anchored_fn)
        base->__vtable->add_anchored_fn(base, root);

    struct ntg_event_object_anchadd_dt anchadd_dt = { .anchored = root };
    ntg_event_raise(
            &base->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_ANCHADD, base, &anchadd_dt));

    if(root->__vtable->set_base_fn)
        root->__vtable->set_base_fn(root, base);

    struct ntg_event_object_bsset_dt bsset_dt = { .base = base };
    ntg_event_raise(
            &root->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_BSSET, root, &bsset_dt));

    if(scene)
        _ntg_scene_register_tree(scene, root);

    return 0;
}

int ntg_object_unanchor(ntg_object* root)
{
    if(!root) return NTG_ERR_INV_ARG;
    if(!root->_base) return 0;

    ntg_object* base = root->_base;

    ntg_scene* scene = ntg_object_get_scene_(root);

    objptr_vec_rm_value(&root->_base->_anchored, root);
    root->_base = NULL;
    root->_anchor_policy = NULL;

    if(base->__vtable->rm_anchored_fn)
        base->__vtable->rm_anchored_fn(base, root);

    struct ntg_event_object_anchrm_dt anchrm_dt = { .anchored = root };
    ntg_event_raise(
            &base->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_ANCHRM, base, &anchrm_dt));

    if(scene)
        _ntg_scene_rm_object_tree(scene, root);

    if(root->__vtable->rm_base_fn)
        root->__vtable->rm_base_fn(root, base);

    struct ntg_event_object_bsrm_dt bsrm_dt = { .base = base };
    ntg_event_raise(
            &root->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_BSRM, root, &bsrm_dt));

    if(scene)
        _ntg_scene_unregister_tree(scene, root);

    return 0;
}

int ntg_object_remove_from_scene(ntg_object* object)
{
    if(!object) return NTG_ERR_INV_ARG;

    ntg_object* parent = object->_parent;
    if(parent)
    {
        ntg_object_detach(object);
        return 0;
    }

    ntg_object* base = object->_base;
    if(base)
    {
        ntg_object_unanchor(object);
        return 0;
    }

    if(ntg_object_is_true_root(object) && ntg_object_get_scene(object))
    {
        ntg_scene_set_root(ntg_object_get_scene_(object), NULL);
        return 0;
    }

    return 0;
}

/* ------------------------------------------------------ */
/* CONTROL */
/* ------------------------------------------------------ */

int ntg_object_set_layout_opts(
        ntg_object* object,
        const struct ntg_layout_opts* opts)
{
    if(!object) return NTG_ERR_INV_ARG;

    struct ntg_layout_opts old_opts = object->_layout_opts;
    struct ntg_layout_opts new_opts = (opts ? (*opts) : ntg_layout_opts_default());

    if(ntg_layout_opts_are_eql(&old_opts, &new_opts))
        return 0;

    object->_layout_opts = new_opts;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->__vtable->chng_layout_opts_fn)
    {
        object->__vtable->chng_layout_opts_fn(
                object,
                &old_opts,
                &object->_layout_opts);
    }

    struct ntg_event_object_layoptchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &object->_layout_opts
    };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_LAYOPTCHG, object, &event_dt));

    return 0;
}

int ntg_object_set_border_opts(
        ntg_object* object,
        const struct ntg_border_opts* opts)
{
    if(!object) return NTG_ERR_INV_ARG;

    struct ntg_border_opts old_opts = object->_border_opts;
    struct ntg_border_opts new_opts = (opts ? (*opts) : ntg_border_opts_default());

    if(ntg_border_opts_are_eql(&old_opts, &new_opts))
        return 0;

    object->_border_opts = new_opts;
    if(!object->_border_opts.style)
        object->_border_opts.style = &NTG_BORDER_STYLE_DEFAULT;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->__vtable->chng_border_opts_fn)
    {
        object->__vtable->chng_border_opts_fn(
                object,
                &old_opts,
                &object->_border_opts);
    }

    struct ntg_event_object_bdroptchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &object->_border_opts
    };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_BDROPTCHG, object, &event_dt));

    return 0;
}

int ntg_object_set_padding_opts(
        ntg_object* object,
        const struct ntg_padding_opts* opts)
{
    if(!object) return NTG_ERR_INV_ARG;

    struct ntg_padding_opts old_opts = object->_padding_opts;
    struct ntg_padding_opts new_opts = (opts ? (*opts) : ntg_padding_opts_default());

    if(ntg_padding_opts_are_eql(&old_opts, &new_opts))
        return 0;

    object->_padding_opts = new_opts;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->__vtable->chng_padding_opts_fn)
    {
        object->__vtable->chng_padding_opts_fn(
                object,
                &old_opts,
                &object->_padding_opts);
    }

    struct ntg_event_object_padoptchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &object->_padding_opts
    };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_PADOPTCHG, object, &event_dt));

    return 0;
}

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_abs_pos(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    return ntg_xy_from_dxy(ntg_object_map_to_scene(object, ntg_dxy_from_xy(object->_pos)));
}

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

    struct ntg_event_object_key_dt event_dt = { .key = event };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_KEY, object, &event_dt));

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
    enum ntg_object_hit_result _hit_result;
    ntg_object_hit_test(object, pos, NULL, &_hit_result);

    if(!(object->_clickable == NTG_OBJECT_CLICKABLE_BORDER) &&
        (_hit_result == NTG_OBJECT_HIT_BORDER))
    {
        return false;
    }

    bool consumed = false;

    if(object->__vtable->process_mouse_fn)
    {
        consumed = object->__vtable->process_mouse_fn(object, event);
    }

    struct ntg_event_object_mouse_dt event_dt = { .mouse = event };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_MOUSE, object, &event_dt));

    return consumed;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_object* object)
{
    (*object) = (ntg_object) {0};

    object->_layout_opts = ntg_layout_opts_default();

    object->_border_opts = ntg_border_opts_default();
    object->_padding_opts = ntg_padding_opts_default();
    object->_anchor_policy = &NTG_ANCHOR_POLICY_ROOT;

    object->__base_bg = ntg_vcell_new_default();

    object->_clickable = NTG_OBJECT_UNCLICKABLE;
    object->_focusable = NTG_OBJECT_UNFOCUSABLE;
}

int ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!object)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instance_of(type, &NTG_TYPE_OBJECT))
        return NTG_ERR_INV_TYPE;

    if(!vtable ||!vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    init_default(object);

    object->_type = type;
    object->__vtable = vtable;
    object->layout_dt = layout_dt;

    ntg_object_drawing_init(&object->_drawing);

    ntg_event_delegate_init(&object->_event_del);

    return 0;
}

int ntg_object_deinit(ntg_object* object)
{
    ntg_not_null(object);

    ntg_scene* scene = ntg_object_get_scene_(object);
    
    if(ntg_object_is_true_root(object) && scene)
    {
        ntg_scene_set_root(scene, NULL);
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

    while(object->_anchored.size > 0)
    {
        ntg_object_unanchor(object->_anchored.data[0]);
    }

    ntg_objptr_vec_deinit(&object->_children);

    ntg_objptr_vec_deinit(&object->_anchored);

    ntg_object_drawing_deinit(&object->_drawing);

    ntg_event_delegate_deinit(&object->_event_del);

    if(object->layout_dt && object->layout_dt->free_fn)
        object->layout_dt->free_fn(object->layout_dt);

    init_default(object);

    return 0;
}

int ntg_object_attach(ntg_object* parent, ntg_object* child)
{
    int _status;

    if(!parent || !child || (child == parent))
        return NTG_ERR_INV_ARG;

    if(parent->_children.size >= NTG_OBJECT_MAX_CHILDREN)
        return NTG_ERR_MAX_CHILDREN;

    if(child->_parent != NULL)
        ntg_object_detach(child);

    if(child->__scene) 
        ntg_scene_set_root(child->__scene, NULL);

    if(child->_base)
        ntg_object_unanchor(child);

    ntg_scene* scene = ntg_object_get_scene_(parent);

    _status = ntg_objptr_vec_pushb(&parent->_children, child);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;
            case GENC_ERR_OVERFLOW:
                return NTG_ERR_OVERFLOW;

            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    child->_parent = parent;

    if(scene)
        _ntg_scene_add_object_tree(scene, child);

    if(child->__vtable->set_parent_fn)
        child->__vtable->set_parent_fn(child, parent);

    struct ntg_event_object_chldadd_dt chldadd_dt = { .child = child };
    struct ntg_event_object_prntset_dt prntset_dt = { .parent = parent };

    ntg_event_raise(
            &parent->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_CHLDADD, parent, &chldadd_dt));

    ntg_event_raise(
            &child->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_PRNTSET, child, &prntset_dt));

    if(scene)
        _ntg_scene_register_tree(scene, child);

    ntg_object_mark_dirty(parent, NTG_OBJECT_DIRTY_FULL);
    return 0;
}

int ntg_object_set_base_bg(ntg_object* object, struct ntg_vcell base_bg)
{
    if(!object) return NTG_ERR_INV_ARG;

    object->__base_bg = base_bg;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_LAYOUT_PREPARE | NTG_OBJECT_DIRTY_DRAW);

    return 0;
}

int ntg_object_set_focusable(ntg_object* object, enum ntg_object_focus_mode mode)
{
    if(!object) return NTG_ERR_INV_ARG;

    if(object->_focusable == NTG_OBJECT_FOCUSABLE)
    {
        const ntg_scene* scene = ntg_object_get_scene(object);
        if(scene)
        {
            ntg_fcs_manager* fm = scene->_fm;
            if(fm && (fm->_focused == object))
                ntg_fcs_manager_request_focus(fm, NULL);
        }
    }

    object->_focusable = mode;

    return 0;
}

int ntg_object_set_clickable(ntg_object* object, enum ntg_object_click_mode mode)
{
    if(!object) return NTG_ERR_INV_ARG;

    object->_clickable = mode;

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void _ntg_object_root_set_scene(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;
    if(object->_parent || object->_base) return;

    object->__scene = scene;

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

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

int ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size)
{
    if(!map || !object) return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
        {
            map->vals[i] = size;
            break;
        }
    }

    return 0;
}

int ntg_object_zero_constrain(const ntg_object* object, ntg_object_size_map* map)
{
    if(!object || !map)
        return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < object->_children.size; i++)
        ntg_object_size_map_set(map, object, 0);

    return 0;
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

int ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos)
{
    if(!map || !object) return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
        {
            map->vals[i] = pos;
            break;
        }
    }

    return 0;
}

int ntg_object_zero_arrange(const ntg_object* object, ntg_object_pos_map* map)
{
    if(!object || !map)
        return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < object->_children.size; i++)
        ntg_object_pos_map_set(map, object, ntg_xy(0, 0));

    return 0;
}

/* ------------------------------------------------------ */
/* DRAW PHASE */
/* ------------------------------------------------------ */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_object_mark_dirty(ntg_object* object, uint32_t dirty)
{
    if(!object) return NTG_ERR_INV_ARG;

    object->_dirty |= dirty;

    ntg_scene* scene = ntg_object_get_scene_(object);
    if(scene)
        ntg_scene_mark_dirty(scene);

    return 0;
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
ntg_object_get_measure(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
        return (struct ntg_object_measure) {0};

    return (struct ntg_object_measure) {
        .min_size = ntg_xy_get(object->_min_size, orient),
        .nat_size = ntg_xy_get(object->_nat_size, orient),
        .max_size = ntg_xy_get(object->_max_size, orient),
        .grow = ntg_xy_get(object->_grow, orient)
    };
}

size_t ntg_object_get_size_1d(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
        return 0;
        
    return ntg_xy_get(object->_size, orient);
}

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->_min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
        ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->_nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->_max_size, sub);
}

struct ntg_xy ntg_object_get_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets border_size = object->_border_size;
    struct ntg_insets padding_size = object->_padding_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(border_size) + ntg_insets_hsum(padding_size),
        ntg_insets_vsum(border_size) + ntg_insets_vsum(padding_size)
    );
    return ntg_xy_sub(object->_size, sub);
}

struct ntg_object_measure
ntg_object_get_measure_cont(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    struct ntg_object_measure m = ntg_object_get_measure(object, orient);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->_padding_opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient) +
            ntg_insets_sum(pref_padding_size, orient);

    m.min_size = _sub2_size(m.min_size, sub);
    m.nat_size = _sub2_size(m.nat_size, sub);
    m.max_size = _sub2_size(m.max_size, sub);

    return m;
}

size_t ntg_object_get_size_1d_cont(const ntg_object* object, enum ntg_orient orient)
{
    if(!object) return 0;

    size_t s = ntg_xy_get(object->_size, orient);

    struct ntg_insets border_size = object->_border_size;
    struct ntg_insets padding_size = object->_padding_size;

    size_t sub = ntg_insets_sum(border_size, orient) +
            ntg_insets_sum(padding_size, orient);

    return _sub2_size(s, sub);
}

size_t ntg_object_get_for_size_cont(const ntg_object* object, enum ntg_orient orient)
{
    if(!object) return 0;

    return (orient == NTG_ORIENT_H) ?
            NTG_SIZE_MAX :
            ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);
}

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size),
        ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_max_size, sub);
}

struct ntg_xy ntg_object_get_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets border_size = object->_border_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(border_size),
            ntg_insets_vsum(border_size)
    );
    return ntg_xy_sub(object->_size, sub);
}

struct ntg_object_measure
ntg_object_get_measure_pad(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    struct ntg_object_measure m = ntg_object_get_measure(object, orient);

    struct ntg_insets pref_border_size = object->_border_opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient);

    m.min_size = _sub2_size(m.min_size, sub);
    m.nat_size = _sub2_size(m.nat_size, sub);
    m.max_size = _sub2_size(m.max_size, sub);

    return m;
}

size_t ntg_object_get_size_1d_pad(const ntg_object* object, enum ntg_orient orient)
{
    if(!object) return 0;

    size_t s = ntg_xy_get(object->_size, orient);

    struct ntg_insets border_size = object->_border_size;

    size_t sub = ntg_insets_sum(border_size, orient);

    return _sub2_size(s, sub);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int _ntg_object_hmeasure(
        ntg_object* object,
        sarena* arena,
        uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    struct ntg_object_measure measure = {0};
    int _status = 0;
    if(object->__vtable->measure_fn)
    {
        _status = object->__vtable->measure_fn(
                object,
                object->layout_dt,
                NTG_ORIENT_H,
                arena,
                relayout,
                &measure);
        if(_status)
        {
            set_hmeasure_helper(object, measure);
            return _status;
        }

        size_t extra = ntg_insets_hsum(object->_padding_opts.pref_size) +
                       ntg_insets_hsum(object->_border_opts.pref_size);

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

    set_hmeasure_helper(object, measure);
    return 0;
}

int _ntg_object_hconstrain(
        ntg_object* object,
        sarena* arena,
        uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    if(object->__skip_hborder)
    {
        object->_border_size.w = 0;
        object->_border_size.e = 0;
    }
    else
    {
        size_t w, e;
        int status = calculate_border_hsize(object, &w, &e);
        if(status != 0)
            return status;

        object->_border_size.w = w;
        object->_border_size.e = e;
    }
    if(object->__skip_hpadding)
    {
        object->_padding_size.w = 0;
        object->_padding_size.e = 0;
    }
    else
    {
        size_t w, e;
        int status = calculate_padding_hsize(object, &w, &e);
        if(status != 0)
            return status;

        object->_padding_size.w = w;
        object->_padding_size.e = e;
    }

    ntg_object_mark_dirty(object,
            NTG_OBJECT_DIRTY_VCONSTRAIN |
            NTG_OBJECT_DIRTY_ARRANGE |
            NTG_OBJECT_DIRTY_DRAW |
            NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);

    int _status;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);

    ntg_object_size_map map;
    _status = size_map_init(&map, &object->_children, arena);
    if(_status != 0)
    {
        for(i = 0; i < object->_children.size; i++)
            set_hsize_helper(object->_children.data[i], 0);

        return _status;
    }

    if(object->__vtable->constrain_fn)
    {
        _status = object->__vtable->constrain_fn(
                object,
                object->layout_dt,
                NTG_ORIENT_H,
                &map,
                arena,
                relayout);
        if(_status)
        {
            for(i = 0; i < object->_children.size; i++)
                set_hsize_helper(object->_children.data[i], 0);

            return _status;
        }
    }

    ntg_object* it_child;
    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = _min2_size(content_size, map.vals[i]);

        set_hsize_helper(it_child, it_size);
    }
    return 0;
}

int _ntg_object_vmeasure(
        ntg_object* object,
        sarena* arena,
        uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    struct ntg_object_measure measure = {0};
    int _status = 0;
    if(object->__vtable->measure_fn)
    {
        _status = object->__vtable->measure_fn(
                object,
                object->layout_dt,
                NTG_ORIENT_V,
                arena,
                relayout,
                &measure);
        if(_status)
        {
            set_vmeasure_helper(object, measure);
            return _status;
        }

        size_t extra = ntg_insets_vsum(object->_padding_opts.pref_size) +
                ntg_insets_vsum(object->_border_opts.pref_size);

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

    set_vmeasure_helper(object, measure);
    return 0;
}

int _ntg_object_vconstrain(
        ntg_object* object,
        sarena* arena,
        uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    ntg_object_mark_dirty(object,
            NTG_OBJECT_DIRTY_ARRANGE |
            NTG_OBJECT_DIRTY_DRAW |
            NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);

    bool repeat_border = false;
    bool repeat_padding = false;
    int _status = vconstrain_border(object, &repeat_border);
    if(_status != 0)
        return _status;
    _status = vconstrain_padding(object, &repeat_padding);
    if(_status != 0)
        return _status;
    if(relayout && (repeat_border || repeat_padding))
        (*relayout) |= NTG_OBJECT_DIRTY_HCONSTRAIN;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);

    ntg_object_size_map map;
    _status = size_map_init(&map, &object->_children, arena);
    if(_status)
    {
        for(i = 0; i < object->_children.size; i++)
            set_vsize_helper(object->_children.data[i], 0);

        return _status;
    }

    if(object->__vtable->constrain_fn)
    {
        _status = object->__vtable->constrain_fn(
                object,
                object->layout_dt,
                NTG_ORIENT_V,
                &map,
                arena,
                relayout);
        if(relayout && (repeat_border || repeat_padding))
            (*relayout) |= NTG_OBJECT_DIRTY_HCONSTRAIN;
        if(_status)
        {
            for(i = 0; i < object->_children.size; i++)
                set_vsize_helper(object->_children.data[i], 0);

            return _status;
        }
    }

    ntg_object* it_child;
    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = _min2_size(content_size, map.vals[i]);

        set_vsize_helper(it_child, it_size);
    }
    return 0;
}

int _ntg_object_arrange(
        ntg_object* object,
        sarena* arena,
        uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    int _status;
    size_t i;

    ntg_object_pos_map map;
    _status = pos_map_init(&map, &object->_children, arena);
    if(_status)
    {
        for(i = 0; i < object->_children.size; i++)
            set_pos_helper(object->_children.data[i], ntg_xy(0, 0));

        return _status;
    }

    if(object->__vtable->arrange_fn)
    {
        _status = object->__vtable->arrange_fn(
                object,
                object->layout_dt,
                &map,
                arena,
                relayout);
        if(_status)
        {
            for(i = 0; i < object->_children.size; i++)
                set_pos_helper(object->_children.data[i], ntg_xy(0, 0));

            return _status;
        }
    }

    struct ntg_xy dcr_sum = ntg_xy(
            object->_border_size.w + object->_padding_size.w,
            object->_border_size.n + object->_padding_size.n);

    ntg_object* it_child;
    struct ntg_xy it_pos;

    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_pos = ntg_xy_add(map.vals[i], dcr_sum);
        it_pos = ntg_xy_pos_clamp(it_pos, it_child->_size, object->_size);

        set_pos_helper(it_child, it_pos);
    }
    return 0;
}

int _ntg_object_draw(
        ntg_object* object,
        sarena* arena,
        uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_LAYOUT_FINALIZE | NTG_OBJECT_DIRTY_RENDER);

    const ntg_scene* scene = ntg_object_get_scene_(object);
    if(!scene)
        return NTG_ERR_INV_ARG;

    int _status = ntg_object_drawing_set_size(
            &object->_drawing, object->_size, scene->_size);
    struct ntg_xy drawing_size = ntg_object_drawing_get_size(&object->_drawing);

    /* Even if the alloc fails, set the cells to defaults */

    size_t i, j;
    for(i = 0; i < drawing_size.y; i++)
    {
        for(j = 0; j < drawing_size.x; j++)
        {
            ntg_object_drawing_set(&object->_drawing, ntg_vcell_new_default(), ntg_xy(j, i));
        }
    }

    /* Now return if alloc failed */

    if(_status != 0)
        return _status;

    if(ntg_insets_hsum(object->_border_size) || ntg_insets_vsum(object->_border_size))
        _status = draw_unoptimized(object, arena, relayout);
    else
        _status = draw_optimized(object, arena, relayout);

    return _status;
}

void _ntg_object_layout_finalize(ntg_object* object, sarena* arena)
{
    if(!object) return;

    if(object->__vtable && object->__vtable->layout_finalize_fn)
        object->__vtable->layout_finalize_fn(object, arena);

    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_LAYFINAL, object, NULL));

    object->_old_layout = (struct ntg_object_layout_result) {
        .min_size = object->_min_size,
        .nat_size = object->_nat_size,
        .max_size = object->_max_size,
        .grow = object->_grow,
        .size = object->_size,
        .cont_size = ntg_object_get_size_cont(object),
        .border_size = object->_border_size,
        .padding_size = object->_padding_size,
        .abs_pos = ntg_object_get_abs_pos(object),
        .first_layout = false
    };

}

void _ntg_object_root_set_hsize(ntg_object* object, size_t size)
{
    if(!object) return;
    if(object->_parent) return;

    set_hsize_helper(object, size);

}

void _ntg_object_root_set_vsize(ntg_object* object, size_t size)
{
    if(!object) return;
    if(object->_parent) return;

    set_vsize_helper(object, size);

}

void _ntg_object_root_set_pos(ntg_object* object, struct ntg_xy pos)
{
    if(!object) return;
    if(object->_parent) return;

    set_pos_helper(object, pos);

}

void _ntg_object_clean(ntg_object* object, uint32_t clean)
{
    if(!object) return;

    object->_dirty &= (~clean);

}

void _ntg_object_scene_enter(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    layout_reset(object);
    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
    object->_old_layout.first_layout = true;

    if(object->__vtable->enter_scene_fn)
        object->__vtable->enter_scene_fn(object, scene);

}

void _ntg_object_on_scene_enter(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    struct ntg_event_object_scnset_dt event_dt = { .scene = scene };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_SCNSET, object, &event_dt));

}

void _ntg_object_scene_leave(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    layout_reset(object);

    if(object->__vtable->rm_scene_fn)
        object->__vtable->rm_scene_fn(object, scene);

}

void _ntg_object_on_scene_leave(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    struct ntg_event_object_scnrm_dt event_dt = { .scene = scene };
    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_SCNRM, object, &event_dt));

}

void _ntg_object_focus(ntg_object* object)
{
    if(!object) return;

    if(object->__vtable->focus_fn)
        object->__vtable->focus_fn(object);

    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_FCS, object, NULL));

}

void _ntg_object_unfocus(ntg_object* object)
{
    if(!object) return;

    if(object->__vtable->unfocus_fn)
        object->__vtable->unfocus_fn(object);

    ntg_event_raise(
            &object->_event_del,
            ntg_event_new(NTG_EVENT_OBJECT_UNFCS, object, NULL));

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* LAYOUT OBJECT INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_object_size_map* map,
        const ntg_objptr_vec* children,
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
        const ntg_objptr_vec* children,
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
    if(!map->vals) return NTG_ERR_ALLOC_FAIL;

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
        return NTG_ERR_INV_ARG;

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

static void layout_reset(ntg_object* object)
{
    if(!object) return;

    object->_min_size = ntg_xy(0, 0);
    object->_nat_size = ntg_xy(0, 0);
    object->_nat_size = ntg_xy(0, 0);
    object->_grow = ntg_xy(0, 0);
    object->_dirty = 0;
    object->__skip_hborder = false;
    object->__skip_hpadding = false;
    object->__special_repeat = false;
    object->_size = ntg_xy(0, 0);
    object->_border_size = ntg_insets(0, 0, 0, 0);
    object->_padding_size = ntg_insets(0, 0, 0, 0);

    int _status = ntg_object_drawing_set_size(
            &object->_drawing, ntg_xy(0, 0), ntg_xy(1, 1));
    assert(!_status);

    object->_old_layout = (struct ntg_object_layout_result) {0};

    if(object->layout_dt && object->layout_dt->reset_fn)
        object->layout_dt->reset_fn(object->layout_dt);
}

static bool set_hmeasure_helper(ntg_object* object, struct ntg_object_measure measure)
{
    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_H);

    if(!ntg_object_measure_are_eql(measure, old))
    {
        object->_min_size.x = measure.min_size;
        object->_nat_size.x = measure.nat_size;
        object->_max_size.x = measure.max_size;
        object->_grow.x = measure.grow;
        ntg_object_mark_dirty(object,
                NTG_OBJECT_DIRTY_HCONSTRAIN |
                NTG_OBJECT_DIRTY_VMEASURE |
                NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
        if(object->_parent)
        {
            ntg_object_mark_dirty(object->_parent,
                    NTG_OBJECT_DIRTY_MEASURE |
                    NTG_OBJECT_DIRTY_CONSTRAIN);
        }
        return true;
    }

    return false;
}

static bool set_vmeasure_helper(ntg_object* object, struct ntg_object_measure measure)
{
    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_V);

    if(!ntg_object_measure_are_eql(measure, old))
    {
        object->_min_size.y = measure.min_size;
        object->_nat_size.y = measure.nat_size;
        object->_max_size.y = measure.max_size;
        object->_grow.y = measure.grow;

        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN | NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
        if(object->_parent)
        {
            ntg_object_mark_dirty(object->_parent,
                    NTG_OBJECT_DIRTY_VMEASURE |
                    NTG_OBJECT_DIRTY_VCONSTRAIN);
        }

        return true;
    }
    return false;
}

static bool set_hsize_helper(ntg_object* object, size_t size)
{
    if(object->_size.x != size)
    {
        object->_size.x = size;
        ntg_object_mark_dirty(object, 
                NTG_OBJECT_DIRTY_HCONSTRAIN |
                NTG_OBJECT_DIRTY_VMEASURE |
                NTG_OBJECT_DIRTY_VCONSTRAIN |
                NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
        object->__skip_hborder = false;
        object->__skip_hpadding = false;
        return true;
    }
    else return false;
}

static bool set_vsize_helper(ntg_object* object, size_t size)
{
    if(object->_size.y != size)
    {
        object->_size.y = size;

        bool hborder_missing = 
            (ntg_insets_hsum(object->_border_opts.pref_size) > 0) &&
            (ntg_insets_hsum(object->_border_size) == 0);
        bool hpadding_missing = 
            (ntg_insets_hsum(object->_padding_opts.pref_size) > 0) &&
            (ntg_insets_hsum(object->_padding_size) == 0);

        if(hborder_missing || hpadding_missing)
        {
            object->__skip_hborder = false;
            object->__skip_hpadding = false;
            object->__special_repeat = true;
            ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN | NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
        }
        else
        {
            ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN | NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
        }
        return true;
    }
    else return false;
}

static void mark_dirty_finalize(ntg_object* object, void* _)
{
    (void)_;

    if(object)
        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
}

NTG_OBJECT_DEF_TRAVERSE_PREORDER(mark_dirty_finalize_tree, mark_dirty_finalize)

static inline bool set_pos_helper(ntg_object* object, struct ntg_xy pos)
{
    if(!ntg_xy_are_eql(object->_pos, pos))
    {
        object->_pos = pos;
        /* Mark dirty whole subtree because abs_pos might have changed */
        mark_dirty_finalize_tree(object, NULL);
        return true;
    }
    else return false;
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

static int get_dcr_size(
        enum ntg_object_dcr_enable enable,
        size_t pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t out_size[2])
{
    size_t extra;

    if(enable == NTG_OBJECT_DCR_ENABLE_MIN)
        extra = _sub2_size(size, inner_measure.min_size);
    else if(enable == NTG_OBJECT_DCR_ENABLE_NAT)
        extra = _sub2_size(size, inner_measure.nat_size);
    else extra = size;

    out_size[0] = 0;
    out_size[1] = 0;
    double scratch_buffer[2];
    return ntg_sap_cap_round_robin(
            pref_size, NULL, extra, 2, scratch_buffer, out_size, NULL);
}

static int calculate_border_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->_border_opts.pref_size.w;
    we_pref_size[1] = object->_border_opts.pref_size.e;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            object->_border_opts.enable,
            we_pref_size,
            object->_size.x,
            ntg_object_get_measure_pad(object, NTG_ORIENT_H),
            _sizes);
    if(status != 0)
        return status;

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
    return 0;
}

static int vconstrain_border(ntg_object* object, bool* out_repeat)
{
    struct ntg_insets pref_border_size = object->_border_opts.pref_size;

    bool hborder_missing = 
            (ntg_insets_hsum(pref_border_size) > 0) &&
            (ntg_insets_hsum(object->_border_size) == 0);

    if(!out_repeat)
        return NTG_ERR_INV_ARG;
    *out_repeat = false;

    size_t n = 0, s = 0;
    int status = calculate_border_vsize(object, &n, &s);
    if(status != 0)
        return status;

    object->_border_size.n = n;
    object->_border_size.s = s;

    bool vborder_missing = 
            (ntg_insets_vsum(pref_border_size) > 0) &&
            (ntg_insets_vsum(object->_border_size) == 0);

    if(hborder_missing)
    {
        if(object->__skip_hborder) // Missing cuz skipped
        {
            // object->__skip_hborder = false;
            if(vborder_missing) // Vborder missing
                return 0;
            else // Vborder present
            {
                object->_border_size.n = 0;
                object->_border_size.s = 0;
                return 0;
            }
        }
        else
        {
            if(vborder_missing) // Vborder missing
                return 0;
            else // Vborder present
            {
                object->_border_size.n = 0;
                object->_border_size.s = 0;
                if(object->__special_repeat)
                {
                    object->__special_repeat = false;
                    *out_repeat = true;
                    return 0;
                }
                else
                    return 0;
            }
        }
    }
    else
    {
        if(object->__skip_hborder) // Skipped but not missing???
        {
            assert(0);
        }
        else
        {
            if(vborder_missing)
            {
                object->__skip_hborder = true;
                *out_repeat = true;
                    return 0;
            }
            else
                return 0;
        }
    }
}

static int vconstrain_padding(ntg_object* object, bool* out_repeat)
{
    struct ntg_insets pref_padding_size = object->_padding_opts.pref_size;

    bool hpadding_missing = 
            (ntg_insets_hsum(pref_padding_size) > 0) &&
            (ntg_insets_hsum(object->_padding_size) == 0);

    if(!out_repeat)
        return NTG_ERR_INV_ARG;
    *out_repeat = false;

    size_t n = 0, s = 0;
    int status = calculate_padding_vsize(object, &n, &s);
    if(status != 0)
        return status;

    object->_padding_size.n = n;
    object->_padding_size.s = s;

    bool vpadding_missing = 
            (ntg_insets_vsum(pref_padding_size) > 0) &&
            (ntg_insets_vsum(object->_padding_size) == 0);

    if(hpadding_missing)
    {
        if(object->__skip_hpadding) // Missing cuz skipped
        {
            // object->__skip_hpadding = false;
            if(vpadding_missing) // Vpadding missing
                return 0;
            else // Vpadding present
            {
                object->_padding_size.n = 0;
                object->_padding_size.s = 0;
                return 0;
            }
        }
        else
        {
            if(vpadding_missing) // Vpadding missing
                return 0;
            else // Vpadding present
            {
                object->_padding_size.n = 0;
                object->_padding_size.s = 0;
                if(object->__special_repeat)
                {
                    object->__special_repeat = false;
                    *out_repeat = true;
                    return 0;
                }
                else
                    return 0;
            }
        }
    }
    else
    {
        if(object->__skip_hpadding) // Skipped but not missing???
        {
            assert(0);
        }
        else
        {
            if(vpadding_missing)
            {
                object->__skip_hpadding = true;
                *out_repeat = true;
                    return 0;
            }
            else
                return 0;
        }
    }
}

static int calculate_border_vsize(
        ntg_object* object, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->_border_opts.pref_size.n;
    ns_pref_size[1] = object->_border_opts.pref_size.s;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            object->_border_opts.enable,
            ns_pref_size, 
            object->_size.y,
            ntg_object_get_measure_pad(object, NTG_ORIENT_V),
            _sizes);
    if(status != 0)
        return status;

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
    return 0;
}

static int calculate_padding_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->_padding_opts.pref_size.w;
    we_pref_size[1] = object->_padding_opts.pref_size.e;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            object->_padding_opts.enable,
            we_pref_size,  
            ntg_object_get_size_pad(object).x,
            ntg_object_get_measure_cont(object, NTG_ORIENT_H),
            _sizes);
    if(status != 0)
        return status;

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
    return 0;
}

static int calculate_padding_vsize(
        ntg_object* object, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->_padding_opts.pref_size.n;
    ns_pref_size[1] = object->_padding_opts.pref_size.s;

    size_t _sizes[2] = {0, 0};

     int status = get_dcr_size(
             object->_padding_opts.enable,
             ns_pref_size,  
             ntg_object_get_size_pad(object).y,
             ntg_object_get_measure_cont(object, NTG_ORIENT_V),
             _sizes);
    if(status != 0)
        return status;

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
    return 0;
}
static int
draw_optimized(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = ntg_object_drawing_get_size(&object->_drawing);

    struct ntg_vcell bg = object->__base_bg;
    struct ntg_insets psize = object->_padding_size;

    struct ntg_object_tmp_drawing content_drawing;
    int _status = tmp_drawing_init(&content_drawing, content_size, bg, arena);
    if(_status != 0)
        return _status;

    if(object->__vtable->draw_fn)
    {
        _status = object->__vtable->draw_fn(
                object,
                object->layout_dt,
                &content_drawing,
                arena,
                relayout);
        if(_status != 0)
            return _status;
    }

    struct ntg_vcell it_src_cell;

    struct ntg_xy offset = ntg_xy(psize.w, psize.n);

    size_t i, j;
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
    return 0;
}

static int 
draw_unoptimized(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = ntg_object_drawing_get_size(&object->_drawing);

    struct ntg_vcell bg = object->__base_bg;
    struct ntg_insets bsize = object->_border_size;
    struct ntg_insets psize = object->_padding_size;
    const struct ntg_border_style* border_style = object->_border_opts.style;

    struct ntg_object_tmp_drawing content_drawing;
    int _status = tmp_drawing_init(&content_drawing, content_size, bg, arena);
    if(_status != 0)
        return _status;

    struct ntg_object_tmp_drawing object_drawing;
    _status = tmp_drawing_init(&object_drawing, object_size, bg, arena);
    if(_status != 0)
        return _status;

    _ntg_border_style_draw(border_style, object_size, bsize, &object_drawing);

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
        _status = object->__vtable->draw_fn(
                object,
                object->layout_dt,
                &content_drawing,
                arena,
                relayout);
        if(_status != 0)
            return _status;
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

    return 0;
}
