#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
#include "shared/_ntg_shared.h"
#include "nt.h"
#include "base/ntg_cell.h"

void ntg_cell_vecgrid_init(ntg_cell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    ntg_vecgrid_init(&vecgrid->__base);
}

void ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    ntg_vecgrid_deinit(&vecgrid->__base);
}

void ntg_cell_vecgrid_set_size(
        ntg_cell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap)
{
    assert(vecgrid != NULL);

    struct ntg_xy old_size = vecgrid->__base._size;

    ntg_vecgrid_set_size(&vecgrid->__base, size, 2.5, size_cap, sizeof(struct ntg_cell));

    size_t i, j;
    struct ntg_cell* it_cell;
    for(i = old_size.y; i < size.y; i++)
    {
        for(j = old_size.x; j < size.x; j++)
        {
            it_cell = ntg_cell_vecgrid_at_(vecgrid, ntg_xy(j, i));
            (*it_cell) = ntg_cell_default();
        }
    }
}

struct ntg_xy ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    return vecgrid->__base._size;
}

void ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    ntg_vecgrid_init(&vecgrid->__base);
}

void ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    ntg_vecgrid_deinit(&vecgrid->__base);
}

void ntg_vcell_vecgrid_set_size(
        ntg_vcell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap)
{
    assert(vecgrid != NULL);

    struct ntg_xy old_size = vecgrid->__base._size;

    ntg_vecgrid_set_size(&vecgrid->__base, size, 2.5, size_cap, sizeof(struct ntg_vcell));

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = old_size.y; i < size.y; i++)
    {
        for(j = old_size.x; j < size.x; j++)
        {
            it_cell = ntg_vcell_vecgrid_at_(vecgrid, ntg_xy(j, i));
            (*it_cell) = ntg_vcell_default();
        }
    }
}

struct ntg_xy ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    return vecgrid->__base._size;
}

