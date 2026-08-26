#ifndef NTG_BORDER_9X_H
#define NTG_BORDER_9X_H

#include "shared/ntg_shared.h"
#include "core/object/border/ntg_border_style.h"
#include "nt_gfx.h"
#include "base/ntg_cell.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_border_9x_sym
{
    uint32_t top_left, top, top_right, right,
             bottom_right, bottom, bottom_left,
             left, padding;
};

struct ntg_border_9x
{
    ntg_border_style _base;

    struct
    {
        struct ntg_border_9x_sym symbols;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_border_9x_init_monochrome(struct ntg_border_9x* style, nt_color color);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_basic(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_basic_edge(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_single(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_double(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_rounded(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_heavy(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_dashed(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_ascii(
        struct ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_transparent(struct ntg_border_9x* style);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_custom(
        struct ntg_border_9x* style,
        enum ntg_vcell_type type,
        struct nt_gfx gfx,
        const struct ntg_border_9x_sym* symbols);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_deinit(struct ntg_border_9x* style);

NTG_API void
ntg_border_9x_deinit_void(void* _style);

#endif // NTG_BORDER_9X_H
