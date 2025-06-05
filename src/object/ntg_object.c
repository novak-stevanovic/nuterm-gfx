#include "object/ntg_object.h"
#include "base/ntg_cell.h"
#include <assert.h>
#include <stdlib.h>

#define __NTG_ALLOW_NTG_OBJECT_DEF__
#include "object/def/ntg_object_def.h"
#undef __NTG_ALLOW_NTG_OBJECT_DEF__

/* -------------------------------------------------------------------------- */

struct ntg_object_drawing
{
    ntg_cell_grid_t* _data;
    struct ntg_xy _size;
};

static void ntg_object_drawing_set_size(ntg_object_drawing_t* drawing,
        struct ntg_xy size);

/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object_t* object, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn)
{
    if(object == NULL) return;

    object->__constrain_fn = constrain_fn;
    object->__measure_fn = measure_fn;
    object->__arrange_fn = arrange_fn;

    object->_children = ntg_object_vec_new();;
    object->_drawing = ntg_object_drawing_new();
    object->_parent = NULL;
    object->_constr = NTG_CONSTR_UNSET;
    object->_adj_constr = NTG_CONSTR_UNSET;
    object->_pos = NTG_XY_UNSET;
    object->_size = NTG_XY_UNSET;
}

void __ntg_object_deinit__(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->_children != NULL)
        ntg_object_vec_destroy(object->_children);

    if(object->_drawing != NULL)
        ntg_object_drawing_destroy(object->_drawing);

    object->_parent = NULL;
    object->_constr = NTG_CONSTR_UNSET;
    object->_pos = NTG_XY_UNSET;
    object->_size = NTG_XY_UNSET;
    object->__arrange_fn = NULL;
    object->__measure_fn = NULL;
    object->__constrain_fn = NULL;
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

    if(object->__constrain_fn != NULL)
        object->__constrain_fn(object);
}

void ntg_object_set_constr(ntg_object_t* object, struct ntg_constr constr)
{
    if(object == NULL) return;

    object->_constr = constr;
    object->_adj_constr = constr;
}

void ntg_object_measure(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->__measure_fn != NULL)
        object->__measure_fn(object);
}

void ntg_object_set_size(ntg_object_t* object, struct ntg_xy size)
{
    if(object == NULL) return;

    object->_size = size;

    ntg_object_drawing_set_size(object->_drawing, size);
}

void ntg_object_arrange(ntg_object_t* object)
{
    if(object == NULL) return;

    if(object->__arrange_fn != NULL)
        object->__arrange_fn(object);
}

void ntg_object_set_pos(ntg_object_t* object, struct ntg_xy pos)
{
    if(object == NULL) return;

    object->_pos = pos;
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

struct ntg_constr ntg_object_get_constr(const ntg_object_t* object)
{
    return (object != NULL) ? object->_constr : NTG_CONSTR_UNSET;
}

struct ntg_constr ntg_object_get_adj_constr(const ntg_object_t* object)
{
    return (object != NULL) ? object->_adj_constr : NTG_CONSTR_UNSET;
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

/* -------------------------------------------------------------------------- */

ntg_object_drawing_t* ntg_object_drawing_new()
{
    ntg_object_drawing_t* new = (ntg_object_drawing_t*)malloc(sizeof(struct ntg_object_drawing));
    if(new == NULL) return NULL;

    new->_data = ntg_cell_grid_new(NTG_XY(NTG_TERM_MAX_WIDTH, NTG_TERM_MAX_HEIGHT));
    if(new->_data == NULL)
    {
        free(new);
        return NULL;
    }

    new->_size = NTG_XY_UNSET;

    return new;
}

void ntg_object_drawing_destroy(ntg_object_drawing_t* drawing)
{
    if(drawing == NULL) return;

    ntg_cell_grid_destroy(drawing->_data);

    free(drawing);
}

ntg_cell_t* ntg_object_drawing_at_(ntg_object_drawing_t* drawing,
        struct ntg_xy pos)
{
    if(drawing == NULL) return NULL;

    if(!(NTG_XY_POS_IN_BOUNDS(pos, drawing->_size)))
        return NULL;

    return ntg_cell_grid_at_(drawing->_data, pos);
}

const ntg_cell_t* ntg_object_drawing_at(const ntg_object_drawing_t* drawing,
        struct ntg_xy pos)
{
    if(drawing == NULL) return NULL;

    if(!NTG_XY_POS_IN_BOUNDS(pos, drawing->_size))
        return NULL;

    return ntg_cell_grid_at(drawing->_data, pos);
}

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing_t* drawing)
{
    return (drawing != NULL) ? drawing->_size : NTG_XY_UNSET;
}

static void ntg_object_drawing_set_size(ntg_object_drawing_t* drawing,
        struct ntg_xy size)
{
    if(drawing == NULL) return;

    drawing->_size = size;
}
