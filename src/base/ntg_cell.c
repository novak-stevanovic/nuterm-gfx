#include "ntg.h"
#include "shared/ntg_shared_internal.h"

#define CELL_VECGRID_CAP_FACTOR 1.4
#define VCELL_VECGRID_CAP_FACTOR 1.4

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

    if(ntg_vecgrid_init(&vecgrid->priv.base, CELL_VECGRID_CAP_FACTOR))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_deinit(&vecgrid->priv.base))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_cell_vecgrid_set_size(ntg_cell_vecgrid* vecgrid, struct ntg_xy size)
{
    if(!vecgrid)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
    {
        return NTG_ERR_INV_ARG;
    }

    struct ntg_xy old = vecgrid->priv.base.size;

    int _status = ntg_vecgrid_set_size(
            &vecgrid->priv.base,
            size,
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

/* ------------------------------------------------------ */
/* VCELL VECGRID */
/* ------------------------------------------------------ */

int ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_init(&vecgrid->priv.base, VCELL_VECGRID_CAP_FACTOR))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(ntg_vecgrid_deinit(&vecgrid->priv.base))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_vcell_vecgrid_set_size(ntg_vcell_vecgrid* vecgrid, struct ntg_xy size)
{
    if(!vecgrid)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    struct ntg_xy old = vecgrid->priv.base.size;

    int _status = ntg_vecgrid_set_size(
            &vecgrid->priv.base,
            size,
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
