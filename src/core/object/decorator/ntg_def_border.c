#include "ntg.h"
#include <assert.h>

static struct ntg_decorator_width 
get_border_width(struct ntg_xy size, struct ntg_xy child_start,
                 struct ntg_xy child_size, struct ntg_xy child_end);

static void 
draw_padding(const ntg_def_border* border, struct ntg_xy size,
             ntg_tmp_object_drawing* out_drawing);

static void 
draw_north(const ntg_def_border* border, struct ntg_xy size,
           struct ntg_xy child_start, struct ntg_xy child_size,
           struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing);

static void 
draw_east(const ntg_def_border* border, struct ntg_xy size,
          struct ntg_xy child_start, struct ntg_xy child_size,
          struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing);

static void 
draw_south(const ntg_def_border* border, struct ntg_xy size,
           struct ntg_xy child_start, struct ntg_xy child_size,
           struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing);

static void 
draw_west(const ntg_def_border* border, struct ntg_xy size,
          struct ntg_xy child_start, struct ntg_xy child_size,
          struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_border* ntg_def_border_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_BORDER,
        .deinit_fn = ntg_def_border_deinit_fn,
        .system = system
    };

    ntg_def_border* new = (ntg_def_border*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_def_border_init(ntg_def_border* def_border)
{
    assert(def_border != NULL);

    ntg_decorator_init((ntg_decorator*)def_border, _ntg_def_border_draw_fn);

    def_border->_style = ntg_def_border_style_def();
}

void ntg_def_border_set_style(ntg_def_border* border, struct ntg_def_border_style style)
{
    assert(border != NULL);

    border->_style = style;

    ntg_entity_raise_event_((ntg_entity*)border, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* ------------------------------------------------------ */
/* PRESETS */
/* ------------------------------------------------------ */

struct ntg_def_border_style 
ntg_def_border_style_def()
{
    return (struct ntg_def_border_style) {
        .top_left = ntg_vcell_default(),
        .top = ntg_vcell_default(),
        .top_right = ntg_vcell_default(),
        .right = ntg_vcell_default(),
        .bottom_right = ntg_vcell_default(),
        .bottom = ntg_vcell_default(),
        .bottom_left = ntg_vcell_default(),
        .left = ntg_vcell_default(),
        .padding = ntg_vcell_default()
    };
}

struct ntg_def_border_style 
ntg_def_border_style_monochrome(nt_color color)
{
    return (struct ntg_def_border_style) {
        .top_left = ntg_vcell_bg(color),
        .top = ntg_vcell_bg(color),
        .top_right = ntg_vcell_bg(color),
        .right = ntg_vcell_bg(color),
        .bottom_right = ntg_vcell_bg(color),
        .bottom = ntg_vcell_bg(color),
        .bottom_left = ntg_vcell_bg(color),
        .left = ntg_vcell_bg(color),
        .padding = ntg_vcell_bg(color),
    };
}

struct ntg_def_border_style
ntg_def_border_style_uniform(struct nt_gfx gfx, uint32_t codepoint)
{
    return (struct ntg_def_border_style) {
        .top_left = ntg_vcell_full(codepoint, gfx),
        .top = ntg_vcell_full(codepoint, gfx),
        .top_right = ntg_vcell_full(codepoint, gfx),
        .right = ntg_vcell_full(codepoint, gfx),
        .bottom_right = ntg_vcell_full(codepoint, gfx),
        .bottom = ntg_vcell_full(codepoint, gfx),
        .bottom_left = ntg_vcell_full(codepoint, gfx),
        .left = ntg_vcell_full(codepoint, gfx),
        .padding = ntg_vcell_full(codepoint, gfx),
    };
}

struct ntg_def_border_style
ntg_def_border_style_uniform_edge(struct nt_gfx gfx, uint32_t codepoint)
{
    return (struct ntg_def_border_style) {
        .top_left = ntg_vcell_full(codepoint, gfx),
        .top = ntg_vcell_full(codepoint, gfx),
        .top_right = ntg_vcell_full(codepoint, gfx),
        .right = ntg_vcell_full(codepoint, gfx),
        .bottom_right = ntg_vcell_full(codepoint, gfx),
        .bottom = ntg_vcell_full(codepoint, gfx),
        .bottom_left = ntg_vcell_full(codepoint, gfx),
        .left = ntg_vcell_full(codepoint, gfx),
        .padding = ntg_vcell_full(' ', gfx),
    };
}

struct ntg_def_border_style
ntg_def_border_style_single(struct nt_gfx gfx)
{
    return (struct ntg_def_border_style) {
        .top_left    = ntg_vcell_full(0x250C, gfx), /* ┌ */
        .top         = ntg_vcell_full(0x2500, gfx), /* ─ */
        .top_right   = ntg_vcell_full(0x2510, gfx), /* ┐ */
        .right       = ntg_vcell_full(0x2502, gfx), /* │ */
        .bottom_right= ntg_vcell_full(0x2518, gfx), /* ┘ */
        .bottom      = ntg_vcell_full(0x2500, gfx), /* ─ */
        .bottom_left = ntg_vcell_full(0x2514, gfx), /* └ */
        .left        = ntg_vcell_full(0x2502, gfx), /* │ */
        .padding     = ntg_vcell_full((uint32_t)' ', gfx)
    };
}

struct ntg_def_border_style
ntg_def_border_style_double(struct nt_gfx gfx)
{
    return (struct ntg_def_border_style) {
        .top_left    = ntg_vcell_full(0x2554, gfx), /* ╔ */
        .top         = ntg_vcell_full(0x2550, gfx), /* ═ */
        .top_right   = ntg_vcell_full(0x2557, gfx), /* ╗ */
        .right       = ntg_vcell_full(0x2551, gfx), /* ║ */
        .bottom_right= ntg_vcell_full(0x255D, gfx), /* ╝ */
        .bottom      = ntg_vcell_full(0x2550, gfx), /* ═ */
        .bottom_left = ntg_vcell_full(0x255A, gfx), /* ╚ */
        .left        = ntg_vcell_full(0x2551, gfx), /* ║ */
        .padding     = ntg_vcell_full((uint32_t)' ', gfx)
    };
}

struct ntg_def_border_style
ntg_def_border_style_rounded(struct nt_gfx gfx)
{
    return (struct ntg_def_border_style) {
        .top_left    = ntg_vcell_full(0x256D, gfx), /* ╭ */
        .top         = ntg_vcell_full(0x2500, gfx), /* ─ */
        .top_right   = ntg_vcell_full(0x256E, gfx), /* ╮ */
        .right       = ntg_vcell_full(0x2502, gfx), /* │ */
        .bottom_right= ntg_vcell_full(0x256F, gfx), /* ╯ */
        .bottom      = ntg_vcell_full(0x2500, gfx), /* ─ */
        .bottom_left = ntg_vcell_full(0x2570, gfx), /* ╰ */
        .left        = ntg_vcell_full(0x2502, gfx), /* │ */
        .padding     = ntg_vcell_full((uint32_t)' ', gfx)
    };
}

/* Heavy / bold */
struct ntg_def_border_style
ntg_def_border_style_heavy(struct nt_gfx gfx)
{
    return (struct ntg_def_border_style) {
        .top_left    = ntg_vcell_full(0x250F, gfx), /* ┏ */
        .top         = ntg_vcell_full(0x2501, gfx), /* ━ */
        .top_right   = ntg_vcell_full(0x2513, gfx), /* ┓ */
        .right       = ntg_vcell_full(0x2503, gfx), /* ┃ */
        .bottom_right= ntg_vcell_full(0x251B, gfx), /* ┛ */
        .bottom      = ntg_vcell_full(0x2501, gfx), /* ━ */
        .bottom_left = ntg_vcell_full(0x2517, gfx), /* ┗ */
        .left        = ntg_vcell_full(0x2503, gfx), /* ┃ */
        .padding     = ntg_vcell_full((uint32_t)' ', gfx)
    };
}

struct ntg_def_border_style
ntg_def_border_style_dashed(struct nt_gfx gfx)
{
    return (struct ntg_def_border_style) {
        .top_left    = ntg_vcell_full(0x250C, gfx), /* fallback ┌ */
        .top         = ntg_vcell_full(0x254C, gfx), /* ╌ dash (U+254C) */
        .top_right   = ntg_vcell_full(0x2510, gfx), /* ┐ */
        .right       = ntg_vcell_full(0x254E, gfx), /* ╎ vertical dashed (U+254E) */
        .bottom_right= ntg_vcell_full(0x2518, gfx),
        .bottom      = ntg_vcell_full(0x254C, gfx),
        .bottom_left = ntg_vcell_full(0x2514, gfx),
        .left        = ntg_vcell_full(0x254E, gfx),
        .padding     = ntg_vcell_full((uint32_t)' ', gfx)
    };
}

struct ntg_def_border_style
ntg_def_border_style_ascii(struct nt_gfx gfx)
{
    /* use '|' '-' '+' and space with same gfx */
    return (struct ntg_def_border_style) {
        .top_left    = ntg_vcell_full((uint32_t)'+', gfx),
        .top         = ntg_vcell_full((uint32_t)'-', gfx),
        .top_right   = ntg_vcell_full((uint32_t)'+', gfx),
        .right       = ntg_vcell_full((uint32_t)'|', gfx),
        .bottom_right= ntg_vcell_full((uint32_t)'+', gfx),
        .bottom      = ntg_vcell_full((uint32_t)'-', gfx),
        .bottom_left = ntg_vcell_full((uint32_t)'+', gfx),
        .left        = ntg_vcell_full((uint32_t)'|', gfx),
        .padding     = ntg_vcell_full((uint32_t)' ', gfx)
    };
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_border_deinit_fn(ntg_entity* entity)
{
    ntg_def_border* border = (ntg_def_border*)entity;

    border->_style = (struct ntg_def_border_style) {0};
    ntg_decorator_deinit_fn(entity);
}

void _ntg_def_border_draw_fn(
        const ntg_object* _border,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena)
{
    const ntg_def_border* border = (const ntg_def_border*)_border;
    const ntg_object* child = _border->_children.data[0];
    struct ntg_xy size = _border->_size;

    struct ntg_xy child_start = child->_pos;
    struct ntg_xy child_size = child->_size;
    struct ntg_xy child_end = ntg_xy_add(child_start, child_size);

    draw_padding(border, size, out_drawing);
    draw_west(border, size, child_start, child_size, child_end, out_drawing);
    draw_east(border, size, child_start, child_size, child_end, out_drawing);
    draw_north(border, size, child_start, child_size, child_end, out_drawing);
    draw_south(border, size, child_start, child_size, child_end, out_drawing);
}

/* -------------------------------------------------------------------------- */

static struct ntg_decorator_width 
get_border_width(struct ntg_xy size, struct ntg_xy child_start,
                 struct ntg_xy child_size, struct ntg_xy child_end)
{
    return (struct ntg_decorator_width) {
        .north = child_start.y,
        .east = size.x - child_end.x,
        .south = size.y - child_end.y,
        .west = child_start.x
    };
}

static void 
draw_padding(const ntg_def_border* border, struct ntg_xy size,
             ntg_tmp_object_drawing* out_drawing)
{
    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = border->_style.padding;
        }
    }
}

static void 
draw_north(const ntg_def_border* border, struct ntg_xy size,
           struct ntg_xy child_start, struct ntg_xy child_size,
           struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing)
{
    struct ntg_decorator_width border_width;
    border_width = get_border_width(size, child_start, child_size, child_end);

    size_t j;
    struct ntg_vcell* it_cell;

    if(size.x >= 3)
    {
        if(border_width.north > 0)
        {
            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(0, 0));
            (*it_cell) = border->_style.top_left;

            for(j = 1; j < (size.x - 1); j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, 0));
                (*it_cell) = border->_style.top;
            }

            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, 0));
            (*it_cell) = border->_style.top_right;
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.north > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, 0));
                (*it_cell) = border->_style.top;
            }
        }
    }
}

static void 
draw_east(const ntg_def_border* border, struct ntg_xy size,
          struct ntg_xy child_start, struct ntg_xy child_size,
          struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing)
{

    struct ntg_decorator_width border_width;
    border_width = get_border_width(size, child_start, child_size, child_end);

    size_t i;
    struct ntg_vcell* it_cell;

    if(size.y >= 3)
    {
        if(border_width.east > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, i));
                (*it_cell) = border->_style.right;
            }
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.east > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, i));
                (*it_cell) = border->_style.right;
            }
        }
    }
}

static void 
draw_south(const ntg_def_border* border, struct ntg_xy size,
           struct ntg_xy child_start, struct ntg_xy child_size,
           struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing)
{
    struct ntg_decorator_width border_width;
    border_width = get_border_width(size, child_start, child_size, child_end);

    size_t j;
    struct ntg_vcell* it_cell;

    if(size.x >= 3)
    {
        if(border_width.south > 0)
        {
            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(0, size.y - 1));
            (*it_cell) = border->_style.bottom_left;

            for(j = 1; j < (size.x - 1); j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, size.y - 1));
                (*it_cell) = border->_style.top;
            }

            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(size.x - 1, size.y - 1));
            (*it_cell) = border->_style.bottom_right;
        }
    }
    else if(size.x >= 1)
    {
        if(border_width.south > 0)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, size.y - 1));
                (*it_cell) = border->_style.top;
            }
        }
    }
}

static void 
draw_west(const ntg_def_border* border, struct ntg_xy size,
          struct ntg_xy child_start, struct ntg_xy child_size,
          struct ntg_xy child_end, ntg_tmp_object_drawing* out_drawing)
{
    struct ntg_decorator_width border_width;
    border_width = get_border_width(size, child_start, child_size, child_end);

    size_t i;
    struct ntg_vcell* it_cell;

    if(size.y >= 3)
    {
        if(border_width.west > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(0, i));
                (*it_cell) = border->_style.left;
            }
        }

    }
    else if(size.y >= 1)
    {
        if(border_width.west > 0)
        {
            for(i = 0; i < size.y; i++)
            {
                it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(0, i));
                (*it_cell) = border->_style.left;
            }
        }
    }
}
