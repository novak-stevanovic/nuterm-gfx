#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* CELL TYPES */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* CELL VECGRID */
/* -------------------------------------------------------------------------- */

void ntg_cell_vecgrid_init(ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid) return;

    ntg_vecgrid_init(&vecgrid->__base);
}

void ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid) return;

    ntg_vecgrid_deinit(&vecgrid->__base);
}

void ntg_cell_vecgrid_set_size(
        ntg_cell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!vecgrid)
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

    struct ntg_xy old = vecgrid->__base._size;

    ntg_vecgrid_set_size(
            &vecgrid->__base,
            size,
            2.5,
            size_cap,
            sizeof(struct ntg_cell),
            out_status);

    size_t i, j;

    for (i = old.y; i < size.y; i++)
    {
        for (j = 0; j < size.x; j++)
        {
            ntg_cell_vecgrid_set(vecgrid, ntg_cell_default(), ntg_xy(j, i));
        }
    }

    size_t common_y = (old.y < size.y) ? old.y : size.y;
    for (i = 0; i < common_y; i++)
    {
        for (j = old.x; j < size.x; j++)
        {
            ntg_cell_vecgrid_set(vecgrid, ntg_cell_default(), ntg_xy(j, i));
        }
    }
}

struct ntg_xy ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid)
        return ntg_xy(0, 0);

    return vecgrid->__base._size;
}

/* ========================================================================== */
/* VCELL TYPES */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* VCELL VECGRID */
/* -------------------------------------------------------------------------- */

void ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid) return;

    ntg_vecgrid_init(&vecgrid->__base);
}

void ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid) return;

    ntg_vecgrid_deinit(&vecgrid->__base);
}

void ntg_vcell_vecgrid_set_size(
        ntg_vcell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!vecgrid)
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

    struct ntg_xy old = vecgrid->__base._size;

    int _status;

    ntg_vecgrid_set_size(&vecgrid->__base, size, 2.5,
            size_cap, sizeof(struct ntg_vcell), &_status);
    if(_status != NTG_SUCCESS)
    {
        switch(_status)
        {
            case NTG_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

    size_t i, j;

    for (i = old.y; i < size.y; i++)
    {
        for (j = 0; j < size.x; j++)
        {
            ntg_vcell_vecgrid_set(vecgrid, ntg_vcell_default(), ntg_xy(j, i));
        }
    }

    size_t common_y = (old.y < size.y) ? old.y : size.y;
    for (i = 0; i < common_y; i++)
    {
        for (j = old.x; j < size.x; j++)
        {
            ntg_vcell_vecgrid_set(vecgrid, ntg_vcell_default(), ntg_xy(j, i));
        }
    }
}

struct ntg_xy ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid)
        return ntg_xy(0, 0);

    return vecgrid->__base._size;
}
