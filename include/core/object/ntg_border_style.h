#ifndef NTG_BORDER_STYLE_H
#define NTG_BORDER_STYLE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

struct ntg_border_style
{
    ntg_border_style_draw_fn __draw_fn;
};

void ntg_border_style_init(ntg_border_style* style);

        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing);
void ntg_border_style_draw(const ntg_border_style* style, struct ntg_xy size, struct ntg_insets border_size, 

#endif // NTG_BORDER_STYLE_H
