#include "core/object/ntg_object_padding.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/scene/shared/ntg_drawable_kit.h"
#include "core/scene/shared/ntg_drawable_vec.h"

void __ntg_object_padding_init__(ntg_object_padding* padding, ntg_draw_fn draw_fn, void* data)
{
    assert(padding != NULL);
    assert(draw_fn != NULL);

    __ntg_object_init__((ntg_object*)padding,
            NTG_OBJECT_PADDING,
            __ntg_object_padding_measure_fn,
            __ntg_object_padding_constrain_fn,
            __ntg_object_padding_arrange_fn,
            draw_fn,
            NULL, NULL, NULL, data);

    padding->__north_size = 0;
    padding->__east_size = 0;
    padding->__south_size = 0;
    padding->__west_size = 0;

    ntg_object_set_grow((ntg_object*)padding, ntg_xy(0, 0));
}

void __ntg_object_padding_deinit__(ntg_object_padding* padding)
{
    assert(padding != NULL);

    __ntg_object_deinit__((ntg_object*)padding);

    padding->__north_size = 0;
    padding->__east_size = 0;
    padding->__south_size = 0;
    padding->__west_size = 0;
}

struct ntg_measure_out __ntg_object_padding_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena)
{
    const ntg_object* _padding = (ntg_object*)ntg_drawable_user(drawable);
    const ntg_object_padding* padding = (ntg_object_padding*)_padding;

    const ntg_drawable_vec* children = drawable->_get_children_fn_(drawable);
    const ntg_drawable* child = children->_data[0];

    struct ntg_measure_data child_data = ntg_measure_ctx_get(ctx, child);

    if(orientation == NTG_ORIENTATION_H)
    {
        size_t h_size = padding->__west_size + padding->__east_size;
        return (struct ntg_measure_out) {
            .min_size = child_data.min_size + h_size,
            .natural_size = child_data.natural_size + h_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX :
                child_data.max_size + h_size,
            .grow = 0
        };
    }
    else
    {
        size_t v_size = padding->__north_size + padding->__south_size;
        return (struct ntg_measure_out) {
            .min_size = child_data.min_size + v_size,
            .natural_size = child_data.natural_size + v_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX :
                child_data.max_size + v_size,
            .grow = 0
        };
    }
}

void __ntg_object_padding_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out,
        sarena* arena)
{
}

void __ntg_object_padding_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out,
        sarena* arena)
{
}

// void __ntg_object_padding_draw_fn(
//         const ntg_drawable* drawable,
//         struct ntg_xy size,
//         ntg_drawing* out_drawing,
//         sarena* arena)
// {
// }
