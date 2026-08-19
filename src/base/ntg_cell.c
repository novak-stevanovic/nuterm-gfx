#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* CELL VECGRID */
/* ------------------------------------------------------ */

int ntg_cell_vecgrid_init(ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_init(&vecgrid->__base))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_deinit(&vecgrid->__base))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_cell_vecgrid_set_size(
        ntg_cell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap)
{
    if(!vecgrid)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX) ||
    (size_cap.x > NTG_SIZE_MAX) || (size_cap.y > NTG_SIZE_MAX))
    {
        return NTG_ERR_INV_ARG;
    }

    struct ntg_xy old = vecgrid->__base._size;

    int _status = ntg_vecgrid_set_size(
            &vecgrid->__base,
            size,
            2.5,
            size_cap,
            sizeof(struct ntg_cell));

    switch(_status)
    {
        case 0: break;
        case NTG_ERR_ALLOC_FAIL:
            return NTG_ERR_ALLOC_FAIL;
        default:
            return NTG_ERR_UNEXPECTED;
    }

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

    return 0;
}

struct ntg_xy ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid)
        return ntg_xy(0, 0);

    return vecgrid->__base._size;
}

/* ------------------------------------------------------ */
/* VCELL VECGRID */
/* ------------------------------------------------------ */

int ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_init(&vecgrid->__base))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_deinit(&vecgrid->__base))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_vcell_vecgrid_set_size(
        ntg_vcell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap)
{
    if(!vecgrid)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX) ||
    (size_cap.x > NTG_SIZE_MAX) || (size_cap.y > NTG_SIZE_MAX))
    {
        return NTG_ERR_INV_ARG;
    }

    struct ntg_xy old = vecgrid->__base._size;

    int _status = ntg_vecgrid_set_size(
            &vecgrid->__base,
            size,
            2.5,
            size_cap,
            sizeof(struct ntg_vcell));
    if(_status != 0)
    {
        switch(_status)
        {
            case NTG_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;

            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    size_t i, j;

    for (i = old.y; i < size.y; i++)
    {
        for (j = 0; j < size.x; j++)
        {
            ntg_vcell_vecgrid_set(vecgrid, ntg_vcell_new_default(), ntg_xy(j, i));
        }
    }

    size_t common_y = (old.y < size.y) ? old.y : size.y;
    for (i = 0; i < common_y; i++)
    {
        for (j = old.x; j < size.x; j++)
        {
            ntg_vcell_vecgrid_set(vecgrid, ntg_vcell_new_default(), ntg_xy(j, i));
        }
    }

    return 0;
}

struct ntg_xy ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid)
        return ntg_xy(0, 0);

    return vecgrid->__base._size;
}
