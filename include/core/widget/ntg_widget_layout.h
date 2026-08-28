#ifndef NTG_WIDGET_LAYOUT_H
#define NTG_WIDGET_LAYOUT_H

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "base/ntg_xy.h"
#include "base/ntg_cell.h"
#include <stdint.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_widget_layout_dt
{
    /* Called on scene enter */
    void (*reset_fn)(struct ntg_widget_layout_dt* data);

    /* Called on widget deinit */
    void (*free_fn)(void* layout_dt);
};

/* ------------------------------------------------------ */
/* DIRTY */
/* ------------------------------------------------------ */

enum ntg_widget_dirty_flag
{
    /* Must be set by user explicitly. Calls user-defined layout_prepare()
     * fn before scene starts the layout process. */
    NTG_WIDGET_DIRTY_PREPARE = (1u << 0),

    /* These flags can be set by the user or in the layout process.
     * They are cleaned by the scene. */
    NTG_WIDGET_DIRTY_HMEASURE = (1u << 1),
    NTG_WIDGET_DIRTY_HCONSTRAIN = (1u << 2),
    NTG_WIDGET_DIRTY_VMEASURE = (1u << 3),
    NTG_WIDGET_DIRTY_VCONSTRAIN = (1u << 4),
    NTG_WIDGET_DIRTY_ARRANGE = (1u << 5),
    NTG_WIDGET_DIRTY_DRAW = (1u << 6),

    /* Set in draw phase automatically. It is cleaned by the stage. */
    NTG__WIDGET_DIRTY_RENDER = (1u << 8)
};

#define NTG_WIDGET_DIRTY_MEASURE (                                             \
    NTG_WIDGET_DIRTY_HMEASURE |                                                \
    NTG_WIDGET_DIRTY_VMEASURE )

#define NTG_WIDGET_DIRTY_CONSTRAIN (                                           \
    NTG_WIDGET_DIRTY_HCONSTRAIN |                                              \
    NTG_WIDGET_DIRTY_VCONSTRAIN)

#define NTG_WIDGET_DIRTY_FULL (                                                \
    NTG_WIDGET_DIRTY_HMEASURE |                                                \
    NTG_WIDGET_DIRTY_HCONSTRAIN |                                              \
    NTG_WIDGET_DIRTY_VMEASURE |                                                \
    NTG_WIDGET_DIRTY_VCONSTRAIN |                                              \
    NTG_WIDGET_DIRTY_ARRANGE |                                                 \
    NTG_WIDGET_DIRTY_DRAW )

/* ------------------------------------------------------ */
/* MEASURE PHASE */
/* ------------------------------------------------------ */

struct ntg_widget_measure
{
    size_t min_size, nat_size, max_size, grow;
};

static const struct ntg_widget_measure NTG_WIDGET_MEASURE_ZERO = {0};

static inline bool
ntg_widget_measure_are_eql(struct ntg_widget_measure m1, struct ntg_widget_measure m2)
{
    return ((m1.min_size == m2.min_size) && (m1.nat_size == m2.nat_size) &&
            (m1.max_size == m2.max_size) && (m1.grow == m2.grow));
}

/* ------------------------------------------------------ */
/* CONSTRAIN PHASE */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_widget_size_map_get(
        const ntg_widget_size_map* map,
        const ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API int
ntg_widget_size_map_set(
        ntg_widget_size_map* map,
        const ntg_widget* widget,
        size_t size);

/* ------------------------------------------------------ */

NTG_API int
ntg_widget_zero_constrain(const ntg_widget* widget, ntg_widget_size_map* map);

/* ------------------------------------------------------ */
/* ARRANGE PHASE */
/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_widget_pos_map_get(
        const ntg_widget_pos_map* map,
        const ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API int
ntg_widget_pos_map_set(
        ntg_widget_pos_map* map,
        const ntg_widget* widget,
        struct ntg_xy pos);

/* ------------------------------------------------------ */

NTG_API int
ntg_widget_zero_arrange(const ntg_widget* widget, ntg_widget_pos_map* map);

/* ------------------------------------------------------ */
/* DRAW PHASE */
/* ------------------------------------------------------ */

struct ntg_widget_tmp_draw
{
    struct ntg_vcell* data;
    struct ntg_xy size;
};

static inline struct ntg_vcell
ntg_widget_tmp_draw_get(const ntg_widget_tmp_draw* drawing, struct ntg_xy pos)
{
    if(!drawing) return NTG_VCELL_ZERO;

    if(ntg_xy_is_lt(pos, drawing->size))
        return drawing->data[drawing->size.x * pos.y + pos.x];
    else
        return NTG_VCELL_ZERO;
}

static inline int
ntg_widget_tmp_draw_set(
        ntg_widget_tmp_draw* drawing,
        struct ntg_vcell cell,
        struct ntg_xy pos)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    if(ntg_xy_is_lt(pos, drawing->size))
        drawing->data[drawing->size.x * pos.y + pos.x] = cell;

    return 0;
}

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_widget_mark_dirty(ntg_widget* widget, uint32_t dirty);

/* ------------------------------------------------------ */
/* MEASURE & SIZE HELPERS */
/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_widget_get_min_size(const ntg_widget* widget);

NTG_API struct ntg_xy
ntg_widget_get_nat_size(const ntg_widget* widget);

NTG_API struct ntg_xy
ntg_widget_get_max_size(const ntg_widget* widget);

NTG_API struct ntg_widget_measure
ntg_widget_get_measure(const ntg_widget* widget, enum ntg_orient orient);

NTG_API size_t
ntg_widget_get_size_1d(const ntg_widget* widget, enum ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_widget_get_min_size_cont(const ntg_widget* widget);

NTG_API struct ntg_xy
ntg_widget_get_nat_size_cont(const ntg_widget* widget);

NTG_API struct ntg_xy
ntg_widget_get_max_size_cont(const ntg_widget* widget);

NTG_API struct ntg_widget_measure
ntg_widget_get_measure_cont(const ntg_widget* widget, enum ntg_orient orient);

NTG_API size_t
ntg_widget_get_size_1d_cont(const ntg_widget* widget, enum ntg_orient orient);

NTG_API size_t
ntg_widget_get_for_size_cont(const ntg_widget* widget, enum ntg_orient orient);

/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_widget_get_min_size_pad(const ntg_widget* widget);

NTG_API struct ntg_xy
ntg_widget_get_nat_size_pad(const ntg_widget* widget);

NTG_API struct ntg_xy
ntg_widget_get_max_size_pad(const ntg_widget* widget);

NTG_API struct ntg_widget_measure
ntg_widget_get_measure_pad(const ntg_widget* widget, enum ntg_orient orient);

NTG_API size_t
ntg_widget_get_size_1d_pad(const ntg_widget* widget, enum ntg_orient orient);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__widget_layout_prepare(ntg_widget* widget, sarena* arena);
int ntg__widget_hmeasure(ntg_widget* widget, sarena* arena, uint32_t* relayout);
int ntg__widget_hconstrain(ntg_widget* widget, sarena* arena, uint32_t* relayout);
int ntg__widget_vmeasure(ntg_widget* widget, sarena* arena, uint32_t* relayout);
int ntg__widget_vconstrain(ntg_widget* widget, sarena* arena, uint32_t* relayout);
int ntg__widget_arrange(ntg_widget* widget, sarena* arena, uint32_t* relayout);
void ntg__widget_layout_finalize(ntg_widget* widget, sarena* arena);
int ntg__widget_draw(ntg_widget* widget, sarena* arena);

void ntg__widget_root_set_hsize(ntg_widget* widget, size_t size);
void ntg__widget_root_set_vsize(ntg_widget* widget, size_t size);
void ntg__widget_root_set_pos(ntg_widget* widget, struct ntg_xy pos);

void ntg__widget_clean(ntg_widget* widget, uint32_t clean);

#endif // NTG_WIDGET_LAYOUT_H
