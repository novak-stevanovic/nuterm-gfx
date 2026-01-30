#ifndef NTG_OBJECT_LAYOUT_H
#define NTG_OBJECT_LAYOUT_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_object_layout_ops
{
    ntg_object_lctx_init_fn lctx_init_fn;
    ntg_object_lctx_deinit_fn lctx_deinit_fn;
    ntg_object_hmeasure_fn hmeasure_fn;
    ntg_object_hconstrain_fn hconstrain_fn;
    ntg_object_vmeasure_fn vmeasure_fn;
    ntg_object_vconstrain_fn vconstrain_fn;
    ntg_object_arrange_fn arrange_fn;
    ntg_object_draw_fn draw_fn;
};

#define NTG_OBJECT_DIRTY_NONE 0
#define NTG_OBJECT_DIRTY_MEASURE (1 << 0)
#define NTG_OBJECT_DIRTY_CONSTRAIN (1 << 1)
#define NTG_OBJECT_DIRTY_ARRANGE (1 << 2)
#define NTG_OBJECT_DIRTY_DRAW (1 << 3)
#define NTG_OBJECT_DIRTY_FULL \
    ( NTG_OBJECT_DIRTY_MEASURE \
    | NTG_OBJECT_DIRTY_CONSTRAIN \
    | NTG_OBJECT_DIRTY_ARRANGE \
    | NTG_OBJECT_DIRTY_DRAW )

/* -------------------------------------------------------------------------- */
/* MEASURE PHASE */
/* -------------------------------------------------------------------------- */

struct ntg_object_measure
{
    size_t min_size, nat_size, max_size, grow;
};

static inline bool ntg_object_measure_are_equal(
        struct ntg_object_measure m1,
        struct ntg_object_measure m2)
{
    return ((m1.min_size == m2.min_size) &&
    (m1.nat_size == m2.nat_size) &&
    (m1.max_size == m2.max_size) &&
    (m1.grow == m2.grow));
}

/* -------------------------------------------------------------------------- */
/* CONSTRAIN PHASE */
/* -------------------------------------------------------------------------- */

size_t ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object);

void ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size);

/* -------------------------------------------------------------------------- */
/* ARRANGE PHASE */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_object_pos_map_get(
        const ntg_object_pos_map* map,
        const ntg_object* object);

void ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos);

/* -------------------------------------------------------------------------- */
/* DRAW PHASE */
/* -------------------------------------------------------------------------- */

// TODO: inline

struct ntg_vcell
ntg_object_tmp_drawing_get(const ntg_object_tmp_drawing* drawing, struct ntg_xy pos);

void ntg_object_tmp_drawing_set(
        ntg_object_tmp_drawing* drawing,
        struct ntg_vcell cell,
        struct ntg_xy pos);

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* MEASURE & SIZE HELPERS */
/* -------------------------------------------------------------------------- */

// To fetch measure/size for the whole object(including border and padding),
// access object fields directly or use on of these helpers:

struct ntg_object_measure ntg_object_get_hmeasure(const ntg_object* object);
struct ntg_object_measure ntg_object_get_vmeasure(const ntg_object* object);

// To fetch measure/size for object excluding border and padding, use one
// of these functions:

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size_cont(const ntg_object* object);
struct ntg_object_measure ntg_object_get_hmeasure_cont(const ntg_object* object);
struct ntg_object_measure ntg_object_get_vmeasure_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_size_cont(const ntg_object* object);

// To fetch measure/size for object excluding border, use one of these
// functions:

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object);
struct ntg_object_measure ntg_object_get_hmeasure_pad(const ntg_object* object);
struct ntg_object_measure ntg_object_get_vmeasure_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_size_pad(const ntg_object* object);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

void ntg_object_add_dirty(ntg_object* object, uint8_t dirty);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

// Called by scene layer - internal?

void _ntg_object_lctx_init(ntg_object* object);
void _ntg_object_lctx_deinit(ntg_object* object);
void _ntg_object_hmeasure(ntg_object* object, sarena* arena);
void _ntg_object_hconstrain(ntg_object* object, sarena* arena);
void _ntg_object_vmeasure(ntg_object* object, sarena* arena);
void _ntg_object_vconstrain(ntg_object* object, sarena* arena);
void _ntg_object_arrange(ntg_object* object, sarena* arena);
void _ntg_object_draw(ntg_object* object, sarena* arena);

void _ntg_object_root_set_hsize(ntg_object* object, size_t size);
void _ntg_object_root_set_vsize(ntg_object* object, size_t size);

#endif // NTG_OBJECT_LAYOUT_H
