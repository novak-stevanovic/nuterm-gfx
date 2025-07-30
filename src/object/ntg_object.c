#include <stdlib.h>
#include <assert.h>

#include "object/ntg_object.h"
#include "base/ntg_event_types.h"
#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"
#include "core/ntg_scene.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"

/* -------------------------------------------------------------------------- */
/* STATIC DECLARATIONS */
/* -------------------------------------------------------------------------- */

static struct ntg_border_size __border_size_round_robin(
        struct ntg_border_size pref_border_size,
        struct ntg_xy border_size_sum);
static void __ntg_object_init_default(ntg_object* object);
static void __construct_full_drawing(ntg_object* object);
static void __adjust_constr(size_t pref_size, size_t* min_size, size_t* max_size);
static void _fit_size_to_constr(size_t* size, size_t min_size, size_t max_size);
static void __adjust_scene_fn(ntg_object* object, void* scene);

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

struct ntg_border_size ntg_border_size_fn_show_if_can_fit(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy total_size = ntg_object_get_constr(object).max_size;
    struct ntg_border_size border_nsize = ntg_object_get_natural_border_size(object);
    struct ntg_xy border_nsize_sum = ntg_border_size_sum(border_nsize);
    struct ntg_xy content_nsize = ntg_object_get_natural_content_size(object);

    struct ntg_xy border_total_size;

    if(total_size.x >= content_nsize.x)
    {
        if((border_nsize_sum.x + content_nsize.x) <= total_size.x)
            border_total_size.x = border_nsize_sum.x;
        else
            border_total_size.x = total_size.x - content_nsize.x;
    }
    else
        border_total_size.x = 0;

    if(total_size.y >= content_nsize.y)
    {
        if((border_nsize_sum.y + content_nsize.y) <= total_size.y)
            border_total_size.y = border_nsize_sum.y;
        else
            border_total_size.y = total_size.y - content_nsize.y;
    }
    else
        border_total_size.y = 0;

    return __border_size_round_robin(border_nsize, border_total_size);
}

struct ntg_border_size ntg_border_size_fn_always_show(const ntg_object* object)
{
    struct ntg_border_size border_size = __border_size_round_robin(
            ntg_object_get_natural_border_size(object),
            ntg_object_get_constr(object).max_size);

    return border_size;
}

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

    struct ntg_xy pref_border_size = ntg_border_size_sum(
            object->_border_pref_size);

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

    if(ntg_xy_are_equal(object->__pref_size, pref_size)) return;

    struct ntg_size_change data = {
        .old = object->__pref_size,
        .new = pref_size
    };

    object->__pref_size = pref_size;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_OBJECT_PREF_SIZE_CHANGE, object, &data);
    ntg_listenable_raise(&object->__listenable, &e);
}

struct ntg_xy ntg_object_get_natural_size(const ntg_object* object)
{
    assert(object != NULL);

    return object->__natural_size;
}

struct ntg_xy ntg_object_get_natural_content_size(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy pref_border_size = ntg_border_size_sum(
            object->_border_pref_size);

    return ntg_xy_sub(object->__natural_size, pref_border_size);
}

struct ntg_border_size ntg_object_get_natural_border_size(const ntg_object* object)
{
    return object->__natural_border_size;
}

struct ntg_constr ntg_object_get_content_constr(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_constr = ntg_border_size_sum(object->__border_constr);

    // struct ntg_xy min_size = ntg_xy_size(ntg_xy_sub(object->__constr.min_size,
    //             border_size));
    //
    // struct ntg_xy max_size = ntg_xy_size(ntg_xy_sub(object->__constr.max_size,
    //             border_size));

    struct ntg_xy min_size = ntg_xy_sub(object->__constraints.min_size,
                border_constr);

    struct ntg_xy max_size = ntg_xy_sub(object->__constraints.max_size,
                border_constr);

    return ntg_constr(min_size, max_size);
}

struct ntg_constr ntg_object_get_constr(const ntg_object* object)
{
    assert(object != NULL);

    return object->__constraints;
}

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    assert(object != NULL);

    return object->__size;
}

struct ntg_xy ntg_object_get_content_size(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_size = ntg_border_size_sum(object->__border_size);

    return ntg_xy_size(ntg_xy_sub(object->__size, border_size));
}

struct ntg_border_size ntg_object_get_border_size(const ntg_object* object)
{
    return object->__border_size;
}

struct ntg_xy ntg_object_get_pos_abs(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy pos = ntg_xy(0, 0);
    const ntg_object* it_obj = object;
    while(it_obj != NULL)
    {
        pos = ntg_xy_add(pos, ntg_object_get_pos_rel(it_obj));
        it_obj = it_obj->_parent;
    }

    return pos;
}

struct ntg_xy ntg_object_get_pos_rel(const ntg_object* object)
{
    assert(object != NULL);

    return object->__pos;
}

struct ntg_xy ntg_object_get_content_pos_abs(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_offset = ntg_border_size_offset(object->__border_size);

    return ntg_xy_add(ntg_object_get_pos_abs(object), border_offset);
}

struct ntg_xy ntg_object_get_content_pos_rel(const ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy border_offset = ntg_border_size_offset(object->__border_size);

    return ntg_xy_add(object->__pos, border_offset);
}

void ntg_object_calculate_natural_size(ntg_object* object)
{
    if(object == NULL) return;

    if(object->__natural_size_fn != NULL)
        object->__natural_size_fn(object);
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

void ntg_object_set_border_style(ntg_object* object,
        struct ntg_border_style border_style)
{
    assert(object != NULL);

    if(ntg_border_style_are_equal(object->_border_style, border_style)) return;

    object->_border_style = border_style;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_OBJECT_CONTENT_INVALID, object, NULL);
    ntg_listenable_raise(&object->__listenable, &e);
}

void ntg_object_set_border_pref_size(ntg_object* object,
        struct ntg_border_size border_size)
{
    assert(object != NULL);

    if(ntg_border_size_are_equal(object->_border_pref_size, border_size)) return;
    
    object->_border_pref_size = border_size;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_OBJECT_LAYOUT_INVALID, object, NULL);
    ntg_listenable_raise(&object->__listenable, &e);
}

void ntg_object_set_border_size_fn(ntg_object* object,
        ntg_border_size_fn calculate_border_size_fn)
{
    assert(object != NULL);

    if(object->__border_size_fn == calculate_border_size_fn) return;

    object->__border_size_fn = calculate_border_size_fn;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_OBJECT_LAYOUT_INVALID, object, NULL);
    ntg_listenable_raise(&object->__listenable, &e);
}

bool ntg_object_feed_key(ntg_object* object, struct nt_key_event key_event)
{
    assert(object != NULL);

    if(object->__process_key_fn != NULL)
        return object->__process_key_fn(object, key_event);
    else
        return false;
}

void ntg_object_listen(ntg_object* object, struct ntg_event_sub subscription)
{
    ntg_listenable_listen(&object->__listenable, subscription);
}

void ntg_object_stop_listening(ntg_object* object, void* subscriber)
{
    ntg_listenable_stop_listening(&object->__listenable, subscriber);
}

void _ntg_object_set_scene(ntg_object* root, ntg_scene* scene)
{
    assert(root != NULL);

    // TODO: event?

    _ntg_object_perform_tree(root, __adjust_scene_fn, scene);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object* object,
        ntg_natural_size_fn natural_size_fn,
        ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn)
{
    assert(object != NULL); 

    __ntg_object_init_default(object);

    object->__constrain_fn = constrain_fn;
    object->__measure_fn = measure_fn;
    object->__arrange_fn = arrange_fn;
    object->__natural_size_fn = natural_size_fn;

    object->_virtual_drawing = ntg_object_drawing_new();
    assert(object->_virtual_drawing != NULL);

    object->_full_drawing = ntg_object_drawing_new();
    assert(object->_full_drawing != NULL);

    object->_children = ntg_object_vec_new();

    __ntg_listenable_init__(&object->__listenable);
}

void __ntg_object_deinit__(ntg_object* object)
{
    assert(object != NULL); 

    ntg_object_drawing_destroy(object->_virtual_drawing);
    ntg_object_drawing_destroy(object->_full_drawing);

    if(object->_children != NULL)
        ntg_object_vec_destroy(object->_children);

    __ntg_listenable_deinit__(&object->__listenable);
    __ntg_object_init_default(object);
}

void _ntg_object_set_natural_content_size(ntg_object* object, struct ntg_xy size)
{
    if(object == NULL) return;

    struct ntg_border_size border_pref_size = object->_border_pref_size;

    object->__natural_border_size = border_pref_size;

    if((size.x == 0) || (size.y == 0))
    {
        object->__natural_size = ntg_xy(0, 0);
        return;
    }

    struct ntg_xy border_pref_size_sum = ntg_border_size_sum(border_pref_size);

    size = ntg_xy_add(size, border_pref_size_sum);

    if(object->__pref_size.x != NTG_PREF_SIZE_UNSET)
        size.x = object->__pref_size.x;

    if(object->__pref_size.y != NTG_PREF_SIZE_UNSET)
        size.y = object->__pref_size.y;

    object->__natural_size = size;
}

void _ntg_object_set_constr(ntg_object* object, struct ntg_constr constr)
{
    if(object == NULL) return;

    __adjust_constr(object->__pref_size.x, &constr.min_size.x, &constr.max_size.x);
    __adjust_constr(object->__pref_size.y, &constr.min_size.y, &constr.max_size.y);

    object->__constraints = constr;

    struct ntg_border_size border_constr = object->__border_size_fn(object);
    struct ntg_xy border_size_sum = ntg_border_size_sum(border_constr);

    if(border_size_sum.x > constr.max_size.x)
    {
        size_t _sizes[2];
        size_t nsizes[] = { 
            object->__natural_border_size.west,
            object->__natural_border_size.east
        };

        ntg_sap_nsize_round_robin(nsizes, _sizes, constr.max_size.x, 2);

        border_constr.west = _sizes[0];
        border_constr.east = _sizes[1];
    }
    if(border_size_sum.y > constr.max_size.y)
    { 
        size_t _sizes[2];
        size_t nsizes[] = { 
            object->__natural_border_size.north,
            object->__natural_border_size.south
        };

        ntg_sap_nsize_round_robin(nsizes, _sizes, constr.max_size.y, 2);

        border_constr.north = _sizes[0];
        border_constr.south = _sizes[1];
    }

    object->__border_constr = border_constr;
}

void _ntg_object_set_content_size(ntg_object* object, struct ntg_xy content_size)
{
    if(object == NULL) return;

    struct ntg_constr content_constr = ntg_object_get_content_constr(object);

    _fit_size_to_constr(&(content_size.x), content_constr.min_size.x,
            content_constr.max_size.x);
    _fit_size_to_constr(&(content_size.y), content_constr.min_size.y,
            content_constr.max_size.y);

    // content_size = ntg_xy_size(content_size);

    struct ntg_border_size border_size = object->__border_constr;
    object->__border_size = border_size;
    struct ntg_xy border_size_sum = ntg_border_size_sum(border_size);

    struct ntg_xy size = ntg_xy_add(content_size, border_size_sum);
    object->__size = size;

    struct ntg_xy content_nsize = ntg_object_get_natural_content_size(object);

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

    struct ntg_xy border_offset = (parent != NULL) ?
        ntg_border_size_offset(parent->__border_size) :
        ntg_xy(0, 0);

    struct ntg_xy final_pos = ntg_xy_add(border_offset, pos);

    object->__pos = final_pos;
}

void _ntg_object_scroll_enable(ntg_object* object)
{
    assert(object != NULL);

    if(!(object->__scroll))
    {
        object->__scroll = true;
        object->__curr_scroll = ntg_xy(0, 0);
        object->__buffered_scroll = ntg_dxy(0, 0);

        ntg_event e;
        __ntg_event_init__(&e, NTG_ETYPE_OBJECT_LAYOUT_INVALID, object, NULL);
        ntg_listenable_raise(&object->__listenable, &e);
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

        ntg_event e;
        __ntg_event_init__(&e, NTG_ETYPE_OBJECT_LAYOUT_INVALID, object, NULL);
        ntg_listenable_raise(&object->__listenable, &e);
    }
}

void _ntg_object_scroll(ntg_object* object, struct ntg_dxy offset_diff)
{
    assert(object != NULL);

    if(object->__scroll)
    {
        object->__buffered_scroll = ntg_dxy_add(object->__buffered_scroll,
                offset_diff);

        ntg_event e;
        __ntg_event_init__(&e, NTG_ETYPE_OBJECT_CONTENT_INVALID, object, NULL);
        ntg_listenable_raise(&object->__listenable, &e);
    }
}

struct ntg_xy _ntg_object_get_scroll(const ntg_object* object)
{
    return object->__curr_scroll;
}

void _ntg_object_child_add(ntg_object* parent, ntg_object* child)
{
    assert((parent != NULL) && (child != NULL));

    if(child->_parent == parent) return;

    ntg_object* curr_parent = child->_parent;

    if(curr_parent != NULL)
    {
        _ntg_object_child_remove(curr_parent, child);
    }

    ntg_object_vec_append(parent->_children, child);

    child->_parent = parent;

    _ntg_object_perform_tree(child, __adjust_scene_fn, parent->_scene);

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_OBJECT_LAYOUT_INVALID, parent, NULL);
    ntg_listenable_raise(&parent->__listenable, &e);
}

void _ntg_object_child_remove(ntg_object* parent, ntg_object* child)
{
    assert((parent != NULL) && (child != NULL));

    if(child->_parent != parent) return;

    ntg_object_vec_remove(parent->_children, child);

    child->_parent = NULL;

    _ntg_object_perform_tree(child, __adjust_scene_fn, NULL);

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_OBJECT_LAYOUT_INVALID, parent, NULL);
    ntg_listenable_raise(&parent->__listenable, &e);
}

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn)
{
    assert(object != NULL);

    object->__process_key_fn = process_key_fn;
}

ntg_listenable* _ntg_object_get_listenable(ntg_object* object)
{
    assert(object != NULL);

    return &object->__listenable;
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

/* -------------------------------------------------------------------------- */

static struct ntg_border_size __border_size_round_robin(
        struct ntg_border_size pref_border_size,
        struct ntg_xy border_size_sum)
{
    size_t size_north, size_east, size_south, size_west;

    size_t ns_nsizes[] = { 
        pref_border_size.north,
        pref_border_size.south
    };
    size_t _ns_sizes[2] = {0};

    ntg_sap_nsize_round_robin(ns_nsizes, _ns_sizes, border_size_sum.y, 2);

    size_north = _ns_sizes[0];
    size_south = _ns_sizes[1];

    size_t we_nsizes[] = { 
        pref_border_size.west,
        pref_border_size.east
    };
    size_t _we_sizes[2] = {0};

    ntg_sap_nsize_round_robin(we_nsizes, _we_sizes, border_size_sum.x, 2);

    size_west = _we_sizes[0];
    size_east = _we_sizes[1];

    return (struct ntg_border_size) {
        .north = size_north,
        .east = size_east,
        .south = size_south,
        .west = size_west
    };
}

static void __ntg_object_init_default(ntg_object* object)
{
    object->_parent = NULL;
    object->_children = NULL;

    object->__pref_size = ntg_xy(NTG_PREF_SIZE_UNSET, NTG_PREF_SIZE_UNSET);

    object->_border_pref_size = NTG_BORDER_SIZE_DEFAULT;
    object->_border_style = ntg_border_style_default();
    object->__border_size_fn = ntg_border_size_fn_always_show;
 
    object->_virtual_drawing = NULL;
    object->__scroll = false;
    object->__curr_scroll = ntg_xy(0, 0);
    object->__buffered_scroll = ntg_dxy(0, 0);
    object->_full_drawing = NULL;

    object->__natural_size = NTG_XY_UNSET;
    object->__natural_border_size = NTG_BORDER_SIZE_DEFAULT;

    object->__constraints = NTG_CONSTR_UNSET;
    object->__border_constr = NTG_BORDER_SIZE_DEFAULT;

    object->__size = NTG_XY_UNSET;
    object->__border_size = NTG_BORDER_SIZE_DEFAULT;

    object->__pos = NTG_XY_UNSET;

    object->__natural_size_fn = NULL;
    object->__constrain_fn = NULL;
    object->__measure_fn = NULL;
    object->__arrange_fn = NULL;

    object->__process_key_fn = NULL;

    object->_scene = NULL;
}

static void __construct_full_drawing(ntg_object* object)
{
    struct ntg_xy content_size = ntg_object_get_content_size(object);
    struct ntg_xy border_offset = ntg_border_size_offset(object->__border_size);

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

    size_t north_width = object->__border_size.north;
    size_t east_width = object->__border_size.east;
    size_t south_width = object->__border_size.south;
    size_t west_width = object->__border_size.west;

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

static void __adjust_constr(size_t pref_size, size_t* min_size, size_t* max_size)
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

static void _fit_size_to_constr(size_t* size, size_t min_size, size_t max_size)
{
    if((*size) > max_size)
        (*size) = max_size;

    else if((*size) < min_size)
        (*size) = min_size;
}

static void __adjust_scene_fn(ntg_object* object, void* scene)
{
    if(object->_scene == scene) return;
    if(object == NULL) return;

    if(object->_scene != NULL)
    {
        ntg_scene_unregister_object(object->_scene, object);
    }
    object->_scene = (ntg_scene*)scene;
    if(scene != NULL)
    {
        ntg_scene_register_object(NTG_SCENE(scene), object);
    }
}
