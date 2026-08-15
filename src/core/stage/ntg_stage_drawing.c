#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_stage_drawing_init(ntg_stage_drawing* drawing, int* out_status)
{
    ntg_init_status(out_status);

    if(drawing == NULL)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_cell_vecgrid_init(&drawing->__data);
}

void ntg_stage_drawing_deinit(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_cell_vecgrid_deinit(&drawing->__data);
}

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_cell_vecgrid_get_size(&drawing->__data) :
        NTG_XY_UNSET;
}

void ntg_stage_drawing_set_size(
        ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status)
{
    ntg_init_status(out_status);

    if(drawing == NULL)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX) ||
    (size_cap.x > NTG_SIZE_MAX) || (size_cap.y > NTG_SIZE_MAX))
    {
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);
    }

    int _status = 0;
    ntg_cell_vecgrid_set_size(&drawing->__data, size, size_cap, &_status);
    switch(_status)
    {
        case 0: break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }
}
