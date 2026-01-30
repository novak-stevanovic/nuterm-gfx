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

struct ntg_object_tmp_drawing
{
    struct ntg_vcell* data;
    struct ntg_xy size;
};

/* ========================================================================== */
/* STATIC - FUNCTION DECLARATIONS */
/* ========================================================================== */

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

static struct ntg_object_measure incorporate_user_measure(
        struct ntg_object_measure measure,
        size_t user_min_size,
        size_t user_max_size,
        size_t user_grow);
    
static void get_dcr_hsize(
        ntg_object_dcr_enable enable,
        size_t we_pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t we_out_size[2],
        sarena* arena);

static void get_dcr_vsize(
        size_t ns_pref_size[2],
        size_t size,
        size_t pref_hsize,
        size_t hsize,
        size_t ns_out_size[2],
        sarena* arena);

static void update_border_hsize(ntg_object* object, sarena* arena);
static void update_border_vsize(ntg_object* object, sarena* arena);
static void update_padding_hsize(ntg_object* object, sarena* arena);
static void update_padding_vsize(ntg_object* object, sarena* arena);

/* ========================================================================== */
/* PUBLIC - TYPES (ntg_object.h) */
/* ========================================================================== */

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

ntg_scene_layer* ntg_object_get_scene_layer_(ntg_object* object)
{
    assert(object);

    ntg_object* root = ntg_object_get_root_(object);

    return root->__scene_layer;
}

const ntg_scene_layer* ntg_object_get_scene_layer(const ntg_object* object)
{
    assert(object);

    const ntg_object* root = ntg_object_get_root(object);

    return root->__scene_layer;
}

ntg_scene* ntg_object_get_scene_(ntg_object* object)
{
    assert(object);

    ntg_scene_layer* layer = ntg_object_get_scene_layer_(object);

    return layer ? layer->_scene : NULL;
}

const ntg_scene* ntg_object_get_scene(const ntg_object* object)
{
    assert(object);

    const ntg_scene_layer* layer = ntg_object_get_scene_layer(object);

    return layer ? layer->_scene : NULL;
}

bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant)
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

bool ntg_object_is_descendant_eq(const ntg_object* object, const ntg_object* descendant)
{
    return ((object == descendant) || ntg_object_is_descendant(object, descendant));
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

void ntg_object_set_user_min_size(ntg_object* object, struct ntg_xy size)
{
    assert(object);

    object->_user_min_size = size;

    object->dirty |= NTG_OBJECT_DIRTY_MEASURE;
}

void ntg_object_set_user_max_size(ntg_object* object, struct ntg_xy size)
{
    assert(object);

    object->_user_max_size = size;

    object->dirty |= NTG_OBJECT_DIRTY_MEASURE;
}

void ntg_object_set_user_grow(ntg_object* object, struct ntg_xy grow)
{
    object->_user_grow = grow;

    object->dirty |= NTG_OBJECT_DIRTY_MEASURE;
}

void ntg_object_set_z_index(ntg_object* object, int z_index)
{
    assert(object != NULL);

    object->_z_index = z_index;

    object->dirty |= NTG_OBJECT_DIRTY_DRAW;
}

void ntg_object_set_def_bg(ntg_object* object, struct ntg_vcell def_bg)
{
    assert(object != NULL);

    object->_def_bg = def_bg;

    object->dirty |= NTG_OBJECT_DIRTY_DRAW;
}

void ntg_object_set_border_opts(
        ntg_object* object,
        struct ntg_object_border_opts opts)
{
    assert(object != NULL);

    object->_border.opts = opts;

    object->dirty |= NTG_OBJECT_DIRTY_FULL;
}

void ntg_object_set_border_style(
        ntg_object* object,
        struct ntg_object_border_style style)
{
    assert(object != NULL);

    object->_border.style = style;

    object->dirty |= NTG_OBJECT_DIRTY_DRAW;
}

void ntg_object_set_padding_opts(
        ntg_object* object,
        struct ntg_object_padding_opts opts)
{
    assert(object != NULL);

    object->_padding.opts = opts;

    object->dirty |= NTG_OBJECT_DIRTY_FULL;
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

struct ntg_dxy ntg_object_map_to_scene_layer(
        const ntg_object* object,
        struct ntg_dxy point)
{
    assert(object != NULL);

    return ntg_object_map_to_ancestor(object, NULL, point);
}

struct ntg_dxy ntg_object_map_from_scene_layer(
        const ntg_object* object,
        struct ntg_dxy point)
{
    assert(object != NULL);

    return ntg_object_map_to_descendant(NULL, object, point);
}

struct ntg_dxy ntg_object_map_to_scene(
        const ntg_object* object,
        struct ntg_dxy point)
{
    assert(object);

    const ntg_scene_layer* layer = ntg_object_get_scene_layer(object);

    if((!layer) || (!layer->_scene))
        return ntg_dxy(0, 0);

    struct ntg_dxy layer_pos = ntg_object_map_to_scene_layer(object, point);

    struct ntg_dxy scene_pos = ntg_scene_layer_map_to_scene(layer, layer_pos);

    return scene_pos;
}

struct ntg_dxy ntg_object_map_from_scene(
        const ntg_object* object,
        struct ntg_dxy point)
{
    assert(object);

    const ntg_scene_layer* layer = ntg_object_get_scene_layer(object);

    if((!layer) || (!layer->_scene))
        return ntg_dxy(0, 0);

    struct ntg_dxy layer_pos = ntg_scene_layer_map_from_scene(
            layer, point);

    struct ntg_dxy object_pos = ntg_object_map_from_scene_layer(
            object, layer_pos);

    return object_pos;
}

/* ========================================================================== */
/* PROTECTED (ntg_object.h) */
/* ========================================================================== */

static void init_default(ntg_object* object)
{
    (*object) = (ntg_object) {0};
}

void ntg_object_init(
        ntg_object* object,
        struct ntg_object_layout_ops layout_ops,
        const ntg_type* type)
{
    assert(object != NULL);
    assert(ntg_type_instance_of(type, &NTG_TYPE_OBJECT));

    init_default(object);

    object->_type = type;

    ntg_object_vec_init(&object->_children, 2, NULL);

    object->__layout_ops = layout_ops;
    ntg_object_drawing_init(&object->_drawing);

    object->_def_bg = ntg_vcell_default();
    object->_user_min_size = ntg_xy(
            NTG_OBJECT_MIN_SIZE_UNSET,
            NTG_OBJECT_MIN_SIZE_UNSET);
    object->_user_max_size = ntg_xy(
            NTG_OBJECT_MAX_SIZE_UNSET,
            NTG_OBJECT_MAX_SIZE_UNSET);
    object->_user_grow = ntg_xy(
            NTG_OBJECT_GROW_UNSET,
            NTG_OBJECT_GROW_UNSET);
}

void ntg_object_deinit(ntg_object* object)
{
    assert(object);
    assert(object->_parent == NULL);
    assert(object->_children.size == 0);
    assert(object->__scene_layer == NULL);
 
    ntg_object_vec_deinit(&object->_children, NULL);
    ntg_object_drawing_deinit(&object->_drawing);

    init_default(object);
}

void ntg_object_attach(ntg_object* parent, ntg_object* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    assert(child->_parent == NULL);

    if(child->_parent != NULL)
        ntg_object_detach(child);

    ntg_scene_layer* layer = ntg_object_get_scene_layer_(parent);

    ntg_object_vec_pushb(&parent->_children, child, NULL);

    child->_parent = parent;

    if(layer)
        _ntg_scene_layer_register_tree(layer, child);

    parent->dirty |= NTG_OBJECT_DIRTY_FULL;
}

void ntg_object_detach(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* parent = object->_parent;
    if(parent == NULL) return;

    ntg_scene_layer* layer = ntg_object_get_scene_layer_(object);

    ntg_object_vec_rm(&parent->_children, object, NULL);

    object->_parent = NULL;

    if(layer)
        _ntg_scene_layer_unregister_tree(layer, object);

    ntg_object_add_dirty((ntg_object*)parent, NTG_OBJECT_DIRTY_FULL);
}

/* ========================================================================== */
/* INTERNAL (ntg_object.h) */
/* ========================================================================== */

void _ntg_object_root_set_scene_layer(ntg_object* object, ntg_scene_layer* layer)
{
    assert(object);

    object->__scene_layer = layer;
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

struct ntg_vcell
ntg_object_tmp_drawing_get(const ntg_object_tmp_drawing* drawing, struct ntg_xy pos)
{
    assert(drawing);

    if(ntg_xy_is_lesser(pos, drawing->size))
    {
        return drawing->data[drawing->size.x * pos.y + pos.x];
    }
    else
    {
        assert(0);
        return ntg_vcell_default();
    }
}

void ntg_object_tmp_drawing_set(
        ntg_object_tmp_drawing* drawing,
        struct ntg_vcell cell,
        struct ntg_xy pos)
{
    assert(drawing);

    if(ntg_xy_is_lesser(pos, drawing->size))
    {
        drawing->data[drawing->size.x * pos.y + pos.x] = cell;
    }
    else
    {
        assert(0);
    }
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS (ntg_object_layout.h) */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* MEASURE & SIZE HELPERS */
/* -------------------------------------------------------------------------- */

struct ntg_object_measure ntg_object_get_hmeasure(const ntg_object* object);
struct ntg_object_measure ntg_object_get_vmeasure(const ntg_object* object);

/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size_cont(const ntg_object* object);
struct ntg_object_measure ntg_object_get_hmeasure_cont(const ntg_object* object);
struct ntg_object_measure ntg_object_get_vmeasure_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_size_cont(const ntg_object* object);

/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object);
struct ntg_object_measure ntg_object_get_hmeasure_pad(const ntg_object* object);
struct ntg_object_measure ntg_object_get_vmeasure_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_size_pad(const ntg_object* object);

/* ========================================================================== */
/* PROTECTED (ntg_object_layout.h) */
/* ========================================================================== */

void ntg_object_add_dirty(ntg_object* object, uint8_t dirty)
{
    assert(object != NULL);

    object->dirty |= dirty;
}

/* ========================================================================== */
/* INTERNAL (ntg_object_layout.h) */
/* ========================================================================== */

void _ntg_object_lctx_init(ntg_object* object)
{
    assert(object);

    if(object->__layout_ops.lctx_init_fn)
        object->__lctx = object->__layout_ops.lctx_init_fn(object);
    else
        object->__lctx = NULL;
}

void _ntg_object_lctx_deinit(ntg_object* object)
{
    if(object->__layout_ops.lctx_deinit_fn)
        object->__layout_ops.lctx_deinit_fn(object->__lctx, object);

    object->__lctx = NULL;
}

void _ntg_object_hmeasure(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.hmeasure_fn)
    {
        measure = object->__layout_ops.hmeasure_fn(object, object->__lctx, arena);

        size_t extra = ntg_insets_hsum(object->_padding.opts.pref_size) +
                ntg_insets_hsum(object->_border.opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->_user_min_size.x,
                object->_user_max_size.x,
                object->_user_grow.x);
    }

    struct ntg_object_measure old = ntg_object_get_hmeasure(object);

    if(!ntg_object_measure_are_equal(measure, old))
    {
        object->_min_size.x = measure.min_size;
        object->_nat_size.x = measure.nat_size;
        object->_max_size.x = measure.max_size;
        object->_grow.x = measure.grow;
        if(object->_parent)
        {
            object->_parent->dirty |= (
                    NTG_OBJECT_DIRTY_MEASURE |
                    NTG_OBJECT_DIRTY_CONSTRAIN);
        }
    }
}

void _ntg_object_hconstrain(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    ntg_object* it_obj;

    ntg_object_size_map map;
    size_map_init(&map, &object->_children, arena);

    if(object->__layout_ops.hconstrain_fn)
        object->__layout_ops.hconstrain_fn(object, &map, object->__lctx, arena);

    size_t i;
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
            it_child->dirty |= NTG_OBJECT_DIRTY_FULL;
            it_child->_size.x = it_size;
            update_border_hsize(it_child, arena);
            update_padding_hsize(it_child, arena);
        }
    }
}

void _ntg_object_vmeasure(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.vmeasure_fn)
    {
        measure = object->__layout_ops.vmeasure_fn(object, object->__lctx, arena);

        size_t extra = ntg_insets_vsum(object->_padding.opts.pref_size) +
                ntg_insets_vsum(object->_border.opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->_user_min_size.y,
                object->_user_max_size.y,
                object->_user_grow.y);
    }

    struct ntg_object_measure old = ntg_object_get_vmeasure(object);

    if(!ntg_object_measure_are_equal(measure, old))
    {
        object->_min_size.y = measure.min_size;
        object->_nat_size.y = measure.nat_size;
        object->_max_size.y = measure.max_size;
        object->_grow.y = measure.grow;

        if(object->_parent)
        {
            object->_parent->dirty |= (
                    NTG_OBJECT_DIRTY_MEASURE |
                    NTG_OBJECT_DIRTY_CONSTRAIN);
        }
    }
}

void _ntg_object_vconstrain(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    ntg_object* it_obj;

    ntg_object_size_map map;
    size_map_init(&map, &object->_children, arena);

    if(object->__layout_ops.vconstrain_fn)
        object->__layout_ops.vconstrain_fn(object, &map, object->__lctx, arena);

    size_t i;
    ntg_object* it_child;
    size_t it_size;
    size_t it_old_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = map.vals[i];
        it_old_size = it_child->_size.y;

        if(it_old_size != it_size)
        {
            it_child->dirty |= NTG_OBJECT_DIRTY_ARRANGE |
            NTG_OBJECT_DIRTY_DRAW;

            it_child->_size.y = it_size;
            update_border_vsize(it_child, arena);
            update_padding_vsize(it_child, arena);
        }
    }
}

void _ntg_object_arrange(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    ntg_object* it_obj;

    ntg_object_pos_map map;
    pos_map_init(&map, &object->_children, arena);

    if(object->__layout_ops.arrange_fn)
        object->__layout_ops.arrange_fn(object, &map, object->__lctx, arena);

    struct ntg_insets bsize = object->_border.size;
    struct ntg_insets psize = object->_padding.size;
    size_t hsum = ntg_insets_hsum(bsize) + ntg_insets_hsum(psize);
    size_t vsum = ntg_insets_vsum(bsize) + ntg_insets_vsum(psize);

    size_t i;
    ntg_object* it_child;
    struct ntg_xy it_pos;
    struct ntg_xy it_old_pos;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_pos = map.vals[i];
        it_old_pos = it_child->_pos;

        // if(!ntg_xy_are_equal(it_pos, it_old_pos)) // TODO??
          //  it_child->dirty |= NTG_OBJECT_DIRTY_DRAW;

        it_child->_pos.x = it_pos.x + hsum;
        it_child->_pos.y = it_pos.y + vsum;
    }
}

void _ntg_object_draw(ntg_object* object, sarena* arena)
{
    assert(object);
    assert(arena);

    struct ntg_vcell bg = object->_def_bg;
    struct ntg_insets bsize = object->_border.size;
    struct ntg_insets psize = object->_padding.size;

    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = object->_size;

    struct ntg_object_tmp_drawing content_drawing;
    tmp_drawing_init(&content_drawing, content_size, bg, arena);

    struct ntg_object_tmp_drawing object_drawing;
    tmp_drawing_init(&object_drawing, object_size, bg, arena);

    // Draw border
    if(object->_border.style.draw_fn)
    {
        object->_border.style.draw_fn(object_size, bsize, &object_drawing);
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
        object->__layout_ops.draw_fn(object, &content_drawing, object->__lctx, arena);
    }

    struct ntg_vcell it_src_cell;

    // Set object drawing size
    
    const ntg_scene_layer* layer = ntg_object_get_scene_layer_(object);
    ntg_object_drawing_set_size(&object->_drawing, object_size, layer->_size);

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

void _ntg_object_root_set_hsize(ntg_object* object, size_t size)
{
    assert(object);
    assert(!object->_parent);

    size_t old = object->_size.x;

    if(old != size)
    {
        object->dirty |= NTG_OBJECT_DIRTY_FULL;
    }

    object->_size.x = size;
}

void _ntg_object_root_set_vsize(ntg_object* object, size_t size)
{
    assert(object);
    assert(!object->_parent);

    size_t old = object->_size.y;

    if(old != size)
    {
        object->dirty |= NTG_OBJECT_DIRTY_CONSTRAIN |
        NTG_OBJECT_DIRTY_ARRANGE | NTG_OBJECT_DIRTY_DRAW;
    }

    object->_size.y = size;
}

/* ========================================================================== */
/* STATIC - FUNCTION DEFINITIONS */
/* ========================================================================== */

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
    drawing->data = sarena_malloc(arena, sizeof(struct ntg_vcell) * size.x * size.y);
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

static void get_dcr_hsize(
        ntg_object_dcr_enable enable,
        size_t we_pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t we_out_size[2],
        sarena* arena)
{
    size_t extra;

    if(enable == NTG_OBJECT_DCR_ENABLE_MIN)
        extra = _ssub_size(size, inner_measure.min_size);
    else if(enable == NTG_OBJECT_DCR_ENABLE_NAT)
        extra = _ssub_size(size, inner_measure.nat_size);
    else extra = size;

    ntg_sap_cap_round_robin(we_pref_size, NULL, we_out_size, extra, 2, arena);
}

static void get_dcr_vsize(
        size_t ns_pref_size[2],
        size_t size,
        size_t pref_hsize,
        size_t hsize,
        size_t ns_out_size[2],
        sarena* arena)
{
    size_t extra;

    if((pref_hsize > 0) && (hsize == 0)) // h border not drawn
        extra = 0;
    else
        extra = size;

    ntg_sap_cap_round_robin(ns_pref_size, NULL, ns_out_size, extra, 2, arena);
}

static void update_border_hsize(ntg_object* object, sarena* arena)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->_border.opts.pref_size.w;
    we_pref_size[1] = object->_border.opts.pref_size.e;
    size_t _sizes[2];

    get_dcr_hsize(object->_border.opts.enable,
            we_pref_size, 
            object->_size.x,
            ntg_object_get_hmeasure_pad(object),
            _sizes,
            arena);

    object->_border.size.w = _sizes[0];
    object->_border.size.e = _sizes[1];
}

static void update_border_vsize(ntg_object* object, sarena* arena)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->_border.opts.pref_size.n;
    ns_pref_size[1] = object->_border.opts.pref_size.s;

    size_t _sizes[2];

    size_t hsize = ntg_insets_hsum(object->_border.size);
    size_t pref_hsize = ntg_insets_hsum(object->_border.opts.pref_size);

    get_dcr_vsize(
        ns_pref_size,
        object->_size.y,
        pref_hsize,
        hsize,
        _sizes,
        arena);

    object->_border.size.n = _sizes[0];
    object->_border.size.s = _sizes[1];
}

static void update_padding_hsize(ntg_object* object, sarena* arena)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->_padding.opts.pref_size.w;
    we_pref_size[1] = object->_padding.opts.pref_size.e;
    size_t _sizes[2];

    get_dcr_hsize(object->_padding.opts.enable,
            we_pref_size,  
            ntg_object_get_size_pad(object).x,
            ntg_object_get_hmeasure_cont(object),
            _sizes,
            arena);

    object->_padding.size.w = _sizes[0];
    object->_padding.size.e = _sizes[1];
}

static void update_padding_vsize(ntg_object* object, sarena* arena)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->_padding.opts.pref_size.n;
    ns_pref_size[1] = object->_padding.opts.pref_size.s;

    size_t _sizes[2];

    size_t hsize = ntg_insets_hsum(object->_padding.size);
    size_t pref_hsize = ntg_insets_hsum(object->_padding.opts.pref_size);

    get_dcr_vsize(
        ns_pref_size,
        ntg_object_get_size_pad(object).y,
        pref_hsize,
        hsize,
        _sizes,
        arena);

    object->_padding.size.n = _sizes[0];
    object->_padding.size.s = _sizes[1];
}
