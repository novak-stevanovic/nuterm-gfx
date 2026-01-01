#ifndef _NTG_OBJECT_LAYOUT_H_
#define _NTG_OBJECT_LAYOUT_H_

#include <stddef.h>
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object;
typedef struct sarena sarena;
typedef struct ntg_object_measure_map ntg_object_measure_map;
typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object_xy_map ntg_object_xy_map;
typedef struct ntg_object_drawing ntg_object_drawing;

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

/* ------------------------------------------------------ */

struct ntg_object_measure_ctx
{
    const ntg_object_measure_map* measures; // children measures
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
    const ntg_object_measure_map* measures; // children measures
};

struct ntg_object_constrain_out
{
    ntg_object_size_map* const sizes; // children sizes
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
    const ntg_object_xy_map* sizes; // children sizes
};

struct ntg_object_arrange_out
{
    ntg_object_xy_map* const positions; // children positions
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
    const ntg_object_xy_map* sizes; // children sizes
    const ntg_object_xy_map* positions; // children positions
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

#endif // _NTG_OBJECT_LAYOUT_H_
