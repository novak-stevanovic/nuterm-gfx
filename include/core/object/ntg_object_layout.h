#ifndef NTG_OBJECT_LAYOUT_H
#define NTG_OBJECT_LAYOUT_H

#include "shared/ntg_shared.h"
#include <stdint.h>

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_object_layout_ops
{
    ntg_object_measure_fn measure_fn;
    ntg_object_constrain_fn constrain_fn;
    ntg_object_fixup_fn fixup_fn;
    ntg_object_arrange_fn arrange_fn;
    ntg_object_draw_fn draw_fn;
};

#define NTG_OBJECT_DIRTY_NONE 0
#define NTG_OBJECT_DIRTY_HMEASURE (1 << 0)
#define NTG_OBJECT_DIRTY_HCONSTRAIN (1 << 1)
#define NTG_OBJECT_DIRTY_VMEASURE (1 << 2)
#define NTG_OBJECT_DIRTY_VCONSTRAIN (1 << 3)
#define NTG_OBJECT_DIRTY_ARRANGE (1 << 4)
#define NTG_OBJECT_DIRTY_DRAW (1 << 5)
#define NTG_OBJECT_DIRTY_RENDER (1 << 6)

#define NTG_OBJECT_DIRTY_MEASURE ( \
    NTG_OBJECT_DIRTY_HMEASURE | \
    NTG_OBJECT_DIRTY_VMEASURE)

#define NTG_OBJECT_DIRTY_CONSTRAIN ( \
    NTG_OBJECT_DIRTY_HCONSTRAIN | \
    NTG_OBJECT_DIRTY_VCONSTRAIN)

#define NTG_OBJECT_DIRTY_FULL ( \
    NTG_OBJECT_DIRTY_HMEASURE | \
    NTG_OBJECT_DIRTY_HCONSTRAIN | \
    NTG_OBJECT_DIRTY_VMEASURE | \
    NTG_OBJECT_DIRTY_VCONSTRAIN | \
    NTG_OBJECT_DIRTY_ARRANGE | \
    NTG_OBJECT_DIRTY_DRAW | \
    NTG_OBJECT_DIRTY_RENDER)

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

struct ntg_object_tmp_drawing
{
    struct ntg_vcell* data;
    struct ntg_xy size;
};

static inline struct ntg_vcell
ntg_object_tmp_drawing_get(const ntg_object_tmp_drawing* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_vcell_default();

    if(ntg_xy_is_lesser(pos, drawing->size))
        return drawing->data[drawing->size.x * pos.y + pos.x];
    else
        return ntg_vcell_default();
}

static inline void ntg_object_tmp_drawing_set(
        ntg_object_tmp_drawing* drawing,
        struct ntg_vcell cell,
        struct ntg_xy pos)
{
    if(!drawing) return;

    if(ntg_xy_is_lesser(pos, drawing->size))
        drawing->data[drawing->size.x * pos.y + pos.x] = cell;
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_object_mark_dirty(ntg_object* object, uint8_t dirty);

/* -------------------------------------------------------------------------- */
/* MEASURE & SIZE HELPERS */
/* -------------------------------------------------------------------------- */

// To fetch measure/size for the whole object(including border and padding),
// access object fields directly or use one of these helpers:

struct ntg_xy ntg_object_get_min_size(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size(const ntg_object* object);
struct ntg_xy ntg_object_get_size(const ntg_object* object);

struct ntg_object_measure 
ntg_object_get_measure(const ntg_object* object, ntg_orient orient);
size_t ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient);

// To fetch measure/size for object excluding border and padding, use one
// of these functions:

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size_cont(const ntg_object* object);
struct ntg_xy ntg_object_get_size_cont(const ntg_object* object);

struct ntg_object_measure
ntg_object_get_measure_cont(const ntg_object* object, ntg_orient orient);
size_t ntg_object_get_size_1d_cont(const ntg_object* object, ntg_orient orient);

size_t ntg_object_get_for_size_cont(const ntg_object* object, ntg_orient orient);

// To fetch measure/size for object excluding border, use one of these
// functions:

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object);
struct ntg_xy ntg_object_get_size_pad(const ntg_object* object);

struct ntg_object_measure 
ntg_object_get_measure_pad(const ntg_object* object, ntg_orient orient);
size_t ntg_object_get_size_1d_pad(const ntg_object* object, ntg_orient orient);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

// Called by scene layer - internal?

void _ntg_object_hmeasure(ntg_object* object, sarena* arena);
void _ntg_object_hconstrain(ntg_object* object, sarena* arena);
void _ntg_object_vmeasure(ntg_object* object, sarena* arena);
void _ntg_object_vconstrain(ntg_object* object, sarena* arena);
bool _ntg_object_fixup(ntg_object* object, sarena* arena);
void _ntg_object_arrange(ntg_object* object, sarena* arena);
void _ntg_object_draw(ntg_object* object, sarena* arena);

void _ntg_object_root_set_hsize(ntg_object* object, size_t size);
void _ntg_object_root_set_vsize(ntg_object* object, size_t size);
void _ntg_object_root_set_pos(ntg_object* object, struct ntg_xy pos);

void _ntg_object_clean(ntg_object* object, uint8_t dirty);

#endif // NTG_OBJECT_LAYOUT_H
