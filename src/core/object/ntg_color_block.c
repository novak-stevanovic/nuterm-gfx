#include "ntg.h"
#include "shared/ntg_shared_internal.h"

#define DEFAULT_SIZE 1

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_color_block_init(
        ntg_color_block* color_block,
        struct nt_color color)
{
    int _status = ntg_color_block_init_inherit(
            color_block,
            &NTG_COLOR_BLOCK_VTABLE,
            &NTG_TYPE_COLOR_BLOCK,
            NULL);

    if(!_status)
        ntg_color_block_set_color(color_block, color);

    return _status;
}

void ntg_color_block_deinit(ntg_color_block* color_block)
{
    if(!color_block) return;

    color_block->_color = NT_COLOR_DEFAULT;
    color_block->hooks = (struct ntg_color_block_hooks) {0};

    ntg_object_deinit((ntg_object*)color_block);
}

void ntg_color_block_deinit_void(void* _color_block)
{
    ntg_color_block_deinit(_color_block);
}

/* ------------------------------------------------------ */
/* COLOR */
/* ------------------------------------------------------ */

void ntg_color_block_set_color(
        ntg_color_block* color_block,
        struct nt_color color)
{
    if(!color_block) return;

    struct nt_color old_color = color_block->_color;

    if(nt_color_are_eql(old_color, color))
        return;

    color_block->_color = color;

    ntg_object_mark_dirty((ntg_object*)color_block, NTG_OBJECT_DIRTY_DRAW);

    if(color_block->hooks.on_color_chng_fn)
        color_block->hooks.on_color_chng_fn(color_block, old_color, color);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

int ntg_color_block_init_inherit(
        ntg_color_block* color_block,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!color_block || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instance_of(type, &NTG_TYPE_COLOR_BLOCK))
        return NTG_ERR_INV_TYPE;

    int _status = ntg_object_init_inherit(
            (ntg_object*)color_block, vtable, type, layout_dt);
    if(_status != 0)
        return _status;

    color_block->hooks = (struct ntg_color_block_hooks) {0};
    color_block->_color = NT_COLOR_DEFAULT;
    return 0;
}

int ntg_color_block_measure_fn(
        const ntg_object* _color_block,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure)
{
    (void)_color_block;
    (void)layout_dt;
    (void)orient;
    (void)arena;
    (void)relayout;
    if(!out_measure)
        return NTG_ERR_INV_ARG;

    *out_measure = (struct ntg_object_measure) {
        .min_size = DEFAULT_SIZE,
        .nat_size = DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
    return 0;
}

int ntg_color_block_draw_fn(
        const ntg_object* _color_block,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_color_block* color_block = ntg_cb(_color_block);
    struct ntg_xy size = ntg_object_get_size_cont(_color_block);

    if(ntg_xy_size_is_zero(size)) return 0;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    ntg_vcell_new_full_bg(color_block->_color),
                    ntg_xy(j, i));
        }
    }

    return 0;
}

void ntg_color_block_deinit_fn(ntg_object* _color_block)
{
    ntg_color_block_deinit((ntg_color_block*)_color_block);
}

NTG_API const struct ntg_object_vtable NTG_COLOR_BLOCK_VTABLE = {
    .measure_fn = ntg_color_block_measure_fn,
    .draw_fn = ntg_color_block_draw_fn,
    .deinit_fn = ntg_color_block_deinit_fn
};
