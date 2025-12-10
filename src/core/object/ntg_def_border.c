#include "core/object/ntg_def_border.h"

void __ntg_def_border_init__(
        ntg_def_border* def_border,
        struct ntg_def_border_opts opts,
        struct ntg_padding_width init_width,
        void* data)
{
    assert(def_border != NULL);

    __ntg_padding_init__((ntg_padding*)def_border, __ntg_def_border_draw_fn, init_width, data);

    def_border->__top_left = opts.top_left;
    def_border->__top_left = opts.top_left;
    def_border->__top_left = opts.top_left;
    def_border->__top_left = opts.top_left;
    def_border->__top_left = opts.top_left;
    def_border->__top_left = opts.top_left;
    def_border->__top_left = opts.top_left;
}

void __ntg_def_border_deinit__(ntg_def_border* def_border)
{
}

void __ntg_def_border_draw_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        ntg_drawing* out_drawing,
        sarena* arena)
{
    assert(0);
}
