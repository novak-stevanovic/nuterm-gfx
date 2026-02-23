#include "ntg.h"
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void border_def_style_draw_fn(
        const void* _data,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing);

static void border_9x_style_draw_fn(
        const void* _data,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

const struct ntg_border_style NTG_BORDER_STYLE_DEF = {
    .free_fn = NULL,
    .data = NULL,
    .draw_fn = border_def_style_draw_fn
};

const struct ntg_border_style* ntg_border_style_def()
{
    return &NTG_BORDER_STYLE_DEF;
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

void ntg_border_style_deinit(struct ntg_border_style* style)
{
    if(!style) return;

    if(style->free_fn)
        style->free_fn(style->data);

    (*style) = (struct ntg_border_style) {0};
}

void ntg_border_style_deinit_(void* _style)
{
    ntg_border_style_deinit(_style);
}

/* -------------------------------------------------------------------------- */
/* 9X */
/* -------------------------------------------------------------------------- */

#define BORDER_STYLE_9X_SYM_DEF (struct ntg_border_style_9x_sym) { \
    .top_left     = ' ', \
    .top          = ' ', \
    .top_right    = ' ', \
    .right        = ' ', \
    .bottom_right = ' ', \
    .bottom       = ' ', \
    .bottom_left  = ' ', \
    .left         = ' ', \
    .padding      = ' '  \
}

void ntg_border_style_init_monochrome(
        struct ntg_border_style* style,
        struct nt_color color)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = BORDER_STYLE_9X_SYM_DEF;

    struct nt_gfx gfx = (struct nt_gfx){
        .fg = NT_COLOR_DEFAULT,
        .bg = color,
        .style = NT_STYLE_DEFAULT
    };

    ntg_border_style_init_custom_9x(style, NTG_VCELL_FULL, gfx, &sym);
}

void ntg_border_style_init_basic(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = cp,
        .top          = cp,
        .top_right    = cp,
        .right        = cp,
        .bottom_right = cp,
        .bottom       = cp,
        .bottom_left  = cp,
        .left         = cp,
        .padding      = cp
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_basic_edge(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = cp,
        .top          = cp,
        .top_right    = cp,
        .right        = cp,
        .bottom_right = cp,
        .bottom       = cp,
        .bottom_left  = cp,
        .left         = cp,
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_single(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = 0x250C, /* ┌ */
        .top          = 0x2500, /* ─ */
        .top_right    = 0x2510, /* ┐ */
        .right        = 0x2502, /* │ */
        .bottom_right = 0x2518, /* ┘ */
        .bottom       = 0x2500, /* ─ */
        .bottom_left  = 0x2514, /* └ */
        .left         = 0x2502, /* │ */
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_double(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = 0x2554, /* ╔ */
        .top          = 0x2550, /* ═ */
        .top_right    = 0x2557, /* ╗ */
        .right        = 0x2551, /* ║ */
        .bottom_right = 0x255D, /* ╝ */
        .bottom       = 0x2550, /* ═ */
        .bottom_left  = 0x255A, /* ╚ */
        .left         = 0x2551, /* ║ */
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_rounded(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = 0x256D, /* ╭ */
        .top          = 0x2500, /* ─ */
        .top_right    = 0x256E, /* ╮ */
        .right        = 0x2502, /* │ */
        .bottom_right = 0x256F, /* ╯ */
        .bottom       = 0x2500, /* ─ */
        .bottom_left  = 0x2570, /* ╰ */
        .left         = 0x2502, /* │ */
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_heavy(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = 0x250F, /* ┏ */
        .top          = 0x2501, /* ━ */
        .top_right    = 0x2513, /* ┓ */
        .right        = 0x2503, /* ┃ */
        .bottom_right = 0x251B, /* ┛ */
        .bottom       = 0x2501, /* ━ */
        .bottom_left  = 0x2517, /* ┗ */
        .left         = 0x2503, /* ┃ */
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_dashed(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = 0x250C, /* fallback ┌ */
        .top          = 0x254C, /* ╌ (U+254C) */
        .top_right    = 0x2510, /* ┐ */
        .right        = 0x254E, /* ╎ (U+254E) */
        .bottom_right = 0x2518, /* ┘ */
        .bottom       = 0x254C, /* ╌ */
        .bottom_left  = 0x2514, /* └ */
        .left         = 0x254E, /* ╎ */
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_ascii(
        struct ntg_border_style* style,
        struct nt_gfx gfx,
        bool flt)
{
    if(!style) return;

    struct ntg_border_style_9x_sym sym = {
        .top_left     = (uint32_t)'+',
        .top          = (uint32_t)'-',
        .top_right    = (uint32_t)'+',
        .right        = (uint32_t)'|',
        .bottom_right = (uint32_t)'+',
        .bottom       = (uint32_t)'-',
        .bottom_left  = (uint32_t)'+',
        .left         = (uint32_t)'|',
        .padding      = ' '
    };

    ntg_vcell_type type = (flt ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL);
    ntg_border_style_init_custom_9x(style, type, gfx, &sym);
}

void ntg_border_style_init_transparent(
        struct ntg_border_style* style)
{
    struct ntg_border_style_9x_sym sym = BORDER_STYLE_9X_SYM_DEF;

    ntg_border_style_init_custom_9x(style, NTG_VCELL_TRANSPARENT, NT_GFX_DEFAULT, &sym);
}

struct ntg_border_style_9x_dt
{
    struct ntg_border_style_9x_sym symbols;
    struct nt_gfx gfx;
    ntg_vcell_type type;
};

void ntg_border_style_init_custom_9x(
        struct ntg_border_style* style,
        ntg_vcell_type type,
        struct nt_gfx gfx,
        const struct ntg_border_style_9x_sym* symbols)
{
    if(!style) return;

    (*style) = (struct ntg_border_style) {0};

    struct ntg_border_style_9x_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_border_style_9x_dt) {
        .type = type,
        .gfx = gfx,
        .symbols = (symbols ? (*symbols) : BORDER_STYLE_9X_SYM_DEF)
    };

    (*style) = (struct ntg_border_style) {
        .data = data,
        .draw_fn = border_9x_style_draw_fn,
        .free_fn = free
    };
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void border_def_style_draw_fn(
        const void* _data,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing)
{
    if(ntg_xy_size_is_zero(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    ntg_vcell_default(),
                    ntg_xy(j, i));
        }
    }
}

static void border_9x_style_draw_fn(
        const void* _data,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing)
{
    if(ntg_xy_size_is_zero(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    const struct ntg_border_style_9x_dt* data = _data;

    size_t i, j;
    size_t x_end = size.x - 1;
    size_t y_end = size.y - 1;

    struct ntg_vcell top_left, top, top_right, right,
    bottom_right, bottom, bottom_left, left, padding;

    top_left = ntg_vcell_new(data->type, data->gfx, data->symbols.top_left);
    top = ntg_vcell_new(data->type, data->gfx, data->symbols.top);
    top_right = ntg_vcell_new(data->type, data->gfx, data->symbols.top_right);
    right = ntg_vcell_new(data->type, data->gfx, data->symbols.right);
    bottom_right = ntg_vcell_new(data->type, data->gfx, data->symbols.bottom_right);
    bottom = ntg_vcell_new(data->type, data->gfx, data->symbols.bottom);
    bottom_left = ntg_vcell_new(data->type, data->gfx, data->symbols.bottom_left);
    left = ntg_vcell_new(data->type, data->gfx, data->symbols.left);
    padding = ntg_vcell_new(data->type, data->gfx, data->symbols.padding);

    // Draw padding
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(out_drawing, padding, ntg_xy(j, i));
        }
    }

    // Draw west & east
    if(size.y >= 1)
    {
        if(border_size.w > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                ntg_object_tmp_drawing_set(out_drawing, left, ntg_xy(0, i));
            }
        }
        if(border_size.e > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                ntg_object_tmp_drawing_set(out_drawing, right, ntg_xy(x_end, i));
            }
        }
    }

    // Draw north & south
    if(size.x >= 3)
    {
        if(border_size.n > 0)
        {
            ntg_object_tmp_drawing_set(out_drawing, top_left, ntg_xy(0, 0));
            for(j = 1; j < x_end; j++)
            {
                ntg_object_tmp_drawing_set(out_drawing, top, ntg_xy(j, 0));
            }
            ntg_object_tmp_drawing_set(out_drawing, top_right, ntg_xy(x_end, 0));
        }
        if(border_size.s > 0)
        {
            ntg_object_tmp_drawing_set(out_drawing, bottom_left, ntg_xy(0, y_end));
            for(j = 1; j < x_end; j++)
            {
                ntg_object_tmp_drawing_set(out_drawing, bottom, ntg_xy(j, y_end));
            }
            ntg_object_tmp_drawing_set(out_drawing, bottom_right, ntg_xy(x_end, y_end));
        }
    }
    else if(size.x >= 1)
    {
        if(border_size.n > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                ntg_object_tmp_drawing_set(out_drawing, top, ntg_xy(j, 0));
            }
        }
        if(border_size.s > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                ntg_object_tmp_drawing_set(out_drawing, bottom, ntg_xy(j, y_end));
            }
        }
    }
}

