#include "core/object/ntg_def_border.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_xy_map.h"

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
        struct ntg_def_border_opts opts,
        struct ntg_padding_width init_width)
{
    assert(def_border != NULL);

    _ntg_padding_init_((ntg_padding*)def_border,
            init_width,
            _ntg_def_border_draw_fn,
            _ntg_def_border_deinit_fn,
            NULL);

    def_border->__opts = opts;
}

void _ntg_def_border_deinit_(ntg_def_border* def_border)
{
    _ntg_def_border_deinit_fn((ntg_object*)def_border);
}

void _ntg_def_border_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena)
{
    const ntg_def_border* border = (const ntg_def_border*)object;
    const ntg_object* child = (ntg_object_get_children(object))->_data[0];

    struct ntg_xy child_start = ntg_object_xy_map_get(ctx.pos, child);
    struct ntg_xy child_size = ntg_object_xy_map_get(ctx.sizes, child);
    struct ntg_xy child_end = ntg_xy_add(child_start, child_size);

    draw_north(border, size, child_start, child_size, child_end, out->drawing);
    draw_east(border, size, child_start, child_size, child_end, out->drawing);
    draw_south(border, size, child_start, child_size, child_end, out->drawing);
    draw_west(border, size, child_start, child_size, child_end, out->drawing);
}

void _ntg_def_border_deinit_fn(ntg_object* object)
{
    _ntg_padding_deinit_((ntg_padding*)object);

    ntg_def_border* border = (ntg_def_border*)object;

    border->__opts = (struct ntg_def_border_opts) {0};
}

/* -------------------------------------------------------------------------- */

static void draw_north(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{
    struct ntg_padding_width border_width = {
        .north = child_start.y,
        .east = size.x - child_end.x,
        .south = size.y - child_end.y,
        .west = child_start.x
    };

    size_t i, j;
    ntg_cell* it_cell;

    if(size.x >= 3)
    {
        if(border_width.north > 0)
        {
            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(0, 0));
            (*it_cell) = border->__opts.top_left;

            for(i = 1; i < (size.x - 1); i++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(i, 0));
                (*it_cell) = border->__opts.top;
            }

            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(0, size.x - 1));
            (*it_cell) = border->__opts.top_right;
        }

        for(i = 1; i < border_width.north; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(i, j));
                (*it_cell) = border->__opts.padding;
            }
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.north > 0)
        {
            for(i = 0; i < size.x; i++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(i, 0));
                (*it_cell) = border->__opts.top;
            }
        }

        for(i = 1; i < border_width.north; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(i, j));
                (*it_cell) = border->__opts.padding;
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
}
static void draw_south(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{
}
static void draw_west(const ntg_def_border* border,
        struct ntg_xy size,
        struct ntg_xy child_start,
        struct ntg_xy child_size,
        struct ntg_xy child_end,
        ntg_object_drawing* out_drawing)
{
}
