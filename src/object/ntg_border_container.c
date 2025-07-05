#include "object/ntg_border_container.h"
#include <assert.h>
#include <stdlib.h>

static void __set_default_values(ntg_border_container* container)
{
    container->_north = NULL;
    container->_east = NULL;
    container->_south = NULL;
    container->_west = NULL;
    container->_center = NULL;
}

void __ntg_border_container_init__(ntg_border_container* container)
{
    assert(container != NULL);

    __set_default_values(container);
}
void __ntg_border_container_deinit__(ntg_border_container* container)
{
    assert(container != NULL);

    __set_default_values(container);
}

ntg_border_container* ntg_border_container_new()
{
    ntg_border_container* new = (ntg_border_container*)malloc(
            sizeof(struct ntg_border_container));
    assert(new != NULL);

    __ntg_border_container_init__(new);

    return new;
}
void ntg_border_container_destroy(ntg_border_container* container)
{
    assert(container != NULL);

    __ntg_border_container_deinit__(container);

    free(container);
}

void ntg_border_container_set_north(ntg_border_container* container,
        ntg_object* north)
{
    assert(container != NULL);

    container->_north = north;
}

void ntg_border_container_set_east(ntg_border_container* container,
        ntg_object* east)
{
    assert(container != NULL);

    container->_east = east;
}

void ntg_border_container_set_south(ntg_border_container* container,
        ntg_object* south)
{
    assert(container != NULL);

    container->_south = south;
}

void ntg_border_container_set_west(ntg_border_container* container,
        ntg_object* west)
{
    assert(container != NULL);

    container->_west = west;
}

void ntg_border_container_set_center(ntg_border_container* container,
        ntg_object* center)
{
    assert(container != NULL);

    container->_center = center;
}
