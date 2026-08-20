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

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_cell_vecgrid_get_size(&drawing->priv.data) :
        NTG_XY_UNSET;
}

int ntg_stage_drawing_set_size(
        ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap)
{
    if(drawing == NULL)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX) ||
    (size_cap.x > NTG_SIZE_MAX) || (size_cap.y > NTG_SIZE_MAX))
    {
        return NTG_ERR_INV_ARG;
    }

    int _status = ntg_cell_vecgrid_set_size(&drawing->priv.data, size, size_cap);
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
