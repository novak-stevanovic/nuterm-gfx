#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include <stdbool.h>
#include <stddef.h>
#include "base/entity/ntg_entity.h"
#include "core/object/ntg_object_layout.h"
#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_vec.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_padding ntg_padding;
typedef struct ntg_loop_ctx ntg_loop_ctx;
struct ntg_loop_event;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

typedef bool (*ntg_object_event_fn)(
        ntg_object* object,
        struct ntg_loop_event event,
        ntg_loop_ctx* ctx);

/* ------------------------------------------------------ */

struct ntg_object_layout_ops
{
    ntg_object_layout_init_fn init_fn;
    ntg_object_layout_deinit_fn deinit_fn;
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
        ntg_object_vec* __children;
    };

    struct
    {
        struct ntg_xy __min_size, __max_size, __grow;
    };

    struct
    {
        struct ntg_object_layout_ops __layout_ops;
        struct ntg_vcell _background;
    };

    struct
    {
        ntg_object_event_fn __event_fn;
    };

    void* data;
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

ntg_scene* ntg_object_get_scene(ntg_object* object);

const ntg_object* ntg_object_get_group_root(const ntg_object* object);
ntg_object* ntg_object_get_group_root_(ntg_object* object);

const ntg_object* ntg_object_get_root(const ntg_object* object, bool incl_decor);
ntg_object* ntg_object_get_root_(ntg_object* object, bool incl_decor);

/* Gets object's parent node. */
const ntg_object* ntg_object_get_parent(const ntg_object* object, bool incl_decor);
ntg_object* ntg_object_get_parent_(ntg_object* object, bool incl_decor);

/* Returns base widget for node group(non-decorator). */
const ntg_object* ntg_object_get_base_widget(const ntg_object* object);
ntg_object* ntg_object_get_base_widget_(ntg_object* object);

struct ntg_const_object_vecv ntg_object_get_children(const ntg_object* object);
struct ntg_object_vecv ntg_object_get_children_(ntg_object* object);

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor);
bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant);

typedef enum ntg_object_perform_mode
{
    NTG_OBJECT_PERFORM_TOP_DOWN,
    NTG_OBJECT_PERFORM_BOTTOM_UP
} ntg_object_perform_mode;

void ntg_object_tree_perform(
        ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data);

/* ------------------------------------------------------ */
/* SIZE CONTROL */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_min_size(ntg_object* object);
void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size);

struct ntg_xy ntg_object_get_max_size(ntg_object* object);
void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size);

struct ntg_xy ntg_object_get_grow(const ntg_object* object);
void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow);

/* ------------------------------------------------------ */
/* PADDING & BORDER */
/* ------------------------------------------------------ */

// TODO: raise appropriate event for scene

const ntg_padding* ntg_object_get_padding(const ntg_object* object);
ntg_padding* ntg_object_get_padding_(ntg_object* object);
ntg_padding* ntg_object_set_padding(ntg_object* object, ntg_padding* padding);

const ntg_padding* ntg_object_get_border(const ntg_object* object);
ntg_padding* ntg_object_get_border_(ntg_object* object);
ntg_padding* ntg_object_set_border(ntg_object* object, ntg_padding* border);

void ntg_object_set_background(ntg_object* object, struct ntg_vcell background);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

void* ntg_object_layout_init(const ntg_object* object);
void ntg_object_layout_deinit(const ntg_object* object, void* layout_data);

struct ntg_object_measure ntg_object_measure(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

void ntg_object_constrain(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena);

void ntg_object_arrange(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena);

void ntg_object_draw(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_object_feed_event(
        ntg_object* object,
        struct ntg_loop_event event,
        ntg_loop_ctx* ctx);

void ntg_object_set_event_fn(
        ntg_object* object,
        ntg_object_event_fn fn);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_object_init_(ntg_object* object, struct ntg_object_layout_ops layout_ops);
void _ntg_object_deinit_fn(ntg_entity* entity);

/* object & child must be widgets */
void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);

void _ntg_object_mark_change(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_object_add_child_dcr(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child_dcr(ntg_object* object, ntg_object* child);
void _ntg_object_rm_children(ntg_object* object);

void _ntg_object_set_scene(ntg_object* object, ntg_scene* scene);

#endif // _NTG_OBJECT_H_
