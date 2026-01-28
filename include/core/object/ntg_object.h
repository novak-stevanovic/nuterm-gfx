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
    ntg_object_layout_init_fn init_fn;
    ntg_object_layout_deinit_fn deinit_fn;
    ntg_object_measure_fn measure_fn;
    ntg_object_constrain_fn constrain_fn;
    ntg_object_arrange_fn arrange_fn;
    ntg_object_draw_fn draw_fn;
};

enum ntg_object_type
{
    NTG_OBJECT_BASE,
    NTG_OBJECT_DECORATOR
};

#define NTG_OBJECT_DIRTY_NONE 0
#define NTG_OBJECT_DIRTY_MEASURE (1 << 0)
#define NTG_OBJECT_DIRTY_CONSTRAIN (1 << 1)
#define NTG_OBJECT_DIRTY_ARRANGE (1 << 2)
#define NTG_OBJECT_DIRTY_DRAW (1 << 3)
#define NTG_OBJECT_DIRTY_FULL ( \
    NTG_OBJECT_DIRTY_MEASURE   | \
    NTG_OBJECT_DIRTY_CONSTRAIN | \
    NTG_OBJECT_DIRTY_ARRANGE   | \
    NTG_OBJECT_DIRTY_DRAW )

/* ------------------------------------------------------ */

struct ntg_object
{
    ntg_entity __base;

    enum ntg_object_type _type;

    struct
    {
        ntg_scene* _scene;
        ntg_object* _parent;
        ntg_object_vec _children;
    };

    struct
    {
        struct ntg_object_layout_ops __layout_ops;
        void* __ldata;
        uint8_t dirty;
    };

    struct
    {
        struct ntg_xy _min_size, _nat_size, _max_size, _grow;
        struct ntg_xy _size;
        struct ntg_xy _pos;
        int _z_index;
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
bool ntg_object_is_ancestor_eq(const ntg_object* object, const ntg_object* ancestor);
bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant);
bool ntg_object_is_descendant_eq(const ntg_object* object, const ntg_object* descendant);

// Returns total count of objects in tree, including root
size_t ntg_object_get_tree_size(const ntg_object* root);

size_t ntg_object_get_children_by_z(const ntg_object* object, ntg_object** out_buff);

struct ntg_xy ntg_object_map_to_ancestor_space(
        const ntg_object* object_space,
        const ntg_object* ancestor_space,
        struct ntg_xy point);

struct ntg_xy ntg_object_map_to_descendant_space(
        const ntg_object* object_space,
        const ntg_object* descendant_space,
        struct ntg_xy point);

struct ntg_xy ntg_object_map_to_scene_space(
        const ntg_object* object_space,
        struct ntg_xy point);

struct ntg_xy ntg_object_map_from_scene_space(
        const ntg_object* object_space,
        struct ntg_xy point);

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

// Set size and position of root
void ntg_object_root_layout(ntg_object* root, struct ntg_xy size);

void ntg_object_measure(
        ntg_object* object,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void ntg_object_constrain(ntg_object* object, ntg_orient orient, sarena* arena);
void ntg_object_arrange(ntg_object* object, sarena* arena);
void ntg_object_draw(ntg_object* object, sarena* arena);

struct ntg_xy ntg_object_get_pos_abs(const ntg_object* object);

// Layout helpers
size_t ntg_object_get_for_size(
        const ntg_object* object,
        ntg_orient orient,
        bool constrained);

struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient);

size_t ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient);

void ntg_object_set_z_index(ntg_object* object, int z_index);

void ntg_object_add_dirty(ntg_object* object, uint8_t dirty);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_object_init(ntg_object* object,
        struct ntg_object_layout_ops layout_ops,
        ntg_object_type type);
void ntg_object_deinit(ntg_object* object);

/* Updates only the tree. Does not update scene. 
 * Called internally by types extending ntg_object. */
void ntg_object_attach(ntg_object* parent, ntg_object* child);

/* Updates only the tree. Does not update scene.
 * Called internally by types extending ntg_object. */
void ntg_object_detach(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_object_attach_scene(ntg_object* root, ntg_scene* scene);
void _ntg_object_detach_scene(ntg_object* root);

// Called internally by ntg_scene. Updates only the object's state
void _ntg_object_set_scene(ntg_object* object, ntg_scene* scene);

#endif // NTG_OBJECT_H
