#ifndef NTG_BORDER_STYLE_H
#define NTG_BORDER_STYLE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_border_style
{
    void (*draw_fn)(
            const void* data,
            struct ntg_xy size,
            struct ntg_insets border_size,
            ntg_object_tmp_drawing* out_drawing);
    void* data;
    void (*free_fn)(void* data);
};

const struct ntg_border_style* ntg_border_style_def();

struct ntg_border_style_9x_sym
{
    uint32_t top_left, top, top_right, right,
        bottom_right, bottom, bottom_left,
        left, padding;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

// Calls the style's free_fn on data. Zero-initializes the struct.
void ntg_border_style_deinit(struct ntg_border_style* style);
void ntg_border_style_deinit_(void* _style);

/* -------------------------------------------------------------------------- */
/* 9X */
/* -------------------------------------------------------------------------- */

void ntg_border_style_init_monochrome(
        struct ntg_border_style* style,
        struct nt_color color);

void ntg_border_style_init_basic(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay);

void ntg_border_style_init_basic_edge(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay);

void ntg_border_style_init_single(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay);

void ntg_border_style_init_double(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay);

void ntg_border_style_init_rounded(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay);

void ntg_border_style_init_heavy(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay);

void ntg_border_style_init_dashed(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay);

void ntg_border_style_init_ascii(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool overlay);

void ntg_border_style_init_transparent(
        struct ntg_border_style* style);

void ntg_border_style_init_custom_9x(
        struct ntg_border_style* style, 
        ntg_vcell_type type,
        struct nt_gfx gfx,
        const struct ntg_border_style_9x_sym* symbols);

#endif // NTG_BORDER_STYLE_H
