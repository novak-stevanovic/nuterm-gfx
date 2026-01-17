#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include <stddef.h>
#include "core/entity/ntg_entity.h"
#include "shared/ntg_xy.h"
#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "shared/genc.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

GENC_VECTOR_GENERATE(ntg_object_vec, ntg_object*, 1.5, NULL);

enum ntg_object_target
{
    NTG_OBJECT_GROUP_ROOT,
    NTG_OBJECT_SELF
};

struct ntg_object_layout_ops
{
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
        ntg_scene* __scene;
        ntg_object* __parent;
        ntg_object_vec __children;
    };

    struct
    {
        struct ntg_xy _user_min_size, _user_max_size, _user_grow;
    };

    struct
    {
        struct ntg_object_layout_ops __layout_ops;
        struct ntg_vcell _background;

        void* layout_data;
    };

    struct
    {

        struct ntg_xy __min_size, __nat_size, __max_size, __grow;
        struct ntg_xy __size;
        struct ntg_xy __position;
        ntg_object_drawing __drawing;
    };

    struct
    {
        ntg_object_event_fn __event_fn;
    };

    void* data;
};

struct ntg_object_vec_view
{
    ntg_object* const * data;
    size_t size;
};

struct ntg_object_vec_cview
{
    const ntg_object* const * object;
    size_t size;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* IDENTITY */
/* ------------------------------------------------------ */

bool ntg_object_is_widget(const ntg_object* object);
bool ntg_object_is_decorator(const ntg_object* object);
bool ntg_object_is_border(const ntg_object* object);
bool ntg_object_is_padding(const ntg_object* object);

/* ------------------------------------------------------ */
/* OBJECT TREE */
/* ------------------------------------------------------ */

const ntg_object_vec* ntg_object_get_children(const ntg_object* object);

ntg_scene* ntg_object_get_scene(ntg_object* object);

const ntg_object* ntg_object_get_group_root(const ntg_object* object);
ntg_object* ntg_object_get_group_root_(ntg_object* object);

const ntg_object* ntg_object_get_root(const ntg_object* object, bool incl_dcr);
ntg_object* ntg_object_get_root_(ntg_object* object, bool incl_dcr);

/* Gets object's parent node. */
const ntg_object* ntg_object_get_parent(const ntg_object* object, bool incl_dcr);
ntg_object* ntg_object_get_parent_(ntg_object* object, bool incl_dcr);

/* Returns base widget for node group(non-decorator). */
const ntg_object* ntg_object_get_base_widget(const ntg_object* object);
ntg_object* ntg_object_get_base_widget_(ntg_object* object);

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor);
bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant);

#define NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(fn_name, perform_fn)               \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    perform_fn(object, data);                                                  \
    const ntg_object_vec* children = ntg_object_get_children(object);          \
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
    const ntg_object_vec* children = ntg_object_get_children(object);          \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    perform_fn(object, data);                                                  \
}                                                                              \

/* ------------------------------------------------------ */
/* SIZE CONTROL */
/* ------------------------------------------------------ */

void ntg_object_set_user_min_size(ntg_object* object, struct ntg_xy size);

void ntg_object_set_user_max_size(ntg_object* object, struct ntg_xy size);

void ntg_object_set_user_grow(ntg_object* object, struct ntg_xy grow);

/* ------------------------------------------------------ */
/* PADDING & BORDER */
/* ------------------------------------------------------ */

const ntg_padding* ntg_object_get_padding(const ntg_object* object);
ntg_padding* ntg_object_get_padding_(ntg_object* object);
void ntg_object_set_padding(ntg_object* object, ntg_padding* padding);

const ntg_padding* ntg_object_get_border(const ntg_object* object);
ntg_padding* ntg_object_get_border_(ntg_object* object);
void ntg_object_set_border(ntg_object* object, ntg_padding* border);

void ntg_object_set_background(ntg_object* object, struct ntg_vcell background);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

struct ntg_object_measure ntg_object_measure(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

void ntg_object_constrain(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena);

void ntg_object_arrange(
        const ntg_object* object,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena);

void ntg_object_draw(
        const ntg_object* object,
        struct ntg_xy size,
        ntg_temp_object_drawing* out_drawing,
        sarena* arena);

struct ntg_object_measure ntg_object_get_measure(
        const ntg_object* object,
        ntg_orientation orientation,
        ntg_object_target target);
void ntg_object_set_measure(
        ntg_object* object,
        struct ntg_object_measure measure,
        ntg_orientation orientation, ntg_object_target target);

struct ntg_xy ntg_object_get_size(
        const ntg_object* object,
        ntg_object_target target);
void ntg_object_set_size(
        ntg_object* object,
        struct ntg_xy size,
        ntg_object_target target);

struct ntg_xy ntg_object_get_position(
        const ntg_object* object,
        ntg_object_target target);
struct ntg_xy ntg_object_get_position_abs(
        const ntg_object* object,
        ntg_object_target target);
void ntg_object_set_position(
        ntg_object* object,
        struct ntg_xy position,
        ntg_object_target target);

const ntg_object_drawing* ntg_object_get_drawing(
        const ntg_object* object,
        ntg_object_target target);
void ntg_object_update_drawing(
        ntg_object* object,
        const ntg_temp_object_drawing* drawing,
        struct ntg_xy size_cap,
        ntg_object_target target);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_object_feed_event(
        ntg_object* object,
        struct nt_event event,
        ntg_loop_ctx* ctx);

void ntg_object_set_event_fn(
        ntg_object* object,
        ntg_object_event_fn fn);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_object_init(ntg_object* object, struct ntg_object_layout_ops layout_ops);
void _ntg_object_deinit_fn(ntg_entity* entity);

bool _ntg_object_validate_add_child(ntg_object* object, ntg_object* child);
// Adding before validating is UB
void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);

void _ntg_object_mark_change(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_object_set_scene(ntg_object* object, ntg_scene* scene);

#endif // NTG_OBJECT_H
