#include "object/ntg_object.h"
#include "shared/_ntg_shared.h"
#include "core/ntg_scene.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/ntg_log.h"
#include <assert.h>
#include <stdlib.h>

static bool __process_key_fn(ntg_object* object, struct nt_key_event key_event)
{
    ntg_log_log("KEY EVENT FOR OBJECT: %p", object);

    return true;
}

/* -------------------------------------------------------------------------- */
/* def/ntg_object_def.h */
/* -------------------------------------------------------------------------- */

static void _ntg_object_init_default(ntg_object* object)
{
    object->_parent = NULL;
    object->_children = NULL;

    object->__scroll = false;
    object->_drawing = NULL;
    object->__buffered_scroll = ntg_dxy(0, 0);

    object->_pref_size = NTG_XY_UNSET;
    object->_nsize = NTG_XY_UNSET;
    object->_constr = NTG_CONSTR_UNSET;
    object->_size = NTG_XY_UNSET;
    object->_pos = NTG_XY_UNSET;

    object->__arrange_fn = NULL;
    object->__measure_fn = NULL;
    object->__constrain_fn = NULL;
    object->__nsize_fn = NULL;

    // TODO
    object->__process_key_fn = __process_key_fn;

    object->_scene = NULL;
}

void __ntg_object_init__(ntg_object* object, ntg_nsize_fn nsize_fn,
        ntg_constrain_fn constrain_fn, ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn)
{
    assert(object != NULL); 

    _ntg_object_init_default(object);

    object->__constrain_fn = constrain_fn;
    object->__measure_fn = measure_fn;
    object->__arrange_fn = arrange_fn;
    object->__nsize_fn = nsize_fn;

    object->_drawing = ntg_object_drawing_new();
    object->_children = ntg_object_vec_new();
}

void __ntg_object_deinit__(ntg_object* object)
{
    assert(object != NULL); 

    if(object->_drawing != NULL)
        ntg_object_drawing_destroy(object->_drawing);

    if(object->_children != NULL)
        ntg_object_vec_destroy(object->_children);

    _ntg_object_init_default(object);
}

void _ntg_object_set_nsize(ntg_object* object, struct ntg_xy size)
{
    if(object == NULL) return;

    ntg_xy_size(&size);

    if(object->_pref_size.x != NTG_PREF_SIZE_UNSET)
        size.x = object->_pref_size.x;

    if(object->_pref_size.y != NTG_PREF_SIZE_UNSET)
        size.y = object->_pref_size.y;

    object->_nsize = size;
}

static void _adjust_constr(size_t pref_size, size_t* min_size, size_t* max_size)
{
    if(pref_size == NTG_PREF_SIZE_UNSET) return;

    if((pref_size >= (*min_size)) && (pref_size <= (*max_size)))
    {
        (*min_size) = pref_size;
        (*max_size) = pref_size;
    }
    else if(pref_size >= (*max_size))
        (*min_size) = (*max_size);
    else
        (*max_size) = (*min_size);
}

void _ntg_object_set_constr(ntg_object* object, struct ntg_constr constr)
{
    if(object == NULL) return;

    _adjust_constr(object->_pref_size.x, &constr.min_size.x, &constr.max_size.x);
    _adjust_constr(object->_pref_size.y, &constr.min_size.y, &constr.max_size.y);

    object->_constr = constr;

}

static void _fit_size_to_constr(size_t* size, size_t min_size, size_t max_size)
{
    if((*size) > max_size)
        (*size) = max_size;

    else if((*size) < min_size)
        (*size) = min_size;
}

void _ntg_object_set_size(ntg_object* object, struct ntg_xy size)
{
    if(object == NULL) return;

    _fit_size_to_constr(&(size.x), object->_constr.min_size.x,
            object->_constr.max_size.x);
    _fit_size_to_constr(&(size.y), object->_constr.min_size.y,
            object->_constr.max_size.y);

    ntg_xy_size(&size);

    object->_size = size;

    if(object->__scroll)
    {
        struct ntg_xy drawing_size = ntg_xy(
                _max2_size(object->_nsize.x, size.x),
                _max2_size(object->_nsize.y, size.y));

        ntg_object_drawing_set_size(object->_drawing, drawing_size);

        ntg_object_drawing_scroll(object->_drawing, object->__buffered_scroll);
        object->__buffered_scroll = ntg_dxy(0, 0);

        ntg_object_drawing_set_vp_size(object->_drawing, size);
    }
    else
    {
        ntg_object_drawing_set_size(object->_drawing, size);

        ntg_object_drawing_set_vp_offset(object->_drawing,
                ntg_xy(0, 0));
        object->__buffered_scroll = ntg_dxy(0, 0);

        ntg_object_drawing_set_vp_size(object->_drawing, size);
    }
}

void _ntg_object_set_pos(ntg_object* object, struct ntg_xy pos)
{
    if(object == NULL) return;

    struct ntg_xy parent_pos = (object->_parent != NULL) ?
        object->_parent->_pos :
        NTG_XY_UNSET;

    struct ntg_xy abs_pos = ntg_xy_add(pos, parent_pos);
    object->_pos = abs_pos;
}

void _ntg_object_scroll_enable(ntg_object* object)
{
    if(!object->__scroll)
        ntg_object_drawing_set_vp_offset(object->_drawing, ntg_xy(0, 0));

    object->__scroll = true;
}

void _ntg_object_scroll_disable(ntg_object* object)
{
    object->__scroll = false;
    ntg_object_drawing_set_vp_offset(object->_drawing, ntg_xy(0, 0));
}

void _ntg_object_scroll(ntg_object* object, struct ntg_dxy offset_diff)
{
    if(object->__scroll)
    {
        object->__buffered_scroll = ntg_dxy_add(object->__buffered_scroll,
                offset_diff);
    }
}

struct ntg_xy _ntg_object_get_scroll(const ntg_object* object)
{
    return (object->__scroll) ?
        ntg_object_drawing_get_vp_offset(object->_drawing) :
        ntg_xy(0, 0);
}

static void __adjust_scene_fn(ntg_object* object, void* scene)
{
    object->_scene = (ntg_scene*)scene;
}

void _ntg_object_child_add(ntg_object* parent, ntg_object* child)
{
    assert((parent != NULL) && (child != NULL));

    ntg_object* curr_parent = child->_parent;

    if(curr_parent != NULL)
    {
        ntg_object_vec_remove(curr_parent->_children, child);
    }

    ntg_object_vec_append(parent->_children, child);

    child->_parent = parent;

    _ntg_object_perform_tree(child, __adjust_scene_fn, parent->_scene);
}

void _ntg_object_child_remove(ntg_object* parent, ntg_object* child)
{
    assert((parent != NULL) && (child != NULL));

    ntg_object_vec_remove(parent->_children, child);

    child->_parent = NULL;

    _ntg_object_perform_tree(child, __adjust_scene_fn, NULL);
}

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn)
{
    assert(object != NULL);

    object->__process_key_fn = process_key_fn;
}

void _ntg_object_perform_tree(ntg_object* root,
        void (*perform_fn)(ntg_object* curr_obj, void* data),
        void* data)
{
    if(root == NULL) return;

    perform_fn(root, data);

    size_t i;
    for(i = 0; i < root->_children->_count; i++)
    {
        _ntg_object_perform_tree(root->_children->_data[i], perform_fn, data);
    }
}

/* -------------------------------------------------------------------------- */
/* ntg_object.h */
/* -------------------------------------------------------------------------- */

void ntg_object_layout_root(ntg_object* root, struct ntg_xy root_size)
{
    // root->_nsize = root_size;
    root->_constr = ntg_constr(root_size, root_size);
    root->_pos = ntg_xy(0, 0);
}

void ntg_object_set_pref_size(ntg_object* object, struct ntg_xy pref_size)
{
    if(object == NULL) return;

    object->_pref_size = pref_size;
}

void ntg_object_calculate_nsize(ntg_object* object)
{
    if(object == NULL) return;
    // if(object->_parent == NULL) return;

    if(object->__nsize_fn != NULL)
        object->__nsize_fn(object);
}

void ntg_object_constrain(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__constrain_fn != NULL)
        object->__constrain_fn(object);
}

void ntg_object_measure(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__measure_fn != NULL)
        object->__measure_fn(object);
}

void ntg_object_arrange(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__arrange_fn != NULL)
        object->__arrange_fn(object);

}

struct ntg_xy ntg_object_get_pos_rel(const ntg_object* object)
{
    if(object == NULL) return ntg_xy(SIZE_MAX, SIZE_MAX);

    if(object->_parent != NULL)
        return ntg_xy_sub(object->_pos, object->_parent->_pos);
    else
        return object->_pos;
}

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    return (object != NULL) ? object->_size : NTG_XY_UNSET;
}

struct ntg_xy ntg_object_get_content_size(const ntg_object* object)
{
    return (object != NULL) ? object->_size : NTG_XY_UNSET;
}

struct ntg_xy ntg_object_get_pos(const ntg_object* object)
{
    return (object != NULL) ? object->_pos : NTG_XY_UNSET;
}

struct ntg_xy ntg_object_get_content_pos(const ntg_object* object)
{
    return (object != NULL) ? object->_pos : NTG_XY_UNSET;
}

const ntg_object_drawing* ntg_object_get_drawing(const ntg_object* object)
{
    return object->_drawing;
}

bool ntg_object_feed_key_event(ntg_object* object, struct nt_key_event key_event)
{
    assert(object != NULL);

    if(object->__process_key_fn != NULL)
        return object->__process_key_fn(object, key_event);
    else
        return false;
}

void _ntg_object_set_scene(ntg_object* root, ntg_scene* scene)
{
    assert(root != NULL);
    root->_scene = scene;

    _ntg_object_perform_tree(root, __adjust_scene_fn, root->_scene);
}
