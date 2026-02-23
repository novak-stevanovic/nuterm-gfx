#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

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

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* LAYOUT OBJECT INIT */
/* -------------------------------------------------------------------------- */

static void size_map_init(
        ntg_object_size_map* map,
        const ntg_object_vec* children,
        sarena* arena);

static void pos_map_init(
        ntg_object_pos_map* map,
        const ntg_object_vec* children,
        sarena* arena);

static void tmp_drawing_init(
        struct ntg_object_tmp_drawing* drawing,
        struct ntg_xy size,
        struct ntg_vcell def_bg,
        sarena* arena);

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

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
/* PUBLIC - TYPES (ntg_object.h) */
/* ========================================================================== */

struct ntg_border_opts ntg_border_opts_def()
{
    return (struct ntg_border_opts) {
        .style = ntg_border_style_def(),
        .pref_size = ntg_insets(0, 0, 0, 0),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN
    };
}

struct ntg_padding_opts ntg_padding_opts_def()
{
    return (struct ntg_padding_opts) {
        .pref_size = ntg_insets(0, 0, 0, 0),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN
    };
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS (ntg_object.h) */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* OBJECT TREE */
/* -------------------------------------------------------------------------- */

const ntg_object* ntg_object_get_root(const ntg_object* object)
{
    assert(object);
    while(object->_parent) object = object->_parent;
    return object;
}

ntg_object* ntg_object_get_root_(ntg_object* object)
{
    assert(object != NULL);

    return (ntg_object*)ntg_object_get_root(object);
}

ntg_scene* ntg_object_get_scene_(ntg_object* object)
{
    assert(object);

    ntg_object* root = ntg_object_get_root_(object);

    return root->__scene;
}

const ntg_scene* ntg_object_get_scene(const ntg_object* object)
{
    assert(object);

    const ntg_object* root = ntg_object_get_root(object);

    return root->__scene;
}

bool ntg_object_is_descendant(
        const ntg_object* object,
        const ntg_object* descendant)
{
    assert(object != NULL);
    assert(descendant != NULL);

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
    return ((object == descendant) ||
    ntg_object_is_descendant(object, descendant));
}

static void count_fn(ntg_object* object, void* _counter)
{
    if(!object) return;

    size_t* counter = _counter;
    (*counter)++;
}

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(count_tree, count_fn);

size_t ntg_object_get_tree_size(const ntg_object* root)
{
    assert(root);

    size_t counter = 0;
    count_tree((ntg_object*)root, &counter);

    return counter;
}

size_t ntg_object_get_children_by_z(
        const ntg_object* object,
        ntg_object** out_buff,
        size_t cap)
{
    assert(object);

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
                if((out_buff[j])->_z_index < (out_buff[i])->_z_index)
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
        struct ntg_xy* out_local_pos)
{
    assert(object);

    if(!ntg_xy_is_in_rectagle(pos, ntg_xy(0, 0), object->_size))
    {
        if(out_local_pos) *out_local_pos = ntg_xy(0, 0);
        return NULL;
    }

    int curr_z = INT_MIN;
    ntg_object* best_obj = object;
    if(out_local_pos) *out_local_pos = pos;

    size_t i;
    struct ntg_xy it_pos, it_child_local;
    ntg_object* it_child;
    ntg_object* it_hit;
    for(i = 0; i < object->_children.size; i++)
    {
        it_child = object->_children.data[i];
        it_pos = ntg_xy_sub(pos, it_child->_pos);
        it_hit = ntg_object_hit_test(it_child, it_pos, &it_child_local);

        if(it_hit && (it_child->_z_index > curr_z))
        {
            best_obj = it_hit;
            curr_z = it_child->_z_index;
            if(out_local_pos) *out_local_pos = it_child_local;
        }
    }

    return best_obj;
}

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

void ntg_object_set_user_min_size_cont(ntg_object* object, struct ntg_xy size)
{
    assert(object);

    object->_user_min_size_cont = size;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_MEASURE);
}

void ntg_object_set_user_max_size_cont(ntg_object* object, struct ntg_xy size)
{
    assert(object);

    object->_user_max_size_cont = size;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_MEASURE);
}

void ntg_object_set_user_grow(ntg_object* object, struct ntg_xy grow)
{
    object->_user_grow = grow;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_MEASURE);
}

void ntg_object_set_z_index(ntg_object* object, int z_index)
{
    assert(object != NULL);

    object->_z_index = z_index;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_RENDER);
}

void ntg_object_set_def_bg(ntg_object* object, struct ntg_vcell def_bg)
{
    assert(object != NULL);

    object->_def_bg = def_bg;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW);
}

void ntg_object_set_border_opts(
        ntg_object* object,
        const struct ntg_border_opts* opts)
{
    assert(object != NULL);

    object->_border.opts = (opts ? (*opts) : ntg_border_opts_def()); 
    if(!object->_border.opts.style)
        object->_border.opts.style = ntg_border_style_def();

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
}

void ntg_object_set_padding_opts(
        ntg_object* object,
        const struct ntg_padding_opts* opts)
{
    assert(object != NULL);

    object->_padding.opts = (opts ? (*opts) : ntg_padding_opts_def()); 

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
}

/* -------------------------------------------------------------------------- */
/* SPACE MAPPING */
/* -------------------------------------------------------------------------- */

struct ntg_dxy ntg_object_map_to_ancestor(
        const ntg_object* object,
        const ntg_object* ancestor,
        struct ntg_dxy point)
{
    assert(object != NULL);

    if(object == ancestor)
        return point;

    struct ntg_dxy out = point;

    const ntg_object* it = object;
    while(it != NULL && it != ancestor)
    {
        out = ntg_dxy_add(out, ntg_dxy_from_xy(it->_pos));
        it = it->_parent;
    }

    /* If ancestor is non-NULL, it must be reached (must be an ancestor). */
    if(ancestor != NULL)
        assert(it == ancestor);

    return out;
}

struct ntg_dxy ntg_object_map_to_descendant(
        const ntg_object* object,
        const ntg_object* descendant,
        struct ntg_dxy point)
{
    assert(descendant != NULL);

    if(object == descendant)
        return point;

    struct ntg_dxy desc_pos = ntg_object_map_to_ancestor(
            descendant, object, ntg_dxy(0, 0));

    return ntg_dxy_sub(point, desc_pos);
}

struct ntg_dxy 
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point)
{
    assert(object);

    return ntg_object_map_to_ancestor(object, NULL, point);
}

struct ntg_dxy 
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point)
{
    assert(object);

    return ntg_object_map_to_descendant(NULL, object, point);
}

/* ========================================================================== */
/* PROTECTED (ntg_object.h) */
/* ========================================================================== */

static void init_default(ntg_object* object)
{
    (*object) = (ntg_object) {0};

    object->_def_bg = ntg_vcell_default();
    object->_user_min_size_cont = ntg_xy(
            NTG_OBJECT_MIN_SIZE_UNSET,
            NTG_OBJECT_MIN_SIZE_UNSET);
    object->_user_max_size_cont = ntg_xy(
            NTG_OBJECT_MAX_SIZE_UNSET,
            NTG_OBJECT_MAX_SIZE_UNSET);
    object->_user_grow = ntg_xy(
            NTG_OBJECT_GROW_UNSET,
            NTG_OBJECT_GROW_UNSET);

    object->_border.opts = ntg_border_opts_def();
    object->_padding.opts = ntg_padding_opts_def();
}

#define LAYOUT_OPS_DEF (struct ntg_object_layout_ops){0}
#define HOOKS_DEF (struct ntg_object_hooks){0}

void ntg_object_init(
        ntg_object* object,
        const struct ntg_object_layout_ops* layout_ops,
        const struct ntg_object_hooks* hooks,
        const ntg_type* type)
{
    assert(object != NULL);
    assert(ntg_type_instance_of(type, &NTG_TYPE_OBJECT));

    init_default(object);

    object->_type = type;

    ntg_object_vec_init(&object->_children, 2, NULL);

    object->__layout_ops = (layout_ops ? (*layout_ops) : LAYOUT_OPS_DEF);
    object->__hooks = (hooks ? (*hooks) : HOOKS_DEF);

    ntg_object_drawing_init(&object->_drawing);
}

void ntg_object_deinit(ntg_object* object)
{
    assert(object);

    // Root AND is part of scene scene
    if(object->__scene)
    {
        ntg_scene_detach_root(object->__scene, object);
    }

    if(object->_parent)
    {
        ntg_object_detach(object);
    }

    while(object->_children.size > 0)
    {
        ntg_object_detach(object->_children.data[0]);
    }

    ntg_object_vec_deinit(&object->_children, NULL);
    ntg_object_drawing_deinit(&object->_drawing);

    init_default(object);
}

void ntg_object_attach(ntg_object* parent, ntg_object* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);

    if(child->_parent != NULL)
        ntg_object_detach(child);

    if(child->__scene)
        ntg_scene_detach_root(child->__scene, child);

    ntg_scene* scene = ntg_object_get_scene_(parent);

    ntg_object_vec_pushb(&parent->_children, child, NULL);

    child->_parent = parent;

    if(scene)
        _ntg_scene_register_tree(scene, child);
}

void ntg_object_detach(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* parent = object->_parent;
    if(parent == NULL) return;

    ntg_scene* scene = ntg_object_get_scene_(object);

    ntg_object_vec_rm(&parent->_children, object, NULL);

    object->_parent = NULL;

    if(parent->__hooks.on_child_rm_fn)
        parent->__hooks.on_child_rm_fn(parent, object);

    if(scene)
        _ntg_scene_unregister_tree(scene, object);
}

/* ========================================================================== */
/* INTERNAL (ntg_object.h) */
/* ========================================================================== */

void _ntg_object_root_set_scene(ntg_object* object, ntg_scene* scene)
{
    assert(object);

    object->__scene = scene;
}

/* ========================================================================== */
/* PUBLIC - TYPES (ntg_object_layout.h) */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* MEASURE PHASE */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* CONSTRAIN PHASE */
/* -------------------------------------------------------------------------- */

size_t ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object)
{
    assert(map);
    assert(object);

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
            return map->vals[i];
    }

    assert(0);

    return 0;
}

void ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size)
{
    assert(map);
    assert(object);

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

/* -------------------------------------------------------------------------- */
/* ARRANGE PHASE */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_object_pos_map_get(
        const ntg_object_pos_map* map,
        const ntg_object* object)
{
    assert(map);
    assert(object);

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
            return map->vals[i];
    }

    assert(0);

    return ntg_xy(0, 0);
}

void ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos)
{
    assert(map);
    assert(object);

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

/* -------------------------------------------------------------------------- */
/* DRAW PHASE */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */
/* PUBLIC - FUNCTIONS (ntg_object_layout.h) */
/* ========================================================================== */

void ntg_object_mark_dirty(ntg_object* object, uint8_t dirty)
{
    assert(object);

    object->_dirty |= dirty;

    ntg_scene* scene = ntg_object_get_scene_(object);
    if(scene)
        ntg_scene_mark_dirty(scene);
}

/* -------------------------------------------------------------------------- */
/* MEASURE & SIZE HELPERS */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_object_get_min_size(const ntg_object* object)
{
    return object->_min_size;
}

struct ntg_xy ntg_object_get_nat_size(const ntg_object* object)
{
    return object->_nat_size;
}

struct ntg_xy ntg_object_get_max_size(const ntg_object* object)
{
    return object->_max_size;
}

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    return object->_size;
}

struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient)
{
    return (struct ntg_object_measure) {
        .min_size = ntg_xy_get(object->_min_size, orient),
        .nat_size = ntg_xy_get(object->_nat_size, orient),
        .max_size = ntg_xy_get(object->_max_size, orient),
        .grow = ntg_xy_get(object->_grow, orient)
    };
}

size_t ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient)
{
    return ntg_xy_get(object->_size, orient);
}

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object)
{
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
    size_t s = ntg_xy_get(object->_size, orient);

    struct ntg_insets border_size = object->_border.size;
    struct ntg_insets padding_size = object->_padding.size;

    size_t sub = ntg_insets_sum(border_size, orient) +
            ntg_insets_sum(padding_size, orient);

    return _ssub_size(s, sub);
}

size_t ntg_object_get_for_size_cont(const ntg_object* object, ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ?
            NTG_SIZE_MAX :
            ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);
}

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object)
{
    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object)
{
    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size),
        ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object)
{
    struct ntg_insets pref_border_size = object->_border.opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->_max_size, sub);
}

struct ntg_xy ntg_object_get_size_pad(const ntg_object* object)
{
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
    size_t s = ntg_xy_get(object->_size, orient);

    struct ntg_insets border_size = object->_border.size;

    size_t sub = ntg_insets_sum(border_size, orient);

    return _ssub_size(s, sub);
}

/* ========================================================================== */
/* PROTECTED (ntg_object_layout.h) */
/* ========================================================================== */

/* ========================================================================== */
/* INTERNAL (ntg_object_layout.h) */
/* ========================================================================== */

void _ntg_object_hmeasure(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.measure_fn)
    {
        measure = object->__layout_ops.measure_fn(object,
                NTG_ORIENT_H, object->layout_cache, arena);

        size_t extra = ntg_insets_hsum(object->_padding.opts.pref_size) +
                ntg_insets_hsum(object->_border.opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->_user_min_size_cont.x + extra,
                object->_user_max_size_cont.x + extra,
                object->_user_grow.x);
    }

    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_H);

    if(!ntg_object_measure_are_equal(measure, old))
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

void _ntg_object_hconstrain(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

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

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);

    if(content_size == 0)
    {
        for(i = 0; i < object->_children.size; i++)
        {
            object->_children.data[i]->_size.x = 0;
        }

        return;
    }

    ntg_object_size_map map;
    size_map_init(&map, &object->_children, arena);

    if(object->__layout_ops.constrain_fn)
    {
        object->__layout_ops.constrain_fn(object, NTG_ORIENT_H,
                &map, object->layout_cache, arena);
    }

    ntg_object* it_child;
    size_t it_size;
    size_t it_old_size;
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

void _ntg_object_vmeasure(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.measure_fn)
    {
        measure = object->__layout_ops.measure_fn(object,
                NTG_ORIENT_V, object->layout_cache, arena);

        size_t extra = ntg_insets_vsum(object->_padding.opts.pref_size) +
                ntg_insets_vsum(object->_border.opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->_user_min_size_cont.y + extra,
                object->_user_max_size_cont.y + extra,
                object->_user_grow.y);
    }

    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_V);

    if(!ntg_object_measure_are_equal(measure, old))
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

void _ntg_object_vconstrain(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_ARRANGE | NTG_OBJECT_DIRTY_DRAW);
    bool ret = vconstrain_border(object, arena) || vconstrain_padding(object, arena);
    if(ret) return;

    if(object->_children.size == 0) return;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);

    if(content_size == 0)
    {
        for(i = 0; i < object->_children.size; i++)
        {
            object->_children.data[i]->_size.y = 0;
        }

        return;
    }

    ntg_object_size_map map;
    size_map_init(&map, &object->_children, arena);

    if(object->__layout_ops.constrain_fn)
    {
        object->__layout_ops.constrain_fn(object, NTG_ORIENT_V,
                &map, object->layout_cache, arena);
    }

    ntg_object* it_child;
    size_t it_size;
    size_t it_old_size;
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
    assert(object);
    assert(arena);

    bool repeat_dcr = object->__repeat;
    bool repeat_fn = false;
    if(object->__layout_ops.fixup_fn)
    {
        repeat_fn = object->__layout_ops.fixup_fn(object,
                object->layout_cache, arena);;
    }

    if(repeat_dcr || repeat_fn)
    {
        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_HCONSTRAIN);
    }

    object->__repeat = false;

    return (repeat_dcr || repeat_fn);
}

void _ntg_object_arrange(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    if(object->_children.size == 0) return;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);

    if(content_size == 0)
    {
        for(i = 0; i < object->_children.size; i++)
        {
            object->_children.data[i]->_pos = ntg_xy(0, 0);
        }
        return;
    }

    ntg_object_pos_map map;
    pos_map_init(&map, &object->_children, arena);

    if(object->__layout_ops.arrange_fn)
        object->__layout_ops.arrange_fn(object, &map, object->layout_cache, arena);

    struct ntg_xy dcr_sum = ntg_xy(
            object->_border.size.w + object->_padding.size.w,
            object->_border.size.n + object->_padding.size.n);

    ntg_object* it_child;
    struct ntg_xy it_pos;
    struct ntg_xy it_old_pos;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_pos = map.vals[i];
        it_old_pos = it_child->_pos;

        it_pos = ntg_xy_add(it_pos, dcr_sum);

        it_pos = ntg_xy_pos_clamp(it_pos, it_child->_size, object->_size);

        it_child->_pos.x = it_pos.x;
        it_child->_pos.y = it_pos.y;
    }
}

void _ntg_object_draw(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    // Set object drawing size
    
    const ntg_scene* scene = ntg_object_get_scene_(object);
    ntg_object_drawing_set_size(&object->_drawing, object->_size, scene->_size);

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
    assert(object);
    assert(!object->_parent);

    size_t old = object->_size.x;

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
    assert(object);
    assert(!object->_parent);

    size_t old = object->_size.y;

    if(old != size)
    {
        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN);

        object->_size.y = size;
    }
}

void _ntg_object_root_set_pos(ntg_object* object, struct ntg_xy pos)
{
    assert(object);
    assert(!object->_parent);

    object->_pos = pos;
}

void _ntg_object_clean(ntg_object* object, uint8_t dirty)
{
    object->_dirty &= ~dirty;
}

void _ntg_object_on_scene_change(ntg_object* object, ntg_scene* scene)
{
    object->__skip_hborder = false;
    object->__skip_hpadding = false;
    object->__repeat = false;
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* LAYOUT OBJECT INIT */
/* -------------------------------------------------------------------------- */

static void size_map_init(
        ntg_object_size_map* map,
        const ntg_object_vec* children,
        sarena* arena)
{
    if(children->size == 0)
    {
        map->keys = NULL;
        map->vals = NULL;
        map->size = 0;
        return;
    }

    map->size = children->size;
    map->keys = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
    assert(map->keys);
    map->vals = sarena_malloc(arena, sizeof(size_t) * children->size);
    assert(map->vals);
    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = 0;
    }
}

static void pos_map_init(
        ntg_object_pos_map* map,
        const ntg_object_vec* children,
        sarena* arena)
{
    if(children->size == 0)
    {
        map->keys = NULL;
        map->vals = NULL;
        map->size = 0;
        return;
    }

    map->size = children->size;
    map->keys = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
    assert(map->keys);
    map->vals = sarena_malloc(arena, sizeof(struct ntg_xy) * children->size);
    assert(map->vals);
    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = ntg_xy(0, 0);
    }
}

void tmp_drawing_init(
        struct ntg_object_tmp_drawing* drawing,
        struct ntg_xy size,
        struct ntg_vcell def_bg,
        sarena* arena)
{
    drawing->data = sarena_malloc(arena, sizeof(struct ntg_vcell) *
            size.x * size.y + 1);
    assert(drawing->data);
    drawing->size = size;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(drawing, def_bg, ntg_xy(j, i));
        }
    }
}

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

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

    // inner_measure.min_size += pref_size[0] + pref_size[1];
    // inner_measure.nat_size += pref_size[0] + pref_size[1];
    // inner_measure.min_size += pref_size[0] + pref_size[1];

    if(enable == NTG_OBJECT_DCR_ENABLE_MIN)
        extra = _ssub_size(size, inner_measure.min_size);
    else if(enable == NTG_OBJECT_DCR_ENABLE_NAT)
        extra = _ssub_size(size, inner_measure.nat_size);
    else extra = size;
    
    out_size[0] = 0;
    out_size[1] = 0;
    ntg_sap_cap_round_robin(pref_size, NULL, out_size, extra, 2, arena);
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
        // ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_HCONSTRAIN);
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
    else if(hborder_missing && vborder_missing) // both are missing
    {
        object->_border.size.n = 0;
        object->_border.size.s = 0;
    }
    else // neither
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
        // ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_HCONSTRAIN);
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
    else if(hpadding_missing && vpadding_missing) // both are missing
    {
        object->_padding.size.n = 0;
        object->_padding.size.s = 0;
    }
    else // neither
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
    struct ntg_xy object_size = object->_size;

    struct ntg_vcell bg = object->_def_bg;
    struct ntg_insets psize = object->_padding.size;

    struct ntg_object_tmp_drawing content_drawing;
    tmp_drawing_init(&content_drawing, content_size, bg, arena);

    size_t i, j;

    // Draw object content
    if(object->__layout_ops.draw_fn)
    {
        object->__layout_ops.draw_fn(object, &content_drawing, object->layout_cache, arena);
    }

    struct ntg_vcell it_src_cell;

    struct ntg_xy offset = ntg_xy(psize.w, psize.n); 
    struct ntg_xy ji;

    // Update object's actual drawing

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
    struct ntg_xy object_size = object->_size;

    struct ntg_vcell bg = object->_def_bg;
    struct ntg_insets bsize = object->_border.size;
    struct ntg_insets psize = object->_padding.size;
    const struct ntg_border_style* border_style = object->_border.opts.style;

    struct ntg_object_tmp_drawing content_drawing;
    tmp_drawing_init(&content_drawing, content_size, bg, arena);

    struct ntg_object_tmp_drawing object_drawing;
    tmp_drawing_init(&object_drawing, object_size, bg, arena);

    // Draw border
    if(border_style->draw_fn)
    {
        border_style->draw_fn(border_style->data, object_size, bsize, &object_drawing);
    }

    size_t i, j;

    // Constrain border to its size
    for(i = bsize.n; i < (object_size.y - bsize.s); i++)
    {
        for(j = bsize.w; j < (object_size.x - bsize.e); j++)
        {
            ntg_object_tmp_drawing_set(&object_drawing, bg, ntg_xy(j, i));
        }
    }

    // Draw object content
    if(object->__layout_ops.draw_fn)
    {
        object->__layout_ops.draw_fn(object, &content_drawing, object->layout_cache, arena);
    }

    struct ntg_vcell it_src_cell;

    // Place content drawing onto border drawing

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

    // Update object's actual drawing

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
