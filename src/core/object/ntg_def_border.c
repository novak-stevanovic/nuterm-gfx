#include "core/object/ntg_def_border.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_xy_map.h"

static struct ntg_padding_width get_border_width(
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end);
static void draw_padding(const ntg_def_border* border,
        struct ntg_xy size, ntg_object_drawing* out_drawing);
static void draw_north(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing);
static void draw_east(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing);
static void draw_south(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing);
static void draw_west(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing);

/* -------------------------------------------------------------------------- */

void _ntg_def_border_init_(
        ntg_def_border* def_border,
        struct ntg_def_border_style opts,
        struct ntg_padding_width init_width,
        ntg_object_container* container)
{
    assert(def_border != NULL);

    _ntg_padding_init_((ntg_padding*)def_border,
            NTG_PADDING_BORDER,
            init_width,
            _ntg_def_border_draw_fn,
            _ntg_def_border_deinit_fn,
            NULL,
            container);

    def_border->__style = opts;
}

/* ------------------------------------------------------ */
/* PRESETS */
/* ------------------------------------------------------ */

void _ntg_def_border_init_monochrome_(
        ntg_def_border* def_border,
        nt_color color,
        struct ntg_padding_width init_width,
        ntg_object_container* container)
{
    struct ntg_def_border_style opts = {
        .top_left = ntg_cell_bg(color),
        .top = ntg_cell_bg(color),
        .top_right = ntg_cell_bg(color),
        .right = ntg_cell_bg(color),
        .bottom_right = ntg_cell_bg(color),
        .bottom = ntg_cell_bg(color),
        .bottom_left = ntg_cell_bg(color),
        .left = ntg_cell_bg(color),
        .padding = ntg_cell_bg(color),
    };
    _ntg_def_border_init_(def_border, opts, init_width, container);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_border_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena)
{
    const ntg_def_border* border = (const ntg_def_border*)object;
    const ntg_object* child = ntg_object_get_children(object).data[0];

    struct ntg_xy child_start = ntg_object_xy_map_get(ctx.pos, child);
    struct ntg_xy child_size = ntg_object_xy_map_get(ctx.sizes, child);
    struct ntg_xy child_end = ntg_xy_add(child_start, child_size);

    draw_padding(border, size, out->drawing);
    draw_west(border, size, child_start, child_size, child_end, out->drawing);
    draw_east(border, size, child_start, child_size, child_end, out->drawing);
    draw_north(border, size, child_start, child_size, child_end, out->drawing);
    draw_south(border, size, child_start, child_size, child_end, out->drawing);
}

void _ntg_def_border_deinit_fn(ntg_object* object)
{
    _ntg_padding_deinit_fn(object);

    ntg_def_border* border = (ntg_def_border*)object;

    border->__style = (struct ntg_def_border_style) {0};
}

/* -------------------------------------------------------------------------- */

static struct ntg_padding_width get_border_width(
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end)
{
    return (struct ntg_padding_width) {
        .north = child_start.y,
        .east = size.x - child_end.x,
        .south = size.y - child_end.y,
        .west = child_start.x
    };
}

static void draw_padding(const ntg_def_border* border,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = border->__style.padding;
        }
    }
}

static void draw_north(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{
    struct ntg_padding_width border_width = get_border_width(size,
            child_start, child_size, child_end);

    size_t j;
    ntg_cell* it_cell;

    if(size.x >= 3)
    {
        if(border_width.north > 0)
        {
            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(0, 0));
            (*it_cell) = border->__style.top_left;

            for(j = 1; j < (size.x - 1); j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, 0));
                (*it_cell) = border->__style.top;
            }

            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, 0));
            (*it_cell) = border->__style.top_right;
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.north > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, 0));
                (*it_cell) = border->__style.top;
            }
        }
    }
}

static void draw_east(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{

    struct ntg_padding_width border_width = get_border_width(size,
            child_start, child_size, child_end);

    size_t i;
    ntg_cell* it_cell;

    if(size.y >= 3)
    {
        if(border_width.east > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, i));
                (*it_cell) = border->__style.right;
            }
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.east > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, i));
                (*it_cell) = border->__style.right;
            }
        }
    }
}

static void draw_south(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{
    struct ntg_padding_width border_width = get_border_width(size,
            child_start, child_size, child_end);

    size_t j;
    ntg_cell* it_cell;

    if(size.x >= 3)
    {
        if(border_width.south > 0)
        {
            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(0, size.y - 1));
            (*it_cell) = border->__style.bottom_left;

            for(j = 1; j < (size.x - 1); j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, size.y - 1));
                (*it_cell) = border->__style.top;
            }

            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, size.y - 1));
            (*it_cell) = border->__style.top_right;
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.south > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, size.y - 1));
                (*it_cell) = border->__style.top;
            }
        }
    }
}

static void draw_west(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{
    struct ntg_padding_width border_width = get_border_width(size,
            child_start, child_size, child_end);

    size_t i;
    ntg_cell* it_cell;

    if(size.y >= 3)
    {
        if(border_width.west > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(0, i));
                (*it_cell) = border->__style.left;
            }
        }

    }
    else if(size.y >= 1)
    {
        if(border_width.west > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(0, i));
                (*it_cell) = border->__style.left;
            }
        }
    }
}
