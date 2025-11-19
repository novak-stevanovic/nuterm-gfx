#include <assert.h>
#include <stdlib.h>

#include "core/object/shared/ntg_object_fx.h"
#include "core/object/shared/ntg_object_fx_vec.h"
#include "core/scene/shared/_ntg_drawing.h"
#include "shared/_ntg_shared.h"

static bool __ntg_object_fx_brighten_bg_fn(
        const ntg_object* object,
        const struct ntg_object_fx fx,
        ntg_drawing* drawing)
{
    struct ntg_xy size = ntg_drawing_get_size(drawing);
    int brightness_adj = *(int*)fx.data;

    size_t i, j;
    ntg_cell* it_cell;
    nt_color it_old, it_new;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_drawing_at_(drawing, ntg_xy(j, i));

            if(ntg_cell_has_bg(*it_cell))
            {
                it_old = ntg_cell_get_bg(*it_cell);

                // TODO(?)
                int new_r = ((int)it_old._rgb.r) + brightness_adj;
                int new_g = ((int)it_old._rgb.g) + brightness_adj;
                int new_b = ((int)it_old._rgb.b) + brightness_adj;

                new_r = _clamp_int(0, new_r, 255);
                new_g = _clamp_int(0, new_g, 255);
                new_b = _clamp_int(0, new_b, 255);

                it_new = nt_color_new(new_r, new_g, new_b);

                (*it_cell) = ntg_cell_set_bg(*it_cell, it_new);
            }
        }
    }

    return true;
}

struct ntg_object_fx ntg_object_fx_brighten_bg(int brightness_adj)
{
    int* data = (int*)malloc(sizeof(int));
    assert(data != NULL);
    assert((brightness_adj < 255) && (brightness_adj > (-255)));

    (*data) = brightness_adj;

    return (struct ntg_object_fx) {
        .apply_fn = __ntg_object_fx_brighten_bg_fn,
        .data = data
    };
}
