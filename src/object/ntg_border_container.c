#include "object/ntg_border_container.h"
#include "base/ntg_sap.h"
#include "object/ntg_color_block.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

static void __calculate_nsize_fn(ntg_object* _container)
{
    ntg_border_container* container = NTG_BORDER_CONTAINER(_container);

    struct ntg_xy nsize_north = (container->_north != NULL) ?
        ntg_object_get_natural_size(container->_north) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_east = (container->_east != NULL) ?
        ntg_object_get_natural_size(container->_east) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_south = (container->_south != NULL) ?
        ntg_object_get_natural_size(container->_south) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_west = (container->_west != NULL) ?
        ntg_object_get_natural_size(container->_west) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_center = (container->_center != NULL) ?
        ntg_object_get_natural_size(container->_center) :
        ntg_xy(0, 0);

    size_t width = _max3_size(
                nsize_west.x + nsize_center.x + nsize_east.x,
                nsize_north.x,
                nsize_south.x);

    size_t height = _max3_size(
                nsize_north.y + nsize_center.y + nsize_south.y,
                nsize_west.y,
                nsize_east.y);

    _ntg_object_set_natural_content_size(_container, ntg_xy_size(ntg_xy(width, height)));
}

static void __constrain_fn(ntg_object* _container)
{
    // TODO
    ntg_border_container* container = NTG_BORDER_CONTAINER(_container);

    struct ntg_constr constr = ntg_object_get_content_constr(_container);
    struct ntg_xy nsize = ntg_object_get_natural_content_size(_container);

    struct ntg_xy nsize_north = (container->_north != NULL) ?
        ntg_object_get_natural_size(container->_north) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_east = (container->_east != NULL) ?
        ntg_object_get_natural_size(container->_east) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_south = (container->_south != NULL) ?
        ntg_object_get_natural_size(container->_south) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_west = (container->_west != NULL) ?
        ntg_object_get_natural_size(container->_west) :
        ntg_xy(0, 0);
    struct ntg_xy nsize_center = (container->_center != NULL) ?
        ntg_object_get_natural_size(container->_center) :
        ntg_xy(0, 0);

    struct ntg_xy size_north, size_east, size_south, size_west, size_center; 

    if(nsize.x < constr.min_size.x)
    {
        size_north.x = constr.min_size.x;
        size_south.x = constr.min_size.x;
        size_east.x = nsize_east.x;
        size_west.x = nsize_west.x;
        size_center.x = constr.min_size.x - size_west.x - size_east.x;
    }
    else if((nsize.x >= constr.min_size.x) && (nsize.x <= constr.max_size.x))
    {
        size_t ns_size = _max3_size(
                nsize_west.x + nsize_east.x + nsize_center.x,
                nsize_north.x,
                nsize_south.x);

        size_north.x = ns_size;
        size_south.x = ns_size;

        size_east.x = nsize_east.x;
        size_west.x = nsize_west.x;
        size_center.x = ns_size - (size_east.x + size_west.x);
    }
    else
    {
        size_t nsizes[3] = { 
            nsize_west.x,
            (container->_north || container->_south) ? nsize_center.x : 0,
            nsize_east.x
        };
        size_t _sizes[3] = {0};
        ntg_sap_nsize_round_robin(nsizes, _sizes, constr.max_size.x, 3);

        size_north.x = constr.max_size.x;
        size_south.x = constr.max_size.x;

        size_west.x = _sizes[0];
        size_east.x = _sizes[2];

        size_t center_extra = constr.max_size.x - (_sizes[1] + size_west.x + size_east.x);
        size_center.x = _sizes[1] + center_extra;
    }

    if(nsize.y < constr.min_size.y)
    {
        size_north.y = nsize_north.y;
        size_south.y = nsize_south.y;

        size_t wec_size = constr.min_size.y - size_north.y - size_south.y;
        size_east.y = wec_size;
        size_west.y = wec_size;
        size_center.y = wec_size;
    }
    else if((nsize.y >= constr.min_size.y) && (nsize.y <= constr.max_size.y))
    {
        size_t wec_size = _max3_size(
                nsize_center.y,
                nsize_east.y,
                nsize_west.y);

        size_north.y = nsize_north.y;
        size_south.y = nsize_south.y;
        size_east.y = wec_size;
        size_west.y = wec_size;
        size_center.y = wec_size;
    }
    else
    {
        size_t nsizes[3] = { 
            nsize_north.y,
            (container->_west || container->_east) ? nsize_center.y : 0,
            nsize_south.y
        };
        size_t _sizes[3] = {0};
        ntg_sap_nsize_round_robin(nsizes, _sizes, constr.max_size.y, 3);

        size_north.y = _sizes[0];
        size_south.y = _sizes[2];

        size_t center_extra = constr.max_size.y - (_sizes[1] + size_north.y + size_south.y);
        size_center.y = _sizes[1] + center_extra;

        size_west.y = constr.max_size.y - (size_north.y + size_south.y);
        size_east.y = constr.max_size.y - (size_north.y + size_south.y);
    }

    ntg_xy_size_(&size_north);
    ntg_xy_size_(&size_east);
    ntg_xy_size_(&size_south);
    ntg_xy_size_(&size_west);
    ntg_xy_size_(&size_center);

    if(container->_north != NULL)
    {
        _ntg_object_set_constr(container->_north,
                ntg_constr(size_north, size_north));
    }
    if(container->_east != NULL)
    {
        _ntg_object_set_constr(container->_east,
                ntg_constr(size_east, size_east));
    }
    if(container->_south != NULL)
    {
        _ntg_object_set_constr(container->_south,
                ntg_constr(size_south, size_south));
    }
    if(container->_west != NULL)
    {
        _ntg_object_set_constr(container->_west,
                ntg_constr(size_west, size_west));
    }
    if(container->_center != NULL)
    {
        _ntg_object_set_constr( container->_center,
                ntg_constr(size_center, size_center));
    }
}

static void __measure_fn(ntg_object* _container)
{
    ntg_border_container* container = NTG_BORDER_CONTAINER(_container);

    struct ntg_xy size_north = (container->_north != NULL) ?
        ntg_object_get_size(container->_north) :
        ntg_xy(0, 0);
    struct ntg_xy size_east = (container->_east != NULL) ?
        ntg_object_get_size(container->_east) :
        ntg_xy(0, 0);
    struct ntg_xy size_south = (container->_south != NULL) ?
        ntg_object_get_size(container->_south) :
        ntg_xy(0, 0);
    struct ntg_xy size_west = (container->_west != NULL) ?
        ntg_object_get_size(container->_west) :
        ntg_xy(0, 0);
    struct ntg_xy size_center = (container->_center != NULL) ?
        ntg_object_get_size(container->_center) :
        ntg_xy(0, 0);

    size_t width = _max3_size(
                size_west.x + size_center.x + size_east.x,
                size_north.x,
                size_south.x);

    size_t height = _max3_size(
                size_north.y + size_center.y + size_south.y,
                size_west.y,
                size_east.y);

    _ntg_object_set_content_size(_container, ntg_xy_size(ntg_xy(width, height)));
}

static void __arrange_fn(ntg_container* _container)
{
    ntg_object* __container = NTG_OBJECT(_container);
    ntg_border_container* container = NTG_BORDER_CONTAINER(_container);

    struct ntg_xy size = ntg_object_get_content_size(__container);

    struct ntg_xy size_north = (container->_north != NULL) ?
        ntg_object_get_size(container->_north) :
        ntg_xy(0, 0);
    struct ntg_xy size_east = (container->_east != NULL) ?
        ntg_object_get_size(container->_east) :
        ntg_xy(0, 0);
    struct ntg_xy size_south = (container->_south != NULL) ?
        ntg_object_get_size(container->_south) :
        ntg_xy(0, 0);
    struct ntg_xy size_west = (container->_west != NULL) ?
        ntg_object_get_size(container->_west) :
        ntg_xy(0, 0);

    /* Even if center doesn't exist, the background will fill the space */
    struct ntg_xy size_center = ntg_xy(
            size.x - size_east.x - size_west.x,
            size.y - size_north.y - size_south.y);

    if(container->_north)
    {
        struct ntg_xy pos_north = ntg_xy(0, 0);
        _ntg_object_set_pos_inside_content(container->_north, pos_north);
    }
    if(container->_east)
    {
        struct ntg_xy pos_east = ntg_xy(0, size_north.y);
        _ntg_object_set_pos_inside_content(container->_east, pos_east);
    }
    if(container->_center)
    {
        struct ntg_xy pos_center = ntg_xy(size_east.x, size_north.y);
        _ntg_object_set_pos_inside_content(container->_center, pos_center);
    }
    if(container->_west)
    {
        struct ntg_xy pos_west = ntg_xy(size_east.x + size_center.x, size_north.y);
        _ntg_object_set_pos_inside_content(container->_west, pos_west);
    }
    if(container->_south)
    {
        struct ntg_xy pos_south = ntg_xy(0, size_north.y + size_center.y);
        _ntg_object_set_pos_inside_content(container->_south, pos_south);
    }
}

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

    __ntg_container_init__(NTG_CONTAINER(container),
            __calculate_nsize_fn, __constrain_fn, __measure_fn, __arrange_fn);

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

    ntg_object* _container = NTG_OBJECT(container);

    if(container->_north)
        _ntg_object_child_remove(_container, container->_north);

    container->_north = north;

    if(north)
        _ntg_object_child_add(_container, north);
}

void ntg_border_container_set_east(ntg_border_container* container,
        ntg_object* east)
{
    assert(container != NULL);

    ntg_object* _container = NTG_OBJECT(container);

    if(container->_east)
        _ntg_object_child_remove(_container, container->_east);

    container->_east = east;

    if(east)
        _ntg_object_child_add(_container, east);
}

void ntg_border_container_set_south(ntg_border_container* container,
        ntg_object* south)
{
    assert(container != NULL);

    ntg_object* _container = NTG_OBJECT(container);

    if(container->_south)
        _ntg_object_child_remove(_container, container->_south);

    container->_south = south;

    if(south)
        _ntg_object_child_add(_container, south);
}

void ntg_border_container_set_west(ntg_border_container* container,
        ntg_object* west)
{
    assert(container != NULL);

    ntg_object* _container = NTG_OBJECT(container);

    if(container->_west)
        _ntg_object_child_remove(_container, container->_west);

    container->_west = west;

    if(west)
        _ntg_object_child_add(_container, west);
}

void ntg_border_container_set_center(ntg_border_container* container,
        ntg_object* center)
{
    assert(container != NULL);

    ntg_object* _container = NTG_OBJECT(container);

    if(container->_center != NULL)
        _ntg_object_child_remove(_container, container->_center);

    container->_center = center;

    if(center)
        _ntg_object_child_add(_container, center);
}
