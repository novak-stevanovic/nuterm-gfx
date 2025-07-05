#ifndef _NTG_BORDER_CONTAINER_H_
#define _NTG_BORDER_CONTAINER_H_

#include "object/ntg_container.h"

#define NTG_BORDER_CONTAINER(cnt_ptr) ((ntg_border_container*)(cnt_ptr))

typedef struct ntg_border_container
{
    ntg_container __base;

    ntg_object *_north, *_east, *_south, *_west, *_center;
} ntg_border_container;

void __ntg_border_container_init__(ntg_border_container* container);
void __ntg_border_container_deinit__(ntg_border_container* container);

ntg_border_container* ntg_border_container_new();
void ntg_border_container_destroy(ntg_border_container* container);

void ntg_border_container_set_north(ntg_border_container* container,
        ntg_object* north);
void ntg_border_container_set_east(ntg_border_container* container,
        ntg_object* east);
void ntg_border_container_set_south(ntg_border_container* container,
        ntg_object* south);
void ntg_border_container_set_west(ntg_border_container* container,
        ntg_object* west);
void ntg_border_container_set_center(ntg_border_container* container,
        ntg_object* center);

#endif // _NTG_BORDER_CONTAINER_H_
