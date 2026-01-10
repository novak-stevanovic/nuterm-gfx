#ifndef _NTG_VECGRID_H_
#define _NTG_VECGRID_H_

#include "shared/ntg_xy.h"
#include "shared/ntg_typedef.h"

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

#endif // _NTG_VECGRID_H_
