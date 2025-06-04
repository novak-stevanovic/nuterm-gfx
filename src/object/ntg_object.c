#include "object/ntg_object.h"
#include "base/ntg_cell.h"
#include <assert.h>
#include <stdlib.h>

#define __NTG_ALLOW_NTG_OBJECT_DEF__
#include "object/def/ntg_object_def.h"
#undef __NTG_ALLOW_NTG_OBJECT_DEF__

/* -------------------------------------------------------------------------- */

struct ntg_object_content
{
    ntg_cell_grid_t* _data;
    struct ntg_xy _size;
};

static void ntg_object_content_set_size(ntg_object_content_t* content,
        struct ntg_xy size);

/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object_t* object, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn,
        ntg_object_vec_t* children, ntg_object_content_t* content)
{
    if(object == NULL) return;

    object->_constrain_fn = constrain_fn;
    object->_measure_fn = measure_fn;
    object->_arrange_fn = arrange_fn;

    object->_children = children;
    object->_content = content;
    object->_parent = NULL;
    object->_constr = NTG_CONSTR_UNSET;
    object->_pos = NTG_XY_POS_UNSET;
    object->_size = NTG_XY_SIZE_UNSET;
}

void __ntg_object_deinit__(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->_children != NULL) ntg_object_vec_destroy(object->_children);

    if(object->_content != NULL)
        ntg_object_content_destroy(object->_content);

    object->_parent = NULL;
    object->_constr = NTG_CONSTR_UNSET;
    object->_pos = NTG_XY_POS_UNSET;
    object->_size = NTG_XY_SIZE_UNSET;
    object->_arrange_fn = NULL;
    object->_measure_fn = NULL;
    object->_constrain_fn = NULL;
}

ntg_object_t* ntg_object_get_parent(const ntg_object_t* object)
{
    return (object != NULL) ? object->_parent : NULL;
}

const ntg_object_vec_t* ntg_object_get_children(const ntg_object_t* object)
{
    return (object != NULL) ? object->_children : NULL;
}

void ntg_object_constrain(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->_constrain_fn != NULL)
        object->_constrain_fn(object);
}

void ntg_object_measure(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->_measure_fn != NULL)
        object->_measure_fn(object);
}

void ntg_object_arrange(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->_arrange_fn != NULL)
        object->_arrange_fn(object);
}

struct ntg_xy ntg_object_get_position_abs(const ntg_object_t* object)
{
    return (object != NULL) ? object->_pos : NTG_XY(SIZE_MAX, SIZE_MAX);
}

struct ntg_xy ntg_object_get_position_rel(const ntg_object_t* object)
{
    if(object == NULL) return NTG_XY(SIZE_MAX, SIZE_MAX);

    if(object->_parent != NULL)
        return NTG_DXY_XY(NTG_XY_SUB(object->_pos, object->_parent->_pos));
    else
        return object->_pos;
}

const ntg_object_content_t* ntg_object_get_content(const ntg_object_t* object)
{
    return object->_content;
}

/* -------------------------------------------------------------------------- */

ntg_object_content_t* ntg_object_content_new()
{
    ntg_object_content_t* new = (ntg_object_content_t*)malloc(sizeof(struct ntg_object_content));
    if(new == NULL) return NULL;

    new->_data = ntg_cell_grid_new(NTG_XY(NTG_TERM_MAX_WIDTH, NTG_TERM_MAX_HEIGHT));
    if(new->_data == NULL)
    {
        free(new);
        return NULL;
    }

    new->_size = NTG_XY_SIZE_UNSET;

    return new;
}

void ntg_object_content_destroy(ntg_object_content_t* content)
{
    if(content == NULL) return;

    ntg_cell_grid_destroy(content->_data);

    free(content);
}

ntg_cell_t* ntg_object_content_at_(ntg_object_content_t* content,
        struct ntg_xy pos)
{
    if(content == NULL) return NULL;

    if(!NTG_XY_POS_IN_BOUNDS(pos, content->_size))
        return NULL;

    return ntg_cell_grid_at_(content->_data, pos);
}

const ntg_cell_t* ntg_object_content_at(const ntg_object_content_t* content,
        struct ntg_xy pos)
{
    if(content == NULL) return NULL;

    if(!NTG_XY_POS_IN_BOUNDS(pos, content->_size))
        return NULL;

    return ntg_cell_grid_at(content->_data, pos);
}

struct ntg_xy ntg_object_content_get_size(const ntg_object_content_t* content)
{
    return (content != NULL) ? content->_size : NTG_XY_SIZE_UNSET;
}

static void ntg_object_content_set_size(ntg_object_content_t* content,
        struct ntg_xy size)
{
    if(content == NULL) return;

    content->_size = size;
}
