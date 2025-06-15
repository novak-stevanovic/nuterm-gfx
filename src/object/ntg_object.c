#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/ntg_log.h"
#include <assert.h>
#include <stdlib.h>

#define __NTG_ALLOW_NTG_OBJECT_DEF__
#include "object/def/ntg_object_def.h"
#undef __NTG_ALLOW_NTG_OBJECT_DEF__

/* -------------------------------------------------------------------------- */
/* def/ntg_object_def.h */
/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object_t* object, ntg_nsize_fn nsize_fn,
        ntg_constrain_fn constrain_fn, ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn)
{
    if(object == NULL) return;

    object->__constrain_fn = constrain_fn;
    object->__measure_fn = measure_fn;
    object->__arrange_fn = arrange_fn;
    object->__nsize_fn = nsize_fn;

    object->_drawing = ntg_object_drawing_new();
    object->_children = ntg_object_vec_new();

    object->_parent = NULL;
    object->_constr = NTG_CONSTR_UNSET;
    object->_pos = NTG_XY_UNSET;
    object->_size = NTG_XY_UNSET;
    object->_nsize = NTG_XY_UNSET;
    object->_pref_scroll_offset = NTG_XY_UNSET;
    object->_scroll = false;
}

void __ntg_object_deinit__(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->_drawing != NULL)
        ntg_object_drawing_destroy(object->_drawing);

    if(object->_children != NULL)
        ntg_object_vec_destroy(object->_children);

    object->__arrange_fn = NULL;
    object->__measure_fn = NULL;
    object->__constrain_fn = NULL;
    object->__nsize_fn = NULL;

    object->_parent = NULL;
    object->_constr = NTG_CONSTR_UNSET;
    object->_pos = NTG_XY_UNSET;
    object->_size = NTG_XY_UNSET;
    object->_pref_scroll_offset = NTG_XY_UNSET;
    object->_scroll = false;
}

void _ntg_object_set_nsize(ntg_object_t* object, struct ntg_xy size)
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

void _ntg_object_set_constr(ntg_object_t* object, struct ntg_constr constr)
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

void _ntg_object_set_size(ntg_object_t* object, struct ntg_xy size)
{
    if(object == NULL) return;

    _fit_size_to_constr(&(size.x), object->_constr.min_size.x, object->_constr.max_size.x);
    _fit_size_to_constr(&(size.y), object->_constr.min_size.y, object->_constr.max_size.y);

    ntg_xy_size(&size);

    object->_size = size;

    ntg_log_log("A");

    if(object->_scroll)
    {
        struct ntg_xy drawing_size = ntg_xy(
                ((object->_nsize.x >= size.x) ? object->_nsize.x : size.x),
                ((object->_nsize.y >= size.y) ? object->_nsize.y : size.y));

        ntg_object_drawing_set_size(object->_drawing, drawing_size);

        ntg_log_log("B");

        ntg_object_drawing_set_vp_offset(object->_drawing,
                object->_pref_scroll_offset);

        ntg_log_log("C");

        ntg_object_drawing_set_vp_size(object->_drawing, size);

        ntg_log_log("D");
    }
    else
    {
        ntg_object_drawing_set_vp_offset(object->_drawing,
                ntg_xy(0, 0));

        ntg_log_log("E");

        ntg_object_drawing_set_size(object->_drawing, size);

        ntg_log_log("F");

        ntg_object_drawing_set_vp_size(object->_drawing, size);

        ntg_log_log("G");
    }
}

void _ntg_object_set_pos(ntg_object_t* object, struct ntg_xy pos)
{
    if(object == NULL) return;

    struct ntg_xy parent_pos = (object->_parent != NULL) ?
        object->_parent->_pos :
        NTG_XY_UNSET;

    struct ntg_xy abs_pos = ntg_xy_add(pos, parent_pos);
    object->_pos = abs_pos;
}

/* -------------------------------------------------------------------------- */
/* ntg_object.h */
/* -------------------------------------------------------------------------- */

ntg_object_t* ntg_object_get_parent(const ntg_object_t* object)
{
    return (object != NULL) ? object->_parent : NULL;
}

const ntg_object_vec_t* ntg_object_get_children(const ntg_object_t* object)
{
    return (object != NULL) ? object->_children : NULL;
}

void ntg_object_layout_root(ntg_object_t* root, struct ntg_xy root_size)
{
    // root->_nsize = root_size;
    root->_constr = ntg_constr(root_size, root_size);
    root->_pos = ntg_xy(0, 0);
}

struct ntg_xy ntg_object_get_pref_size(const ntg_object_t* object)
{
    return (object != NULL) ? object->_pref_size : NTG_XY_UNSET;
}

void ntg_object_set_pref_size(ntg_object_t* object, struct ntg_xy pref_size)
{
    if(object == NULL) return;

    object->_pref_size = pref_size;
}

void ntg_object_calculate_nsize(ntg_object_t* object)
{
    if(object == NULL) return;
    // if(object->_parent == NULL) return;

    if(object->__nsize_fn != NULL)
        object->__nsize_fn(object);
}

void ntg_object_constrain(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->__constrain_fn != NULL)
        object->__constrain_fn(object);
}

void ntg_object_measure(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->__measure_fn != NULL)
        object->__measure_fn(object);
}

void ntg_object_arrange(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->__arrange_fn != NULL)
        object->__arrange_fn(object);

}

struct ntg_xy ntg_object_get_position_abs(const ntg_object_t* object)
{
    return (object != NULL) ? object->_pos : ntg_xy(SIZE_MAX, SIZE_MAX);
}

struct ntg_xy ntg_object_get_position_rel(const ntg_object_t* object)
{
    if(object == NULL) return ntg_xy(SIZE_MAX, SIZE_MAX);

    if(object->_parent != NULL)
        return ntg_xy_sub(object->_pos, object->_parent->_pos);
    else
        return object->_pos;
}

struct ntg_xy ntg_object_get_nsize(const ntg_object_t* object)
{
    return (object != NULL) ? object->_nsize : NTG_XY_UNSET;
}

struct ntg_constr ntg_object_get_constr(const ntg_object_t* object)
{
    return (object != NULL) ? object->_constr : NTG_CONSTR_UNSET;
}

struct ntg_xy ntg_object_get_size(const ntg_object_t* object)
{
    return (object != NULL) ? object->_size : NTG_XY_UNSET;
}

struct ntg_xy ntg_object_get_content_size(const ntg_object_t* object)
{
    return (object != NULL) ? object->_size : NTG_XY_UNSET;
}

struct ntg_xy ntg_object_get_pos(const ntg_object_t* object)
{
    return (object != NULL) ? object->_pos : NTG_XY_UNSET;
}

struct ntg_xy ntg_object_get_content_pos(const ntg_object_t* object)
{
    return (object != NULL) ? object->_pos : NTG_XY_UNSET;
}

const ntg_object_drawing_t* ntg_object_get_drawing(const ntg_object_t* object)
{
    return object->_drawing;
}

ntg_object_drawing_t* _ntg_object_get_drawing_(ntg_object_t* object)
{
    return object->_drawing;
}

void _ntg_object_scroll_enable(ntg_object_t* object)
{
    object->_scroll = true;
}

void _ntg_object_scroll_disable(ntg_object_t* object)
{
    object->_scroll = false;
}

struct ntg_xy _ntg_object_get_pref_scroll_offset(const ntg_object_t* object)
{
    return object->_pref_scroll_offset;
}

void _ntg_object_set_pref_scroll_offset(ntg_object_t* object,
        struct ntg_xy offset)
{
    object->_pref_scroll_offset = offset;
}

void _ntg_object_scroll(ntg_object_t* object, struct ntg_dxy offset_diff)
{
    struct ntg_dxy new_offset = ntg_dxy_add(
            ntg_dxy_from_xy(object->_pref_scroll_offset),
            offset_diff);

    new_offset = ntg_dxy_clamp(ntg_dxy(0, 0), new_offset, NTG_DXY_MAX);

    object->_pref_scroll_offset = ntg_xy_from_dxy(new_offset);
}

struct ntg_xy _ntg_object_get_scroll_offset(const ntg_object_t* object)
{
    return ntg_object_drawing_get_vp_offset(object->_drawing);
}

void _ntg_object_child_add(ntg_object_t* parent, ntg_object_t* child)
{
    assert((parent != NULL) && (child != NULL));

    ntg_object_t* curr_parent = child->_parent;

    if(curr_parent != NULL)
    {
        ntg_object_vec_remove(curr_parent->_children, child);
    }

    ntg_object_vec_append(parent->_children, child);

    child->_parent = parent;
}

void _ntg_object_child_remove(ntg_object_t* parent, ntg_object_t* child)
{
    assert((parent != NULL) && (child != NULL));

    ntg_object_vec_remove(parent->_children, child);

    child->_parent = NULL;
}
