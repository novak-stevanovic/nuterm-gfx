#include "object/ntg_object.h"
#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"
#include "core/ntg_scene.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/ntg_log.h"
#include <assert.h>
#include <stdlib.h>

static void __calculate_border_size(ntg_object* object);
static void __construct_full_drawing(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* def/ntg_object_def.h */
/* -------------------------------------------------------------------------- */

static void _ntg_object_init_default(ntg_object* object)
{
    object->_parent = NULL;
    object->_children = NULL;

    object->_border_style = ntg_border_style_default();
    object->_border_pref_size = (struct ntg_border_size) {0};
    object->_border_size = (struct ntg_border_size) {0};
 
    object->__scroll = false;
    object->_virtual_drawing = NULL;
    object->__curr_scroll = ntg_xy(0, 0);
    object->__buffered_scroll = ntg_dxy(0, 0);

    object->_full_drawing = NULL;

    object->__pref_size = ntg_xy(NTG_PREF_SIZE_UNSET, NTG_PREF_SIZE_UNSET);
    object->__nsize = NTG_XY_UNSET;
    object->__constr = NTG_CONSTR_UNSET;
    object->__size = NTG_XY_UNSET;
    object->__pos = NTG_XY_UNSET;

    object->__arrange_fn = NULL;
    object->__measure_fn = NULL;
    object->__constrain_fn = NULL;
    object->__calculate_nsize_fn = NULL;
    object->__process_key_fn = NULL;

    object->_scene = NULL;
}

void __ntg_object_init__(ntg_object* object,
        ntg_calculate_nsize_fn calculate_nsize_fn,
        ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn)
{
    assert(object != NULL); 

    _ntg_object_init_default(object);

    object->__constrain_fn = constrain_fn;
    object->__measure_fn = measure_fn;
    object->__arrange_fn = arrange_fn;
    object->__calculate_nsize_fn = calculate_nsize_fn;

    object->_virtual_drawing = ntg_object_drawing_new();
    assert(object->_virtual_drawing != NULL);

    object->_full_drawing = ntg_object_drawing_new();
    assert(object->_full_drawing != NULL);

    object->_children = ntg_object_vec_new();
}

void __ntg_object_deinit__(ntg_object* object)
{
    assert(object != NULL); 

    ntg_object_drawing_destroy(object->_virtual_drawing);
    ntg_object_drawing_destroy(object->_full_drawing);

    if(object->_children != NULL)
        ntg_object_vec_destroy(object->_children);

    _ntg_object_init_default(object);
}

void _ntg_object_set_content_nsize(ntg_object* object, struct ntg_xy size)
{
    if(object == NULL) return;

    if((size.x == 0) || (size.y == 0))
    {
        object->__nsize = ntg_xy(0, 0);
        return;
    }

    struct ntg_xy pref_border_size = ntg_xy(
            object->_border_pref_size.west + object->_border_pref_size.east,
            object->_border_pref_size.north + object->_border_pref_size.south);

    size = ntg_xy_add(size, pref_border_size);

    if(object->__pref_size.x != NTG_PREF_SIZE_UNSET)
        size.x = object->__pref_size.x;

    if(object->__pref_size.y != NTG_PREF_SIZE_UNSET)
        size.y = object->__pref_size.y;

    object->__nsize = size;
}

static void _adjust_constr(size_t pref_size, size_t* min_size, size_t* max_size)
{
    if(pref_size == NTG_PREF_SIZE_UNSET) return;

    if((pref_size >= (*min_size)) && (pref_size <= (*max_size)))
    {
        (*min_size) = pref_size;
        (*max_size) = pref_size;
    }
    else if(pref_size >= (*max_size))
        (*min_size) = (*max_size);
    else
        (*max_size) = (*min_size);
}

void _ntg_object_set_constr(ntg_object* object, struct ntg_constr constr)
{
    if(object == NULL) return;

    _adjust_constr(object->__pref_size.x, &constr.min_size.x, &constr.max_size.x);
    _adjust_constr(object->__pref_size.y, &constr.min_size.y, &constr.max_size.y);

    object->__constr = constr;

    __calculate_border_size(object);
}

static void _fit_size_to_constr(size_t* size, size_t min_size, size_t max_size)
{
    if((*size) > max_size)
        (*size) = max_size;

    else if((*size) < min_size)
        (*size) = min_size;
}

void _ntg_object_set_content_size(ntg_object* object, struct ntg_xy size)
{
    if(object == NULL) return;

    _fit_size_to_constr(&(size.x), object->__constr.min_size.x,
            object->__constr.max_size.x);
    _fit_size_to_constr(&(size.y), object->__constr.min_size.y,
            object->__constr.max_size.y);

    ntg_xy_size_(&size);

    object->__size = size;

    struct ntg_xy content_nsize = ntg_object_get_content_nsize(object);
    struct ntg_xy content_size = ntg_object_get_content_size(object);

    if(object->__scroll)
    {
        struct ntg_xy virtual_size = ntg_xy(
                _max2_size(content_nsize.x, content_size.x),
                _max2_size(content_nsize.y, content_size.y));

        ntg_object_drawing_set_size(object->_virtual_drawing, virtual_size);
        ntg_object_drawing_set_size(object->_full_drawing, size);

        struct ntg_dxy _curr_scroll = ntg_dxy_from_xy(object->__curr_scroll);

        _curr_scroll = ntg_dxy_add(_curr_scroll, object->__buffered_scroll);

        _curr_scroll = ntg_dxy_clamp(ntg_dxy(0, 0),
                _curr_scroll,
                ntg_dxy_from_xy(ntg_xy_sub(virtual_size, content_size)));

        object->__curr_scroll = ntg_xy_from_dxy(_curr_scroll);
        object->__buffered_scroll = ntg_dxy(0, 0);
    }
    else
    {
        ntg_object_drawing_set_size(object->_virtual_drawing, content_size);
        ntg_object_drawing_set_size(object->_full_drawing, size);

        object->__buffered_scroll = ntg_dxy(0, 0);
        object->__curr_scroll = ntg_xy(0, 0);
    }
}

void _ntg_object_set_pos_inside_content(ntg_object* object, struct ntg_xy pos)
{
    if(object == NULL) return;

    ntg_object* parent = object->_parent;

    struct ntg_xy parent_pos = (parent != NULL) ?
        parent->__pos :
        NTG_XY_UNSET;

    struct ntg_xy border_offset = (parent != NULL) ?
        ntg_xy(parent->_border_size.west, parent->_border_size.north) :
        ntg_xy(0, 0);

    struct ntg_xy abs_pos = ntg_xy_add(border_offset, ntg_xy_add(pos, parent_pos));

    object->__pos = abs_pos;
}

void _ntg_object_scroll_enable(ntg_object* object)
{
    assert(object != NULL);

    if(!(object->__scroll))
    {
        object->__scroll = true;
        object->__curr_scroll = ntg_xy(0, 0);
        object->__buffered_scroll = ntg_dxy(0, 0);
    }
}

void _ntg_object_scroll_disable(ntg_object* object)
{
    assert(object != NULL);

    if(object->__scroll)
    {
        object->__scroll = false;
        object->__curr_scroll = ntg_xy(0, 0);
        object->__buffered_scroll = ntg_dxy(0, 0);
    }
}

void _ntg_object_scroll(ntg_object* object, struct ntg_dxy offset_diff)
{
    assert(object != NULL);

    if(object->__scroll)
    {
        object->__buffered_scroll = ntg_dxy_add(object->__buffered_scroll,
                offset_diff);
    }
}

struct ntg_xy _ntg_object_get_scroll(const ntg_object* object)
{
    return object->__curr_scroll;
}

static void __adjust_scene_fn(ntg_object* object, void* scene)
{
    object->_scene = (ntg_scene*)scene;
}

void _ntg_object_child_add(ntg_object* parent, ntg_object* child)
{
    assert((parent != NULL) && (child != NULL));

    ntg_object* curr_parent = child->_parent;

    if(curr_parent != NULL)
    {
        _ntg_object_child_remove(curr_parent, child);
    }

    ntg_object_vec_append(parent->_children, child);

    child->_parent = parent;

    _ntg_object_perform_tree(child, __adjust_scene_fn, parent->_scene);
}

void _ntg_object_child_remove(ntg_object* parent, ntg_object* child)
{
    assert((parent != NULL) && (child != NULL));

    ntg_object_vec_remove(parent->_children, child);

    child->_parent = NULL;

    _ntg_object_perform_tree(child, __adjust_scene_fn, NULL);
}

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn)
{
    assert(object != NULL);

    object->__process_key_fn = process_key_fn;
}

void _ntg_object_perform_tree(ntg_object* root,
        void (*perform_fn)(ntg_object* curr_obj, void* data),
        void* data)
{
    if(root == NULL) return;

    perform_fn(root, data);

    size_t i;
    for(i = 0; i < root->_children->_count; i++)
    {
        _ntg_object_perform_tree(root->_children->_data[i], perform_fn, data);
    }
}

/* -------------------------------------------------------------------------- */
/* ntg_object.h */
/* -------------------------------------------------------------------------- */

void ntg_object_layout_root(ntg_object* root, struct ntg_xy root_size)
{
    assert(root != NULL);
    // root->_nsize = root_size;
    _ntg_object_set_constr(root, ntg_constr(root_size, root_size));
    root->__pos = ntg_xy(0, 0);
}

struct ntg_xy ntg_object_get_pref_size(const ntg_object* object)
{
    assert(object != NULL);

    return object->__pref_size;
}

struct ntg_xy ntg_object_get_pref_content_size(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy pref_content_size;

    struct ntg_xy pref_border_size = ntg_border_size_sum(object->_border_pref_size);

    pref_content_size.x = (object->__pref_size.x != NTG_PREF_SIZE_UNSET) ?
        object->__pref_size.x - pref_border_size.x :
        NTG_PREF_SIZE_UNSET;

    pref_content_size.y = (object->__pref_size.y != NTG_PREF_SIZE_UNSET) ?
        object->__pref_size.y - pref_border_size.y :
        NTG_PREF_SIZE_UNSET;

    return pref_content_size;
}

void ntg_object_set_pref_size(ntg_object* object, struct ntg_xy pref_size)
{
    if(object == NULL) return;

    object->__pref_size = pref_size;
}

void ntg_object_set_pref_content_size(ntg_object* object, struct ntg_xy pref_size)
{
    if(object == NULL) return;

    struct ntg_xy border_size = ntg_border_size_sum(object->_border_pref_size);

    if(pref_size.x != NTG_PREF_SIZE_UNSET)
    {
        object->__pref_size.x = pref_size.x + border_size.x;
    }
    if(pref_size.y != NTG_PREF_SIZE_UNSET)
    {
        object->__pref_size.y = pref_size.y + border_size.y;
    }
}

void ntg_object_set_border_style(ntg_object* object,
        struct ntg_border_style border_style)
{
    assert(object != NULL);

    object->_border_style = border_style;
}

void ntg_object_set_border_size(ntg_object* object,
        struct ntg_border_size border_size)
{
    assert(object != NULL);

    object->_border_pref_size = border_size;
}

void ntg_object_calculate_nsize(ntg_object* object)
{
    if(object == NULL) return;
    // if(object->_parent == NULL) return;

    if(object->__calculate_nsize_fn != NULL)
        object->__calculate_nsize_fn(object);
}

void ntg_object_constrain(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__constrain_fn != NULL)
        object->__constrain_fn(object);
}

void ntg_object_measure(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__measure_fn != NULL)
        object->__measure_fn(object);
}

void ntg_object_arrange(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__arrange_fn != NULL)
        object->__arrange_fn(object);

    __construct_full_drawing(object);
}

struct ntg_xy ntg_object_get_nsize(const ntg_object* object)
{
    assert(object != NULL);

    return object->__nsize;
}

struct ntg_xy ntg_object_get_content_nsize(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy pref_border_size = ntg_xy(
            object->_border_pref_size.west + object->_border_pref_size.east,
            object->_border_pref_size.north + object->_border_pref_size.south);

    return ntg_xy_sub(object->__nsize, pref_border_size);
}

struct ntg_constr ntg_object_get_content_constr(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_size = ntg_xy(
            object->_border_size.west + object->_border_size.east,
            object->_border_size.north + object->_border_size.south);

    struct ntg_xy min_size = ntg_xy_size(ntg_xy_sub(object->__constr.min_size,
                border_size));

    struct ntg_xy max_size = ntg_xy_size(ntg_xy_sub(object->__constr.max_size,
                border_size));

    return ntg_constr(min_size, max_size);
}

struct ntg_constr ntg_object_get_constr(const ntg_object* object)
{
    assert(object != NULL);

    return object->__constr;
}

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    assert(object != NULL);

    return object->__size;
}

struct ntg_xy ntg_object_get_content_size(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_size = ntg_xy(
            object->_border_size.west + object->_border_size.east,
            object->_border_size.north + object->_border_size.south);

    return ntg_xy_size(ntg_xy_sub(object->__size, border_size));
}

struct ntg_xy ntg_object_get_pos_abs(const ntg_object* object)
{
    assert(object != NULL);

    return object->__pos;
}

struct ntg_xy ntg_object_get_pos_rel(const ntg_object* object)
{
    assert(object != NULL);
    if(object == NULL) return ntg_xy(SIZE_MAX, SIZE_MAX);

    if(object->_parent != NULL)
        return ntg_xy_sub(ntg_object_get_pos_abs(object),
                ntg_object_get_pos_abs(object->_parent));
    else
        return object->__pos;
}

struct ntg_xy ntg_object_get_content_pos_abs(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_offset = ntg_xy(object->_border_size.west,
            object->_border_size.north);

    return ntg_xy_add(ntg_object_get_pos_abs(object), border_offset);
}

struct ntg_xy ntg_object_get_content_pos_rel(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_offset = ntg_xy(object->_border_size.west,
            object->_border_size.north);

    return ntg_xy_add(ntg_object_get_pos_rel(object), border_offset);
}

bool ntg_object_feed_key(ntg_object* object, struct nt_key_event key_event)
{
    assert(object != NULL);

    if(object->__process_key_fn != NULL)
        return object->__process_key_fn(object, key_event);
    else
        return false;
}

void _ntg_object_set_scene(ntg_object* root, ntg_scene* scene)
{
    assert(root != NULL);

    _ntg_object_perform_tree(root, __adjust_scene_fn, scene);
}

static void __calculate_border_size(ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy total_size = object->__constr.max_size;
    struct ntg_xy pref_border_size = ntg_xy(
            object->_border_pref_size.west + object->_border_pref_size.east,
            object->_border_pref_size.north + object->_border_pref_size.south);

    struct ntg_xy content_nsize = ntg_object_get_content_nsize(object);
    // struct ntg_xy content_size = ntg_xy_sub(total_size, pref_border_size);

    struct ntg_xy border_total_size;

    if(total_size.x >= content_nsize.x)
    {
        if((pref_border_size.x + content_nsize.x) <= total_size.x)
            border_total_size.x = pref_border_size.x;
        else
            border_total_size.x = total_size.x - content_nsize.x;
    }
    else
    {
        border_total_size.x = 0;
    }

    if(total_size.y >= content_nsize.y)
    {
        if((pref_border_size.y + content_nsize.y) <= total_size.y)
            border_total_size.y = pref_border_size.y;
        else
            border_total_size.y = total_size.y - content_nsize.y;
    }
    else
    {
        border_total_size.y = 0;
    }

    size_t ns_nsizes[] = { 
        object->_border_pref_size.north,
        object->_border_pref_size.south
    };
    size_t _ns_sizes[2] = {0};

    ntg_sap_nsize_round_robin(ns_nsizes, _ns_sizes, border_total_size.y, 2);

    object->_border_size.north = _ns_sizes[0];
    object->_border_size.south = _ns_sizes[1];

    size_t we_nsizes[] = { 
        object->_border_pref_size.west,
        object->_border_pref_size.east
    };
    size_t _we_sizes[2] = {0};

    ntg_sap_nsize_round_robin(we_nsizes, _we_sizes, border_total_size.x, 2);

    object->_border_size.west = _we_sizes[0];
    object->_border_size.east = _we_sizes[1];
}

static void __construct_full_drawing(ntg_object* object)
{
    struct ntg_xy content_size = ntg_object_get_content_size(object);
    struct ntg_xy border_offset = ntg_xy(object->_border_size.west, object->_border_size.north);

    if(object->__scroll)
    {
        ntg_object_drawing_place(
                object->_virtual_drawing, object->__curr_scroll, content_size,
                object->_full_drawing, border_offset);
    }
    else
    {
        ntg_object_drawing_place(
                object->_virtual_drawing, ntg_xy(0, 0), content_size,
                object->_full_drawing, border_offset);
    }

    struct ntg_xy size = ntg_object_get_size(object);

    struct ntg_border_style border = object->_border_style;

    size_t north_width = object->_border_size.north;
    size_t east_width = object->_border_size.east;
    size_t south_width = object->_border_size.south;
    size_t west_width = object->_border_size.west;

    struct ntg_xy center_size = ntg_xy(size.x - (east_width + west_width),
            size.y - (north_width + south_width));

    struct ntg_xy center_box_start = ntg_xy(west_width, north_width);
    struct ntg_xy center_box_end = ntg_xy(west_width + center_size.x,
            north_width + center_size.y);

    struct ntg_constr center_box = ntg_constr(center_box_start, center_box_end);

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            if(ntg_constr_contains(center_box, ntg_xy(j, i))) continue;

            it_cell = ntg_object_drawing_at_(object->_full_drawing, ntg_xy(j, i));

            if((north_width > 0) && (i == 0) && (j == 0))
            {
                *it_cell = border.north_west_corner;
            }
            else if((north_width > 0) && (i == 0) && (j == (size.x - 1)))
            {
                *it_cell = border.north_east_corner;
            }
            else if((south_width > 0) && (i == (size.y - 1)) && (j == 0))
            {
                *it_cell = border.south_west_corner;
            }
            else if((south_width > 0) && (i == (size.y - 1)) &&
                    (j == (size.x - 1)))
            {
                *it_cell = border.south_east_corner;
            }
            else if((north_width > 0) && (i == 0))
            {
                *it_cell = border.north_line;
            }
            else if((west_width > 0) && (j == 0))
            {
                *it_cell = border.west_line;
            }
            else if((south_width > 0) && (i == (size.y - 1)))
            {
                *it_cell = border.south_line;
            }
            else if((east_width > 0) && (j == (size.x - 1)))
            {
                *it_cell = border.east_line;
            }
            else
            {
                *it_cell = border.border_inside;
            }
        }
    }
}
