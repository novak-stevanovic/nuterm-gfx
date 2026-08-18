#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

#define BORDER_9X_SYM_DEFAULT (struct ntg_border_9x_sym) {                    \
    .top_left     = ' ',                                                       \
    .top          = ' ',                                                       \
    .top_right    = ' ',                                                       \
    .right        = ' ',                                                       \
    .bottom_right = ' ',                                                       \
    .bottom       = ' ',                                                       \
    .bottom_left  = ' ',                                                       \
    .left         = ' ',                                                       \
    .padding      = ' '                                                        \
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct border_9x_data
{
    enum ntg_vcell_type type;
    struct nt_gfx gfx;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void border_9x_draw_fn(
        const ntg_border_style* base,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing);

static void border_9x_deinit_fn(ntg_border_style* base);

static const struct ntg_border_style_vtable VTABLE = {
    .draw_fn = border_9x_draw_fn,
    .deinit_fn = border_9x_deinit_fn
};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

int ntg_border_9x_init_monochrome(
        ntg_border_9x* style,
        struct nt_color color)
{
    struct nt_gfx gfx = {
        .fg = NT_COLOR_DEFAULT,
        .bg = color,
        .style = NT_STYLE_DEFAULT
    };

    struct ntg_border_9x_sym symbols = BORDER_9X_SYM_DEFAULT;

    return ntg_border_9x_init_custom_9x(
            style,
            NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_basic(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay)
{
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

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_basic_edge(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        uint32_t cp,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = cp,
        .top = cp,
        .top_right = cp,
        .right = cp,
        .bottom_right = cp,
        .bottom = cp,
        .bottom_left = cp,
        .left = cp,
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_single(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250C,
        .top = 0x2500,
        .top_right = 0x2510,
        .right = 0x2502,
        .bottom_right = 0x2518,
        .bottom = 0x2500,
        .bottom_left = 0x2514,
        .left = 0x2502,
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_double(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = 0x2554,
        .top = 0x2550,
        .top_right = 0x2557,
        .right = 0x2551,
        .bottom_right = 0x255D,
        .bottom = 0x2550,
        .bottom_left = 0x255A,
        .left = 0x2551,
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_rounded(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = 0x256D,
        .top = 0x2500,
        .top_right = 0x256E,
        .right = 0x2502,
        .bottom_right = 0x256F,
        .bottom = 0x2500,
        .bottom_left = 0x2570,
        .left = 0x2502,
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_heavy(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250F,
        .top = 0x2501,
        .top_right = 0x2513,
        .right = 0x2503,
        .bottom_right = 0x251B,
        .bottom = 0x2501,
        .bottom_left = 0x2517,
        .left = 0x2503,
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_dashed(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = 0x250C,
        .top = 0x254C,
        .top_right = 0x2510,
        .right = 0x254E,
        .bottom_right = 0x2518,
        .bottom = 0x254C,
        .bottom_left = 0x2514,
        .left = 0x254E,
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_ascii(
        ntg_border_9x* style,
        struct nt_gfx gfx,
        bool overlay)
{
    struct ntg_border_9x_sym symbols = {
        .top_left = '+',
        .top = '-',
        .top_right = '+',
        .right = '|',
        .bottom_right = '+',
        .bottom = '-',
        .bottom_left = '+',
        .left = '|',
        .padding = ' '
    };

    return ntg_border_9x_init_custom_9x(
            style,
            overlay ? NTG_VCELL_OVERLAY : NTG_VCELL_FULL,
            gfx,
            &symbols);
}

int ntg_border_9x_init_transparent(
        ntg_border_9x* style)
{
    struct ntg_border_9x_sym symbols = BORDER_9X_SYM_DEFAULT;

    return ntg_border_9x_init_custom_9x(
            style,
            NTG_VCELL_TRANSPARENT,
            NT_GFX_DEFAULT,
            &symbols);
}

int ntg_border_9x_init_custom_9x(
        ntg_border_9x* style,
        enum ntg_vcell_type type,
        struct nt_gfx gfx,
        const struct ntg_border_9x_sym* symbols)
{
    if(!style)
        return NTG_ERR_INV_ARG;

    int _status = ntg_border_style_init_inherit(&style->__base, &VTABLE);
    if(_status)
        return _status;

    struct border_9x_data* data = malloc(sizeof(*data));
    if(!data)
    {
        ntg_border_style_deinit(&style->__base);
        return NTG_ERR_ALLOC_FAIL;
    }

    (*data) = (struct border_9x_data) {
        .type = type,
        .gfx = gfx
    };

    style->__base.data = data;
    style->_symbols = symbols ? (*symbols) : BORDER_9X_SYM_DEFAULT;
    return 0;
}

void ntg_border_9x_deinit(ntg_border_9x* style)
{
    if(!style) return;

    free(style->__base.data);
    style->__base.data = NULL;
    style->_symbols = BORDER_9X_SYM_DEFAULT;

    ntg_border_style_deinit(&style->__base);
}

void ntg_border_9x_deinit_void(void* _style)
{
    ntg_border_9x_deinit(_style);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void border_9x_draw_fn(
        const ntg_border_style* base,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing)
{
    if(ntg_xy_size_is_zero(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    const ntg_border_9x* style = (const ntg_border_9x*)base;
    const struct border_9x_data* data = base->data;
    if(!data) return;

    size_t i, j;
    size_t x_end = size.x - 1;
    size_t y_end = size.y - 1;

    struct ntg_vcell top_left, top, top_right, right;
    struct ntg_vcell bottom_right, bottom, bottom_left, left, padding;

    top_left = ntg_vcell_new(
            data->type,
            data->gfx,
            style->_symbols.top_left);
    top = ntg_vcell_new(data->type, data->gfx, style->_symbols.top);
    top_right = ntg_vcell_new(
            data->type,
            data->gfx,
            style->_symbols.top_right);
    right = ntg_vcell_new(data->type, data->gfx, style->_symbols.right);
    bottom_right = ntg_vcell_new(
            data->type,
            data->gfx,
            style->_symbols.bottom_right);
    bottom = ntg_vcell_new(data->type, data->gfx, style->_symbols.bottom);
    bottom_left = ntg_vcell_new(
            data->type,
            data->gfx,
            style->_symbols.bottom_left);
    left = ntg_vcell_new(data->type, data->gfx, style->_symbols.left);
    padding = ntg_vcell_new(data->type, data->gfx, style->_symbols.padding);

    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
            ntg_object_tmp_drawing_set(out_drawing, padding, ntg_xy(j, i));
    }

    if(size.y >= 1)
    {
        if(border_size.w > 0)
        {
            for(i = 0; i < size.y; i++)
                ntg_object_tmp_drawing_set(out_drawing, left, ntg_xy(0, i));
        }

        if(border_size.e > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                ntg_object_tmp_drawing_set(
                        out_drawing,
                        right,
                        ntg_xy(x_end, i));
            }
        }
    }

    if(size.x >= 3)
    {
        if(border_size.n > 0)
        {
            ntg_object_tmp_drawing_set(out_drawing, top_left, ntg_xy(0, 0));
            for(j = 1; j < x_end; j++)
                ntg_object_tmp_drawing_set(out_drawing, top, ntg_xy(j, 0));
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    top_right,
                    ntg_xy(x_end, 0));
        }

        if(border_size.s > 0)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    bottom_left,
                    ntg_xy(0, y_end));
            for(j = 1; j < x_end; j++)
            {
                ntg_object_tmp_drawing_set(
                        out_drawing,
                        bottom,
                        ntg_xy(j, y_end));
            }
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    bottom_right,
                    ntg_xy(x_end, y_end));
        }
    }
    else if(size.x >= 1)
    {
        if(border_size.n > 0)
        {
            for(j = 0; j < size.x; j++)
                ntg_object_tmp_drawing_set(out_drawing, top, ntg_xy(j, 0));
        }

        if(border_size.s > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                ntg_object_tmp_drawing_set(
                        out_drawing,
                        bottom,
                        ntg_xy(j, y_end));
            }
        }
    }
}

static void border_9x_deinit_fn(ntg_border_style* base)
{
    ntg_border_9x_deinit((ntg_border_9x*)base);
}
