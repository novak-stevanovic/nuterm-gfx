#ifndef _NTG_FN_H_
#define _NTG_FN_H_

#include <stddef.h>
#include "shared/ntg_xy.h"
#include "sarena.h"

typedef struct ntg_object ntg_object;
typedef struct ntg_object_drawing ntg_object_drawing;

/* -------------------------------------------------------------------------- */
/* MEASURE */
/* -------------------------------------------------------------------------- */

struct ntg_measure_data
{
    size_t min_size, natural_size, max_size;
};

struct ntg_measure_result
{
    size_t min_size, natural_size, max_size;
};

typedef struct ntg_measure_context ntg_measure_context;

/* Measures how much space this object would require along axis A
 * if it is allocated `size` units along axis B.
 *
 * The calculation should be based on the object's current state
 * (spacing, text content, etc.). */
typedef struct ntg_measure_result (*ntg_measure_fn)(const ntg_object* object,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

/* ntg_measure_context */
ntg_measure_context* ntg_measure_context_new(const ntg_object* parent, SArena* arena);
void ntg_measure_context_destroy(ntg_measure_context* context);
struct ntg_measure_data ntg_measure_context_get(const ntg_measure_context* context,
        const ntg_object* child);
void ntg_measure_context_set(ntg_measure_context* context,
        const ntg_object* child,
        struct ntg_measure_data data);

/* -------------------------------------------------------------------------- */
/* CONSTRAIN */
/* -------------------------------------------------------------------------- */

typedef struct ntg_constrain_context ntg_constrain_context;
typedef struct ntg_constrain_output ntg_constrain_output;

struct ntg_constrain_data
{
    size_t min_size, natural_size, max_size;
};

struct ntg_constrain_result
{
    size_t size;
};

/* Determines the children's sizes based on the given `size` for the parent,
 * as well as the children's min, natural and max sizes.
 *
 * Returns collective children size(content size). */
typedef void (*ntg_constrain_fn)(const ntg_object* object,
        ntg_orientation orientation, size_t size,
        const ntg_constrain_context* context,
        ntg_constrain_output* out_sizes);

/* ntg_constrain_context */
ntg_constrain_context* ntg_constrain_context_new(const ntg_object* parent, SArena* arena);
void ntg_constrain_context_destroy(ntg_constrain_context* context);
struct ntg_constrain_data ntg_constrain_context_get(
        const ntg_constrain_context* context,
        ntg_object* child);
void ntg_constrain_context_set(ntg_constrain_context* context,
        const ntg_object* child,
        struct ntg_constrain_data data);
void ntg_constrain_context_set_min_size(ntg_constrain_context* context,
        size_t min_size);
void ntg_constrain_context_set_natural_size(ntg_constrain_context* context,
        size_t natural_size);
void ntg_constrain_context_set_max_size(ntg_constrain_context* context,
        size_t max_size);
size_t ntg_constrain_context_get_min_size(const ntg_constrain_context* context);
size_t ntg_constrain_context_get_natural_size(const ntg_constrain_context* context);
size_t ntg_constrain_context_get_max_size(const ntg_constrain_context* context);

/* ntg_constrain_output */
ntg_constrain_output* ntg_constrain_output_new(const ntg_object* parent, SArena* arena);
void ntg_constrain_output_destroy(ntg_constrain_output* output);
void ntg_constrain_output_set(ntg_constrain_output* output,
        const ntg_object* child,
        struct ntg_constrain_result result);
struct ntg_constrain_result ntg_constrain_output_get(
        const ntg_constrain_output* output,
        const ntg_object* child);

/* -------------------------------------------------------------------------- */
/* ARRANGE */
/* -------------------------------------------------------------------------- */

struct ntg_arrange_data
{
    struct ntg_xy size;
};

struct ntg_arrange_result
{
    struct ntg_xy pos;
};

typedef struct ntg_arrange_context ntg_arrange_context;
typedef struct ntg_arrange_output ntg_arrange_output;

typedef void (*ntg_arrange_children_fn)(const ntg_object* object,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* out_positions);

typedef void (*ntg_arrange_drawing_fn)(const ntg_object* object,
        struct ntg_xy size, ntg_object_drawing* out_drawing);

/* ntg_arrange_context */
ntg_arrange_context* ntg_arrange_context_new(const ntg_object* parent, SArena* arena);
void ntg_arrange_context_destroy(ntg_arrange_context* context);
struct ntg_arrange_data ntg_arrange_context_get(
        const ntg_arrange_context* context,
        const ntg_object* child);
void ntg_arrange_context_set(ntg_arrange_context* context,
        const ntg_object* child,
        struct ntg_arrange_data data);
// void ntg_arrange_context_set_content_size(ntg_arrange_context* context,
//         struct ntg_xy size);
// struct ntg_xy ntg_arrange_context_get_content_size(const ntg_arrange_context* context);

/* ntg_arrange_output */
ntg_arrange_output* ntg_arrange_output_new(const ntg_object* parent, SArena* arena);
void ntg_arrange_output_destroy(ntg_arrange_output* output);
void ntg_arrange_output_set(ntg_arrange_output* output,
        const ntg_object* child,
        struct ntg_arrange_result result);
struct ntg_arrange_result ntg_arrange_output_get(
        const ntg_arrange_output* output,
        const ntg_object* child);

#endif // _NTG_FN_H_
