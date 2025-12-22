#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include <stdbool.h>
#include <stddef.h>
#include "base/entity/ntg_entity.h"
#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_vec.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object;

typedef struct ntg_scene ntg_scene;
typedef struct ntg_padding ntg_padding;
typedef struct ntg_object_measure_map ntg_object_measure_map;
typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object_xy_map ntg_object_xy_map;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_loop_ctx ntg_loop_ctx;
typedef struct sarena sarena;
struct nt_key_event;
struct ntg_object_measure_ctx;
struct ntg_object_measure_out;
struct ntg_object_constrain_ctx;
struct ntg_object_constrain_out;
struct ntg_object_arrange_ctx;
struct ntg_object_arrange_out;
struct ntg_object_draw_ctx;
struct ntg_object_draw_out;
struct ntg_object_key_ctx;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Dynamically allocates and initializes an object that will be used in the
 * layout process for `object`. */
typedef void* (*ntg_object_layout_init_fn)(const ntg_object* object);

/* Frees the resources associated with `layout_data` object. */
typedef void (*ntg_object_layout_deinit_fn)(
        const ntg_object* object,
        void* layout_data);

struct ntg_object_measure_ctx
{
    const ntg_object_measure_map* measures;
};

struct ntg_object_measure_out
{
    size_t __placeholder;
};

/* Measures how much space the object would require along one axis,
 * if the size is constrained for the other axis. */
typedef struct ntg_object_measure (*ntg_object_measure_fn)(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_constrain_ctx
{
    const ntg_object_measure_map* measures;
};

struct ntg_object_constrain_out
{
    ntg_object_size_map* const sizes;
};

/* Determines the children's sizes for given `orientation` and `size`. */
typedef void (*ntg_object_constrain_fn)(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_arrange_ctx
{
    const ntg_object_xy_map* sizes;
};

struct ntg_object_arrange_out
{
    ntg_object_xy_map* const pos;
};

/* Determines children positions for given `size`. */
typedef void (*ntg_object_arrange_fn)(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_draw_ctx
{
    const ntg_object_xy_map* sizes;
    const ntg_object_xy_map* pos;
};

struct ntg_object_draw_out
{
    ntg_object_drawing* const drawing;
};

/* Creates the `object` drawing. */
typedef void (*ntg_object_draw_fn)(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_key_ctx
{
    ntg_scene* scene;
    ntg_loop_ctx* loop_ctx;
};

/* Returns if the key had been consumed. */
typedef bool (*ntg_object_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event,
        struct ntg_object_key_ctx ctx);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

struct ntg_object_init_data
{
    ntg_object_layout_init_fn layout_init_fn;
    ntg_object_layout_deinit_fn layout_deinit_fn;
    ntg_object_measure_fn measure_fn;
    ntg_object_constrain_fn constrain_fn;
    ntg_object_arrange_fn arrange_fn;
    ntg_object_draw_fn draw_fn;

    ntg_object_process_key_fn process_key_fn;
};

void _ntg_object_init_(ntg_object* object,
        struct ntg_object_init_data object_data,
        struct ntg_entity_init_data entity_data);

void _ntg_object_deinit_fn(ntg_entity* entity);

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

const ntg_object* ntg_object_get_group_root(const ntg_object* object);
ntg_object* ntg_object_get_group_root_(ntg_object* object);

typedef enum ntg_object_parent_opts
{
    NTG_OBJECT_PARENT_INCL_DECOR,
    NTG_OBJECT_PARENT_EXCL_DECOR
} ntg_object_parent_opts;

/* Gets object's parent node. */
const ntg_object* ntg_object_get_parent(const ntg_object* object, ntg_object_parent_opts opts);
ntg_object* ntg_object_get_parent_(ntg_object* object, ntg_object_parent_opts opts);

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

const ntg_padding* ntg_object_get_padding(const ntg_object* object);
ntg_padding* ntg_object_get_padding_(ntg_object* object);
ntg_padding* ntg_object_set_padding(ntg_object* object, ntg_padding* padding);

const ntg_padding* ntg_object_get_border(const ntg_object* object);
ntg_padding* ntg_object_get_border_(ntg_object* object);
ntg_padding* ntg_object_set_border(ntg_object* object, ntg_padding* border);

ntg_cell ntg_object_get_background(const ntg_object* object);
void ntg_object_set_background(ntg_object* object, ntg_cell background);

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

bool ntg_object_feed_key(ntg_object* object,
        struct nt_key_event key_event,
        struct ntg_object_key_ctx ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    ntg_entity __base;

    struct
    {
        ntg_object* __parent;
        ntg_object_vec* __children;
    };

    struct
    {
        struct ntg_xy __min_size, __max_size, __grow;
    };

    struct
    {
        ntg_cell __background;
    };

    struct
    {
        ntg_object_layout_init_fn __layout_init_fn;
        ntg_object_layout_deinit_fn __layout_deinit_fn;
        ntg_object_measure_fn __measure_fn;
        ntg_object_constrain_fn __constrain_fn;
        ntg_object_arrange_fn __arrange_fn;
        ntg_object_draw_fn __draw_fn;

    };

    struct
    {
        ntg_object_process_key_fn __process_key_fn;
    };

    void* data;
};

void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);

#endif // _NTG_OBJECT_H_
