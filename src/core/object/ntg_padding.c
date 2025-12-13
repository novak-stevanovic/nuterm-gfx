#include "core/object/ntg_padding.h"
#include "base/ntg_sap.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "shared/_ntg_shared.h"

void _ntg_padding_init_(
        ntg_padding* padding,
        struct ntg_padding_width init_width,
        ntg_object_draw_fn draw_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data)
{
    assert(padding != NULL);
    assert(draw_fn != NULL);

    _ntg_object_init_((ntg_object*)padding,
            NTG_OBJECT_PADDING,
            _ntg_padding_measure_fn,
            _ntg_padding_constrain_fn,
            _ntg_padding_arrange_fn,
            draw_fn,
            NULL, NULL, NULL,
            deinit_fn,
            data);

    padding->__width = init_width;

    // ntg_object_set_grow((ntg_object*)padding, ntg_xy(0, 0));
}

void _ntg_padding_deinit_(ntg_padding* padding)
{
    assert(padding != NULL);
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

void _ntg_padding_deinit_fn(ntg_object* object)
{
    assert(object != NULL);

    ntg_padding* padding = (ntg_padding*)object;

    _ntg_object_deinit_(object);

    padding->__width = (struct ntg_padding_width) {0};
}

struct ntg_object_measure _ntg_padding_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena)
{
    const ntg_padding* padding = (ntg_padding*)object;
    const ntg_object* child = ntg_object_get_children(object)->_data[0];
    struct ntg_object_measure child_data = ntg_object_measure_map_get(
            ctx.measures, child);

    if(orientation == NTG_ORIENTATION_H)
    {
        size_t h_size = padding->__width.west + padding->__width.east;
        return (struct ntg_object_measure) {
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
        return (struct ntg_object_measure) {
            .min_size = child_data.min_size + v_size,
            .natural_size = child_data.natural_size + v_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX :
                child_data.max_size + v_size,
            .grow = 0
        };
    }
}

void _ntg_padding_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena)
{
    const ntg_padding* padding = (ntg_padding*)object;
    const ntg_object* child = ntg_object_get_children(object)->_data[0];
    struct ntg_object_measure child_data = ntg_object_measure_map_get(
            ctx.measures, child);

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

    ntg_object_size_map_set(out->sizes, child, child_size);
}

void _ntg_padding_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena)
{
    // const ntg_padding* padding = (ntg_padding*)object;
    const ntg_object* child = ntg_object_get_children(object)->_data[0];
    struct ntg_xy child_size = ntg_object_xy_map_get(ctx.sizes, child);

    struct ntg_xy offset = ntg_xy_sub(size, child_size);

    ntg_object_xy_map_set(out->pos, child, offset);
}
