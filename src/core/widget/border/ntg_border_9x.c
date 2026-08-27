#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void border_9x_draw_fn(
        const ntg_border_style* base,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_widget_tmp_draw* out_draw);

static void border_9x_deinit_fn(ntg_object* _style);

static const struct ntg_border_style_vtable VTABLE = {
    .draw_fn = border_9x_draw_fn,
    .base.deinit_fn = border_9x_deinit_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_border_9x_init_monochrome(ntg_border_9x* bs, nt_color color)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct nt_gfx gfx = {
        .fg = NT_COLOR_ZERO,
        .bg = color,
        .style = 0
    };

    struct ntg_border_9x_sym symbols = {0};

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

/* ------------------------------------------------------ */
/* BASIC */
/* ------------------------------------------------------ */

int ntg_border_9x_init_basic(ntg_border_9x* bs, uint32_t cp, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = cp,
        .top = cp,
        .top_right = cp,
        .right = cp,
        .bottom_right = cp,
        .bottom = cp,
        .bottom_left = cp,
        .left = cp,
        .padding = cp
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_basic_overlay(
        ntg_border_9x* bs,
        uint32_t cp,
        nt_color fg,
        nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = cp,
        .top = cp,
        .top_right = cp,
        .right = cp,
        .bottom_right = cp,
        .bottom = cp,
        .bottom_left = cp,
        .left = cp,
        .padding = cp
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* BASIC EDGE */
/* ------------------------------------------------------ */

int ntg_border_9x_init_basic_edge(ntg_border_9x* bs, struct nt_gfx gfx, uint32_t cp)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = cp,
        .top = cp,
        .top_right = cp,
        .right = cp,
        .bottom_right = cp,
        .bottom = cp,
        .bottom_left = cp,
        .left = cp,
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_basic_edge_overlay(
        ntg_border_9x* bs,
        nt_color fg,
        nt_style style,
        uint32_t cp)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = cp,
        .top = cp,
        .top_right = cp,
        .right = cp,
        .bottom_right = cp,
        .bottom = cp,
        .bottom_left = cp,
        .left = cp,
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* SINGLE */
/* ------------------------------------------------------ */

int ntg_border_9x_init_single(ntg_border_9x* bs, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250C,
        .top = 0x2500,
        .top_right = 0x2510,
        .right = 0x2502,
        .bottom_right = 0x2518,
        .bottom = 0x2500,
        .bottom_left = 0x2514,
        .left = 0x2502,
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_single_overlay(ntg_border_9x* bs, nt_color fg, nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250C,
        .top = 0x2500,
        .top_right = 0x2510,
        .right = 0x2502,
        .bottom_right = 0x2518,
        .bottom = 0x2500,
        .bottom_left = 0x2514,
        .left = 0x2502,
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* DOUBLE */
/* ------------------------------------------------------ */

int ntg_border_9x_init_double(ntg_border_9x* bs, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x2554,
        .top = 0x2550,
        .top_right = 0x2557,
        .right = 0x2551,
        .bottom_right = 0x255D,
        .bottom = 0x2550,
        .bottom_left = 0x255A,
        .left = 0x2551,
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_double_overlay(ntg_border_9x* bs, nt_color fg, nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x2554,
        .top = 0x2550,
        .top_right = 0x2557,
        .right = 0x2551,
        .bottom_right = 0x255D,
        .bottom = 0x2550,
        .bottom_left = 0x255A,
        .left = 0x2551,
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* ROUNDED */
/* ------------------------------------------------------ */

int ntg_border_9x_init_rounded(ntg_border_9x* bs, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x256D,
        .top = 0x2500,
        .top_right = 0x256E,
        .right = 0x2502,
        .bottom_right = 0x256F,
        .bottom = 0x2500,
        .bottom_left = 0x2570,
        .left = 0x2502,
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_rounded_overlay(ntg_border_9x* bs, nt_color fg, nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x256D,
        .top = 0x2500,
        .top_right = 0x256E,
        .right = 0x2502,
        .bottom_right = 0x256F,
        .bottom = 0x2500,
        .bottom_left = 0x2570,
        .left = 0x2502,
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* HEAVY */
/* ------------------------------------------------------ */

int ntg_border_9x_init_heavy(ntg_border_9x* bs, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250F,
        .top = 0x2501,
        .top_right = 0x2513,
        .right = 0x2503,
        .bottom_right = 0x251B,
        .bottom = 0x2501,
        .bottom_left = 0x2517,
        .left = 0x2503,
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_heavy_overlay(ntg_border_9x* bs, nt_color fg, nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250F,
        .top = 0x2501,
        .top_right = 0x2513,
        .right = 0x2503,
        .bottom_right = 0x251B,
        .bottom = 0x2501,
        .bottom_left = 0x2517,
        .left = 0x2503,
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* DASHED */
/* ------------------------------------------------------ */

int ntg_border_9x_init_dashed(ntg_border_9x* bs, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250C,
        .top = 0x254C,
        .top_right = 0x2510,
        .right = 0x254E,
        .bottom_right = 0x2518,
        .bottom = 0x254C,
        .bottom_left = 0x2514,
        .left = 0x254E,
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_dashed_overlay(ntg_border_9x* bs, nt_color fg, nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250C,
        .top = 0x254C,
        .top_right = 0x2510,
        .right = 0x254E,
        .bottom_right = 0x2518,
        .bottom = 0x254C,
        .bottom_left = 0x2514,
        .left = 0x254E,
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* ASCII */
/* ------------------------------------------------------ */

int ntg_border_9x_init_ascii(ntg_border_9x* bs, struct nt_gfx gfx)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = '+',
        .top = '-',
        .top_right = '+',
        .right = '|',
        .bottom_right = '+',
        .bottom = '-',
        .bottom_left = '+',
        .left = '|',
        .padding = 0
    };

    return ntg_border_9x_init_custom(bs, gfx, &symbols);
}

int ntg_border_9x_init_ascii_overlay(ntg_border_9x* bs, nt_color fg, nt_style style)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {
        .top_left = '+',
        .top = '-',
        .top_right = '+',
        .right = '|',
        .bottom_right = '+',
        .bottom = '-',
        .bottom_left = '+',
        .left = '|',
        .padding = 0
    };

    return ntg_border_9x_init_custom_overlay(bs, fg, style, &symbols);
}

/* ------------------------------------------------------ */
/* TRANSPARENT */
/* ------------------------------------------------------ */

int ntg_border_9x_init_transparent(ntg_border_9x* bs)
{
    if(!bs) return NTG_ERR_INV_ARG;

    struct ntg_border_9x_sym symbols = {0};

    return ntg_border_9x_init_custom_overlay(bs, NT_COLOR_ZERO, 0, &symbols);
}

/* ------------------------------------------------------ */
/* CUSTOM */
/* ------------------------------------------------------ */

int ntg_border_9x_init_custom(
        ntg_border_9x* bs,
        struct nt_gfx gfx,
        const struct ntg_border_9x_sym* symbols)
{
    if(!bs || !symbols)
        return NTG_ERR_INV_ARG;

    int status = ntg_border_style_init_inherit(
            ntg_bs(bs),
            &VTABLE,
            &NTG_TYPE_BORDER_9X);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(bs);
    bs->ro.overlay = false;
    bs->ro.data.full.gfx = gfx;
    bs->ro.symbols = (*symbols);

    return 0;
}

int ntg_border_9x_init_custom_overlay(
        ntg_border_9x* bs,
        nt_color fg,
        nt_style style,
        const struct ntg_border_9x_sym* symbols)
{
    if(!bs || !symbols)
        return NTG_ERR_INV_ARG;

    int status = ntg_border_style_init_inherit(
            ntg_bs(bs),
            &VTABLE,
            &NTG_TYPE_BORDER_9X);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(bs);
    bs->ro.overlay = true;
    bs->ro.data.overlay.fg = fg;
    bs->ro.data.overlay.style = style;
    bs->ro.symbols = (*symbols);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_border_9x_deinit(ntg_border_9x* style)
{
    if(!style) return NTG_ERR_INV_ARG;

    ntg_object_zero(style);

    ntg_border_style_deinit(&style->_base);

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void border_9x_draw_fn(
        const ntg_border_style* base,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_widget_tmp_draw* out_draw)
{
    if(ntg_xy_is_zero_any(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    const ntg_border_9x* style = (const ntg_border_9x*)base;

    size_t i, j;
    size_t x_end = size.x - 1;
    size_t y_end = size.y - 1;

    struct ntg_vcell top_left, top, top_right, right,
                     bottom_right, bottom, bottom_left,
                     left, padding;

    const struct ntg_border_9x_sym* symbols = &style->ro.symbols;

    top_left = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->top_left, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->top_left, style->ro.data.full.gfx);
    top = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->top, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->top, style->ro.data.full.gfx);
    top_right = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->top_right, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->top_right, style->ro.data.full.gfx);
    bottom_right = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->bottom_right, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->bottom_right, style->ro.data.full.gfx);
    bottom_left = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->bottom_left, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->bottom_left, style->ro.data.full.gfx);
    bottom = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->bottom, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->bottom, style->ro.data.full.gfx);
    left = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->left, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->left, style->ro.data.full.gfx);
    right = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->right, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->right, style->ro.data.full.gfx);
    padding = style->ro.overlay ?
            ntg_vcell_new_overlay(symbols->padding, style->ro.data.overlay.fg, style->ro.data.overlay.style) :
            ntg_vcell_new_full(symbols->padding, style->ro.data.full.gfx);

    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
            ntg_widget_tmp_draw_set(out_draw, padding, ntg_xy(j, i));
    }

    if(size.y >= 1)
    {
        if(border_size.w > 0)
        {
            for(i = 0; i < size.y; i++)
                ntg_widget_tmp_draw_set(out_draw, left, ntg_xy(0, i));
        }

        if(border_size.e > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                ntg_widget_tmp_draw_set(out_draw, right, ntg_xy(x_end, i));
            }
        }
    }

    if(size.x >= 3)
    {
        if(border_size.n > 0)
        {
            ntg_widget_tmp_draw_set(out_draw, top_left, ntg_xy(0, 0));
            for(j = 1; j < x_end; j++)
                ntg_widget_tmp_draw_set(out_draw, top, ntg_xy(j, 0));
            ntg_widget_tmp_draw_set(out_draw, top_right, ntg_xy(x_end, 0));
        }

        if(border_size.s > 0)
        {
            ntg_widget_tmp_draw_set(out_draw, bottom_left, ntg_xy(0, y_end));
            for(j = 1; j < x_end; j++)
            {
                ntg_widget_tmp_draw_set(out_draw, bottom, ntg_xy(j, y_end));
            }
            ntg_widget_tmp_draw_set(out_draw, bottom_right, ntg_xy(x_end, y_end));
        }
    }
    else if(size.x >= 1)
    {
        if(border_size.n > 0)
        {
            for(j = 0; j < size.x; j++)
                ntg_widget_tmp_draw_set(out_draw, top, ntg_xy(j, 0));
        }

        if(border_size.s > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                ntg_widget_tmp_draw_set(out_draw, bottom, ntg_xy(j, y_end));
            }
        }
    }
}

static void border_9x_deinit_fn(ntg_object* _style)
{
    ntg_border_9x_deinit((ntg_border_9x*)_style);
}
