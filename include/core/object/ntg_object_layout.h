#ifndef NTG_OBJECT_LAYOUT_H
#define NTG_OBJECT_LAYOUT_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "base/ntg_cell.h"
#include <stdint.h>

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_object_layout_dt
{
    /* Called on scene enter */
    void (*reset_fn)(struct ntg_object_layout_dt* data);

    /* Called on object deinit */
    void (*free_fn)(struct ntg_object_layout_dt* data);
};

NTG_API void
ntg_object_layout_dt_free_fn(struct ntg_object_layout_dt* data);

/* ------------------------------------------------------ */
/* DIRTY */
/* ------------------------------------------------------ */

enum ntg_object_dirty_flag
{
    /* These flags can be set by the user or in the layout process.
     * They are cleaned by the scene. */
    NTG_OBJECT_DIRTY_HMEASURE = (1u << 0),
    NTG_OBJECT_DIRTY_HCONSTRAIN = (1u << 1),
    NTG_OBJECT_DIRTY_VMEASURE = (1u << 2),
    NTG_OBJECT_DIRTY_VCONSTRAIN = (1u << 3),
    NTG_OBJECT_DIRTY_POST_CONSTRAIN = (1u << 4),
    NTG_OBJECT_DIRTY_ARRANGE = (1u << 5),
    NTG_OBJECT_DIRTY_DRAW = (1u << 6),

    /* This flag is set in draw phase automatically. It is cleaned by the stage. */
    NTG_OBJECT_DIRTY_RENDER = (1u << 7)
};

#define NTG_OBJECT_DIRTY_MEASURE (NTG_OBJECT_DIRTY_HMEASURE | NTG_OBJECT_DIRTY_VMEASURE)

#define NTG_OBJECT_DIRTY_CONSTRAIN (NTG_OBJECT_DIRTY_HCONSTRAIN | NTG_OBJECT_DIRTY_VCONSTRAIN)

#define NTG_OBJECT_DIRTY_FULL (                                                \
    NTG_OBJECT_DIRTY_HMEASURE |                                                \
    NTG_OBJECT_DIRTY_HCONSTRAIN |                                              \
    NTG_OBJECT_DIRTY_VMEASURE |                                                \
    NTG_OBJECT_DIRTY_VCONSTRAIN |                                              \
    NTG_OBJECT_DIRTY_POST_CONSTRAIN |                                          \
    NTG_OBJECT_DIRTY_ARRANGE |                                                 \
    NTG_OBJECT_DIRTY_DRAW )

enum ntg_object_layout_flag
{
    NTG_OBJECT_LAYOUT_STAY_DIRTY = (1u << 0),
    NTG_OBJECT_LAYOUT_REQ_VALIDATE = (1u << 1)
};

/* ------------------------------------------------------ */
/* MEASURE PHASE */
/* ------------------------------------------------------ */

struct ntg_object_measure
{
    size_t min_size, nat_size, max_size, grow;
};

static inline bool
ntg_object_measure_are_eql(struct ntg_object_measure m1, struct ntg_object_measure m2)
{
    return ((m1.min_size == m2.min_size) &&
            (m1.nat_size == m2.nat_size) &&
            (m1.max_size == m2.max_size) &&
            (m1.grow == m2.grow));
}

/* ------------------------------------------------------ */
/* CONSTRAIN PHASE */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size);

/* ------------------------------------------------------ */
/* ARRANGE PHASE */
/* ------------------------------------------------------ */


NTG_API struct ntg_xy
ntg_object_pos_map_get(
        const ntg_object_pos_map* map,
        const ntg_object* object);

/* ------------------------------------------------------ */


NTG_API void
ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos);

/* ------------------------------------------------------ */
/* DRAW PHASE */
/* ------------------------------------------------------ */

struct ntg_object_tmp_drawing
{
    struct ntg_vcell* data;
    struct ntg_xy size;
};

static inline struct ntg_vcell
ntg_object_tmp_drawing_get(const ntg_object_tmp_drawing* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_vcell_new_default();

    if(ntg_xy_is_lesser(pos, drawing->size))
        return drawing->data[drawing->size.x * pos.y + pos.x];
    else
        return ntg_vcell_new_default();
}

static inline void
ntg_object_tmp_drawing_set(
        ntg_object_tmp_drawing* drawing,
        struct ntg_vcell cell,
        struct ntg_xy pos)
{
    if(!drawing) return;

    if(ntg_xy_is_lesser(pos, drawing->size))
        drawing->data[drawing->size.x * pos.y + pos.x] = cell;
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_object_mark_dirty(ntg_object* object, uint32_t dirty);

/* ------------------------------------------------------ */
/* MEASURE & SIZE HELPERS */
/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_min_size(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_nat_size(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_max_size(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_size(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_min_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_nat_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_max_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_object_measure
ntg_object_get_measure_cont(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_get_size_1d_cont(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_get_for_size_cont(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_min_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_nat_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_max_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API struct ntg_object_measure
ntg_object_get_measure_pad(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_get_size_1d_pad(const ntg_object* object, ntg_orient orient);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_object_hmeasure(ntg_object* object, sarena* arena, uint32_t* out_layout_flags);
void _ntg_object_hconstrain(ntg_object* object, sarena* arena, uint32_t* out_layout_flags);
void _ntg_object_vmeasure(ntg_object* object, sarena* arena, uint32_t* out_layout_flags);
void _ntg_object_vconstrain(ntg_object* object, sarena* arena, uint32_t* out_layout_flags);
uint32_t _ntg_object_post_constrain(ntg_object* object, sarena* arena);
void _ntg_object_arrange(ntg_object* object, sarena* arena, uint32_t* out_layout_flags);
void _ntg_object_draw(ntg_object* object, sarena* arena, uint32_t* out_layout_flags);

void _ntg_object_root_set_hsize(ntg_object* object, size_t size);
void _ntg_object_root_set_vsize(ntg_object* object, size_t size);
void _ntg_object_root_set_pos(ntg_object* object, struct ntg_xy pos);

void _ntg_object_clean(ntg_object* object, uint32_t clean);

#endif // NTG_OBJECT_LAYOUT_H
