#ifndef NTG_VECGRID_H
#define NTG_VECGRID_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

struct ntg_vecgrid
{
    void* _data;
    struct ntg_xy _size;
    size_t _capacity;
};

void ntg_vecgrid_init(ntg_vecgrid* vecgrid);
void ntg_vecgrid_deinit(ntg_vecgrid* vecgrid);

void ntg_vecgrid_set_size(ntg_vecgrid* vecgrid, struct ntg_xy size,
        double modifier, struct ntg_xy size_cap, size_t data_size);

#endif // NTG_VECGRID_H
