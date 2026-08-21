#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
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
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_stage_drawing_init(ntg_stage_drawing* drawing)
{
    if(drawing == NULL)
        return NTG_ERR_INV_ARG;

    if(ntg_cell_vecgrid_init(&drawing->priv.data))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_stage_drawing_deinit(ntg_stage_drawing* drawing)
{
    if(drawing == NULL)
        return NTG_ERR_INV_ARG;

    if(ntg_cell_vecgrid_deinit(&drawing->priv.data))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

int ntg_stage_drawing_set_size(ntg_stage_drawing* drawing, struct ntg_xy size)
{
    if(drawing == NULL)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    int _status = ntg_cell_vecgrid_set_size(&drawing->priv.data, size);
    switch(_status)
    {
        case 0: break;
        case NTG_ERR_ALLOC_FAIL:
            return NTG_ERR_ALLOC_FAIL;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    return 0;
}
