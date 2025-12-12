#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_sizes.h"
#include "core/object/shared/ntg_object_xys.h"
#include <stdlib.h>
#include <assert.h>

#define _NTG_OBJECT_DEF_
#include "core/object/ntg_object.h"
#include "base/event/ntg_event.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/object/shared/ntg_object_vec.h"

#include "nt_event.h"
#include "shared/_ntg_shared.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_object_deinit(ntg_object* object)
{
    assert(object != NULL);

    object->__deinit_fn(object);
}

/* ---------------------------------------------------------------- */
/* IDENTITY */
/* ---------------------------------------------------------------- */

unsigned int ntg_object_get_type(const ntg_object* object)
{
    assert(object != NULL);

    return object->__type;
}

bool ntg_object_is_widget(const ntg_object* object)
{
    assert(object != NULL);

    return (object->__type < NTG_OBJECT_DECOR);
}

bool ntg_object_is_decorator(const ntg_object* object)
{
    return (object->__type >= NTG_OBJECT_DECOR);
}

unsigned int ntg_object_get_id(const ntg_object* object)
{
    assert(object != NULL);

    return object->__id;
}

/* ---------------------------------------------------------------- */
/* OBJECT TREE */
/* ---------------------------------------------------------------- */

ntg_object* ntg_object_get_group_root(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(true)
    {
        if((it_obj->__parent == NULL) || ntg_object_is_widget(it_obj->__parent))
        {
            return it_obj;
        }

        it_obj = it_obj->__parent;
    }
}

ntg_object* ntg_object_get_parent(ntg_object* object, ntg_object_parent_opts opts)
{
    assert(object != NULL);

    ntg_object* group_root;
    switch(opts)
    {
        case NTG_OBJECT_PARENT_INCL_DECOR:
            return object->__parent;
        case NTG_OBJECT_PARENT_EXCL_DECOR:
             group_root = ntg_object_get_group_root(object);

             return group_root->__parent;
        default:
             assert(0);
    }
}

ntg_object* ntg_object_get_base_widget(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(ntg_object_is_decorator(it_obj))
        it_obj = it_obj->__children->_data[0];

    return it_obj;
}

ntg_object_vec_view ntg_object_get_children_(ntg_object* object)
{
    assert(object != NULL);

    return ntg_object_vec_view_new(object->__children);
}

const ntg_object_vec* ntg_object_get_children(const ntg_object* object)
{
    assert(object != NULL);

    return object->__children;
}

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor)
{
    assert(object != NULL);
    assert(ancestor != NULL);

    const ntg_object* it_object = object;
    while(it_object != NULL)
    {
        if(it_object == ancestor) return true;

        it_object = it_object->__parent;
    }
    
    return false;
}

bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant)
{
    assert(object != NULL);
    assert(descendant != NULL);

    return ntg_object_is_ancestor(descendant, object);
}

void ntg_object_tree_perform(ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data)
{
    assert(object != NULL);
    assert(perform_fn != NULL);

    ntg_object_vec* children = object->__children;
    size_t count = children->_count;

    ntg_object* it_child;
    size_t i;
    if(mode == NTG_OBJECT_PERFORM_TOP_DOWN)
    {
        perform_fn(object, data);

        for(i = 0; i < count; i++)
        {
            it_child = children->_data[i];
            ntg_object_tree_perform(it_child, mode, perform_fn, data);
        }
    }
    else
    {
        for(i = 0; i < count; i++)
        {
            it_child = children->_data[i];
            ntg_object_tree_perform(it_child, mode, perform_fn, data);
        }

        perform_fn(object, data);
    }
}

/* ---------------------------------------------------------------- */
/* SIZE CONTROL */
/* ---------------------------------------------------------------- */

struct ntg_xy ntg_object_get_min_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__min_size;
}

void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__min_size = size;
}

struct ntg_xy ntg_object_get_max_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__max_size;
}

void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__max_size = size;
}

struct ntg_xy ntg_object_get_grow(const ntg_object* object)
{
    assert(object != NULL);
    
    return object->__grow;
}

void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow)
{
    assert(object != NULL);

    object->__grow = grow;
}

/* ---------------------------------------------------------------- */
/* PADDING & BORDER */
/* ---------------------------------------------------------------- */

ntg_padding* ntg_object_get_padding(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* parent = ntg_object_get_parent(object, NTG_OBJECT_PARENT_INCL_DECOR);
    if((parent == NULL) || (parent->__type != NTG_OBJECT_PADDING))
        return NULL;
    else return (ntg_padding*)parent;
}

ntg_padding* ntg_object_set_padding(ntg_object* object, ntg_padding* padding)
{
    assert(object != NULL);

    ntg_object* _padding = (ntg_object*)padding;
    ntg_object* old_padding = (ntg_object*)ntg_object_get_padding(object);
    ntg_object* border = (ntg_object*)ntg_object_get_border(object);
    ntg_object* wparent = ntg_object_get_parent(object, NTG_OBJECT_PARENT_EXCL_DECOR);
    ntg_object* old_padding_parent = (border != NULL) ? border : wparent;

    if(old_padding == _padding) return NULL;

    if(_padding != NULL)
        assert((_padding->__children->_count == 0) && (_padding->__parent == NULL));

    /* Remove old padding from the tree */
    if(old_padding != NULL)
    {
        if(old_padding_parent != NULL)
            _ntg_object_rm_child(old_padding_parent, old_padding);

        _ntg_object_rm_child(old_padding, object);
    }

    /* Connect padding below */
    if(_padding != NULL)
    {
        _ntg_object_add_child(_padding, object);
    }

    /* Connect old_padding_parent below */
    if(old_padding_parent != NULL)
    {
        ntg_object* old_padding_parent_new_child = 
            (border != NULL) ? border : _padding;

        _ntg_object_add_child(old_padding_parent, old_padding_parent_new_child);
    }

    return ((ntg_padding*)old_padding);
}

ntg_border* ntg_object_get_border(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* root = ntg_object_get_group_root(object);
    if(root->__type != NTG_OBJECT_BORDER) return NULL;
    else return (ntg_border*)root;
}

ntg_border* ntg_object_set_border(ntg_object* object, ntg_border* border)
{
    assert(object != NULL);

    ntg_object* _border = (ntg_object*)border;
    ntg_object* old_border = (ntg_object*)ntg_object_get_border(object);
    ntg_object* wparent = ntg_object_get_parent(object, NTG_OBJECT_PARENT_EXCL_DECOR);
    ntg_object* padding = (ntg_object*)ntg_object_get_padding(object);

    if(old_border == _border) return NULL;

    if(_border != NULL)
        assert((_border->__children->_count == 0) && (_border->__parent == NULL));

    /* Remove old_border from the tree */
    if(old_border != NULL)
    {
        if(wparent != NULL)
            _ntg_object_rm_child(wparent, old_border);

        if(padding != NULL)
            _ntg_object_rm_child(old_border, padding);
    }

    /* Connect new border below */
    if(border != NULL)
    {
        ntg_object* border_new_child = (padding != NULL) ? padding : object;
        _ntg_object_add_child(_border, border_new_child);
    }

    /* Connect wparent with new border */
    if(wparent != NULL)
    {
        ntg_object* wparent_new_child = 
            (border != NULL) ?
            _border :
            ((padding != NULL) ? padding : object);

        _ntg_object_add_child(wparent, wparent_new_child);
    }

    return ((ntg_border*)old_border);
}

ntg_cell ntg_object_get_background(const ntg_object* object)
{
    assert(object != NULL);

    return object->__background;
}

void ntg_object_set_background(ntg_object* object, ntg_cell background)
{
    assert(object != NULL);

    object->__background = background;
}

/* ---------------------------------------------------------------- */
/* LAYOUT */
/* ---------------------------------------------------------------- */

struct ntg_object_measure ntg_object_measure(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena)
{
    assert(object != NULL);
    assert(for_size != 0);
    assert(out != NULL);
    assert(arena != NULL);
    // if(for_size == 0) return (struct ntg_measure_out) {0};

    struct ntg_object_measure result;
    if(object->__measure_fn != NULL)
    {
        result = object->__measure_fn(object, orientation, for_size,
                ctx, out, arena);
    }
    else result = (struct ntg_object_measure) {0};

    size_t user_min_size = (orientation == NTG_ORIENTATION_H) ?
        object->__min_size.x :
        object->__min_size.y;
    size_t user_max_size = (orientation == NTG_ORIENTATION_H) ?
        object->__max_size.x :
        object->__max_size.y;
    size_t user_grow = (orientation == NTG_ORIENTATION_H) ?
        object->__grow.x :
        object->__grow.y;

    user_min_size = _min2_size(user_min_size, user_max_size);

    if(user_max_size < result.min_size)
        result.min_size = user_max_size;
    else
        result.min_size = _max2_size(result.min_size, user_min_size);

    if(user_min_size > result.max_size)
        result.max_size = user_min_size;
    else
        result.max_size = _min2_size(result.max_size, user_max_size);

    result.natural_size = _clamp_size(result.min_size,
            result.natural_size, result.max_size);

    result.grow = user_grow;

    return result;
}

void ntg_object_constrain(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena)
{
    assert(object != NULL);
    assert(size != 0);
    assert(out != NULL);
    assert(arena != NULL);

    /* Check if to call wrapped constrain fn */
    if((size == 0) || (object->__constrain_fn == NULL))
    {
        const ntg_object_vec* children = object->__children;
        if(children == NULL) return;

        size_t i;
        for(i = 0; i < children->_count; i++)
            ntg_object_sizes_set(out->sizes, children->_data[i], 0);
    }
    else
    {
        object->__constrain_fn(object, orientation, size, ctx, out, arena);
    }
}

void ntg_object_arrange(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena)
{
    assert(object != NULL);
    assert(out != NULL);
    assert(arena != NULL);

    if(ntg_xy_size_is_zero(size) || (object->__arrange_fn == NULL))
    {
        const ntg_object_vec* children = object->__children;
        if(children == NULL) return;

        size_t i;
        for(i = 0; i < children->_count; i++)
            ntg_object_xys_set(out->pos, children->_data[i], ntg_xy(0, 0));
    }
    else
    {
        object->__arrange_fn(object, size, ctx, out, arena);
    }
}

void ntg_object_draw(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena)
{
    assert(object != NULL);
    assert(out != NULL);
    assert(arena != NULL);
    if(ntg_xy_size_is_zero(size)) return;

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
            (*it_cell) = object->__background;
        }
    }

    if(object->__draw_fn != NULL)
        object->__draw_fn(object, size, ctx, out, arena);
}

/* ---------------------------------------------------------------- */
/* EVENT */
/* ---------------------------------------------------------------- */

bool ntg_object_feed_key(
        ntg_object* object,
        struct nt_key_event key_event,
        struct ntg_object_key_ctx ctx)
{
    assert(object != NULL);

    return object->__process_key_fn(object, key_event, ctx);
}

/* ---------------------------------------------------------------- */

void ntg_object_on_focus(ntg_object* object, struct ntg_object_focus_ctx ctx)
{
    assert(object != NULL);

    return object->__on_focus_fn(object, ctx);
}

void ntg_object_on_unfocus(ntg_object* object, struct ntg_object_unfocus_ctx ctx)
{
    assert(object != NULL);

    return object->__on_unfocus_fn(object, ctx);
}

ntg_listenable* ntg_object_get_listenable(ntg_object* object)
{
    assert(object != NULL);

    return ntg_event_dlgt_listenable(object->_delegate);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_object* object);

static bool __process_key_fn_def(ntg_object* object,
        struct nt_key_event key_event,
        struct ntg_object_key_ctx ctx)
{
    return false;
}

static void __focus_fn_def(ntg_object* object, struct ntg_object_focus_ctx ctx) {}
static void __unfocus_fn_def(ntg_object* object, struct ntg_object_unfocus_ctx ctx) {}

static unsigned int __id_generator = 0;

void __ntg_object_init__(ntg_object* object,
        unsigned int type,
        ntg_object_measure_fn measure_fn,
        ntg_object_constrain_fn constrain_fn,
        ntg_object_arrange_fn arrange_fn,
        ntg_object_draw_fn draw_fn,
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data)
{
    assert(object != NULL);

    __init_default_values(object);

    object->__id = __id_generator++;
    object->__type = type;

    object->__children = ntg_object_vec_new();

    object->__background = ntg_cell_default();

    object->__measure_fn = measure_fn;
    object->__constrain_fn = constrain_fn;
    object->__arrange_fn = arrange_fn;
    object->__draw_fn = draw_fn;
    object->__process_key_fn = (process_key_fn != NULL) ?
        process_key_fn : __process_key_fn_def;
    object->__on_focus_fn = (on_focus_fn != NULL) ?
        on_focus_fn : __focus_fn_def;
    object->__on_unfocus_fn = (on_unfocus_fn != NULL) ?
        on_unfocus_fn : __unfocus_fn_def;
    object->__deinit_fn = (deinit_fn != NULL) ? deinit_fn : __ntg_object_deinit__;

    object->_delegate = ntg_event_dlgt_new();
    object->_data = data;
}

void __ntg_object_deinit__(ntg_object* object)
{
    assert(object != NULL);

    ntg_object_vec_destroy(object->__children);
    ntg_event_dlgt_destroy(object->_delegate);

    __init_default_values(object);
}

void _ntg_object_add_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    assert(child->__parent == NULL);

    ntg_object_vec_append(object->__children, child);

    child->__parent = object;
}

void _ntg_object_rm_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    if(child->__parent != object) return;

    ntg_object_vec_remove(object->__children, child);

    child->__parent = NULL;
}

/* ---------------------------------------------------------------- */

static void __init_default_values(ntg_object* object)
{
    object->__id = UINT_MAX;
    object->__type = NTG_OBJECT_WIDGET;

    object->__parent = NULL;
    object->__children = NULL;

    object->__grow = ntg_xy(1, 1);
    object->__min_size = ntg_xy(0, 0);
    object->__max_size = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);

    object->__measure_fn = NULL;
    object->__constrain_fn = NULL;
    object->__arrange_fn = NULL;
    object->__draw_fn = NULL;
    object->__process_key_fn = NULL;
    object->__on_focus_fn = NULL;
    object->__on_unfocus_fn = NULL;

    object->_delegate = NULL;
}
