#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include <stddef.h>
#include "core/entity/ntg_entity.h"
#include "shared/ntg_xy.h"
#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "thirdparty/genc.h"
#include "core/object/shared/ntg_object_measure.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

GENC_VECTOR_GENERATE(ntg_object_vec, ntg_object*, 1.5, NULL);

struct ntg_object_layout_ops
{
    ntg_object_layout_data_init init_fn;
    ntg_object_layout_data_deinit deinit_fn;
    ntg_object_measure_fn measure_fn;
    ntg_object_constrain_fn constrain_fn;
    ntg_object_arrange_fn arrange_fn;
    ntg_object_draw_fn draw_fn;
};

/* ------------------------------------------------------ */

struct ntg_object
{
    ntg_entity __base;

    struct
    {
        ntg_scene* _scene;
        ntg_object* _parent;
        ntg_object_vec _children;
    };

    struct
    {
        struct ntg_object_layout_ops __layout_ops;
        void* __layout_data;
    };

    struct
    {

        struct ntg_xy _min_size, _nat_size, _max_size, _grow;
        struct ntg_xy _size;
        struct ntg_xy _pos;
        ntg_object_drawing _drawing;
    };
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* OBJECT TREE */
/* ------------------------------------------------------ */

const ntg_object* ntg_object_get_root(const ntg_object* object);
ntg_object* ntg_object_get_root_(ntg_object* object);

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor);
bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant);

#define NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(fn_name, perform_fn)               \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    perform_fn(object, data);                                                  \
    const ntg_object_vec* children = &object->_children;                       \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
}                                                                              \

#define NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(fn_name, perform_fn)              \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    const ntg_object_vec* children = &object->_children;                       \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    perform_fn(object, data);                                                  \
}                                                                              \

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

void ntg_object_root_layout(ntg_object* root, struct ntg_xy size);

void ntg_object_measure(ntg_object* object,
        ntg_orient orient,
        bool constrained, sarena* arena);
void ntg_object_constrain(ntg_object* object,
        ntg_orient orient, sarena* arena);
void ntg_object_arrange(ntg_object* object, sarena* arena);
void ntg_object_draw(ntg_object* object, sarena* arena);

struct ntg_xy ntg_object_get_pos_abs(const ntg_object* object);

// Layout helpers
size_t ntg_object_get_for_size(const ntg_object* object,
        ntg_orient orient, bool constrained);

struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient);

size_t ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

// enum ntg_object_addchld_status
// {
//     NTG_OBJECT_ADDCHLD_SUCCESS = 0,
//     NTG_OBJECT_ADDCHLD_ALR_CHILD,
//     NTG_OBJECT_ADDCHLD_HAS_PARENT,
//     NTG_OBJECT_ADDCHLD_SELF
// };

void ntg_object_init(ntg_object* object, struct ntg_object_layout_ops layout_ops);
void ntg_object_deinit_fn(ntg_entity* entity);

// enum ntg_object_addchld_status 
// ntg_object_validate_add_child(ntg_object* parent, ntg_object* child);

/* Updates only the tree. Called internally by types extending ntg_object. */
void ntg_object_attach(ntg_object* parent, ntg_object* child);

/* Updates only the tree. Called internally by types extending ntg_object. */
void ntg_object_detach(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// Called internally by ntg_scene. Updates only the object's state
void _ntg_object_root_set_scene(ntg_object* root, ntg_scene* scene);

#endif // NTG_OBJECT_H
