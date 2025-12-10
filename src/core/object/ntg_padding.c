#include "core/object/ntg_padding.h"
#include "base/ntg_sap.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/scene/shared/ntg_drawable_kit.h"
#include "core/scene/shared/ntg_drawable_vec.h"
#include "shared/_ntg_shared.h"

void __ntg_padding_init__(
        ntg_padding* padding,
        ntg_draw_fn draw_fn,
        struct ntg_padding_width init_width,
        void* data)
{
    assert(padding != NULL);
    assert(draw_fn != NULL);

    __ntg_object_init__((ntg_object*)padding,
            NTG_OBJECT_PADDING,
            __ntg_padding_measure_fn,
            __ntg_padding_constrain_fn,
            __ntg_padding_arrange_fn,
            draw_fn,
            NULL, NULL, NULL, data);

    padding->__width = init_width;

    // ntg_object_set_grow((ntg_object*)padding, ntg_xy(0, 0));
}

void __ntg_padding_deinit__(ntg_padding* padding)
{
    assert(padding != NULL);

    __ntg_object_deinit__((ntg_object*)padding);

    padding->__width = (struct ntg_padding_width) {0};
}

void ntg_padding_set_width(ntg_padding* padding, struct ntg_padding_width width)
{
    assert(padding != NULL);

    padding->__width = width;
}

struct ntg_padding_width ntg_padding_get_width(const ntg_padding* padding)
{
    assert(padding != NULL);

    return padding->__width;
}

struct ntg_measure_out __ntg_padding_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena)
{
    const ntg_object* _padding = (ntg_object*)ntg_drawable_user(drawable);
    const ntg_padding* padding = (ntg_padding*)_padding;

    const ntg_drawable_vec* children = drawable->get_children_fn_(drawable);
    const ntg_drawable* child = children->_data[0];

    struct ntg_measure_data child_data = ntg_measure_ctx_get(ctx, child);

    if(orientation == NTG_ORIENTATION_H)
    {
        size_t h_size = padding->__width.west + padding->__width.east;
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
        size_t v_size = padding->__width.north + padding->__width.south;
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

void __ntg_padding_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out,
        sarena* arena)
{
    const ntg_padding* padding = (ntg_padding*)ntg_drawable_user(drawable);

    const ntg_drawable* child = (drawable->get_children_fn_(drawable))->_data[0];

    struct ntg_measure_data child_data = ntg_measure_ctx_get(measure_ctx, child);
    struct ntg_constrain_result child_result;

    size_t extra_space = _ssub_size(child_data.natural_size, size);

    size_t _sizes[2] = {0};
    size_t caps[2];
    if(orientation == NTG_ORIENTATION_H)
    {
        caps[0] = padding->__width.west;
        caps[1] = padding->__width.east;
    }
    else
    {
        caps[0] = padding->__width.north;
        caps[1] = padding->__width.south;
    }

    ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_space, 2);

    size_t child_size = size - _sizes[0] - _sizes[1];
    child_result.size = child_size;

    ntg_constrain_out_set(out, child, child_result);
}

void __ntg_padding_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out,
        sarena* arena)
{
    const ntg_drawable* child = (drawable->get_children_fn_(drawable))->_data[0];

    struct ntg_arrange_data child_data = ntg_arrange_ctx_get(ctx, child);

    struct ntg_xy offset = ntg_xy_sub(size, child_data.size);

    struct ntg_arrange_result result = {
        .pos = offset
    };

    ntg_arrange_out_set(out, child, result);
}
