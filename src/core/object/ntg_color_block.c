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

void ntg_color_block_init(
        ntg_color_block* color_block,
        struct nt_color color,
        int* out_status)
{
    int _status;

    ntg_color_block_init_inherit(
            color_block,
            &NTG_COLOR_BLOCK_VTABLE,
            &NTG_TYPE_COLOR_BLOCK,
            NULL,
            &_status);

    if(!_status)
        ntg_color_block_set_color(color_block, color);

    ntg_vreturn(out_status, _status);
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

void ntg_color_block_init_inherit(
        ntg_color_block* color_block,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!color_block || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_COLOR_BLOCK))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    int _status;

    ntg_object_init_inherit(
            (ntg_object*)color_block, vtable, type, layout_dt, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    color_block->hooks = (struct ntg_color_block_hooks) {0};
    color_block->_color = NT_COLOR_DEFAULT;
}

struct ntg_object_measure ntg_color_block_measure_fn(
        const ntg_object* _color_block,
        struct ntg_object_layout_dt* layout_dt,
        ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        int* out_status)
{
    (void)layout_dt;
    (void)relayout;
    ntg_init_status(out_status);

    return (struct ntg_object_measure) {
        .min_size = DEFAULT_SIZE,
        .nat_size = DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
}

void ntg_color_block_draw_fn(
        const ntg_object* _color_block,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout,
        int* out_status)
{
    (void)layout_dt;
    (void)relayout;
    ntg_init_status(out_status);

    const ntg_color_block* color_block = ntg_cb(_color_block);
    struct ntg_xy size = ntg_object_get_size_cont(_color_block);

    if(ntg_xy_size_is_zero(size)) return;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    ntg_vcell_new_bg(color_block->_color),
                    ntg_xy(j, i));
        }
    }
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
