#ifndef NTG_WIDGET_H
#define NTG_WIDGET_H

#include "core/object/ntg_object.h"

struct ntg_widget_layout_ops
{
    ntg_widget_layout_data_init init_fn;
    ntg_widget_layout_data_deinit deinit_fn;
    ntg_widget_measure_fn measure_fn;
    ntg_widget_constrain_fn constrain_fn;
    ntg_widget_arrange_fn arrange_fn;
    ntg_widget_draw_fn draw_fn;
};

struct ntg_widget_hooks
{
    void (*rm_child_fn)(ntg_widget* widget, ntg_widget* child);
};

struct ntg_widget
{
    ntg_object __base;

    struct ntg_widget_layout_ops __layout_ops;
    struct ntg_widget_hooks __hooks;

    struct ntg_vcell _background;
    struct ntg_xy _user_min_size, _user_max_size, _user_grow;

    ntg_decorator *_padding, *_border;

    ntg_widget_event_fn __event_fn;
    void* data;
};

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

void ntg_widget_set_user_min_size(ntg_widget* widget, struct ntg_xy size);
void ntg_widget_set_user_max_size(ntg_widget* widget, struct ntg_xy size);
void ntg_widget_set_user_grow(ntg_widget* widget, struct ntg_xy grow);
void ntg_widget_set_background(ntg_widget* widget, struct ntg_vcell background);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_widget_feed_event(ntg_widget* widget,
        struct nt_event event, ntg_loop_ctx* ctx);

void ntg_widget_set_event_fn(ntg_widget* widget, ntg_widget_event_fn event_fn);

/* -------------------------------------------------------------------------- */
/* SCENE STATE */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_widget_get_min_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_content_min_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_nat_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_content_nat_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_max_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_content_max_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_grow(const ntg_widget* widget);

struct ntg_xy widget_get_size(const ntg_widget* widget);
struct ntg_xy widget_get_content_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_position(const ntg_widget* widget);
struct ntg_xy widget_get_content_position(const ntg_widget* widget);

/* LAYOUT PROCESS - CONVENIENCE */

struct ntg_widget_measure
ntg_widget_get_measure(const ntg_widget* widget, ntg_orientation orientation);

size_t ntg_widget_get_size(const ntg_widget* widget, ntg_orientation orientation);

/* -------------------------------------------------------------------------- */
/* WIDGET TREE */
/* -------------------------------------------------------------------------- */

bool ntg_widget_is_ancestor(const ntg_widget* widget, const ntg_widget* ancestor);
bool ntg_widget_is_descendant(const ntg_widget* widget, const ntg_widget* descendant);

const ntg_widget* ntg_widget_get_parent(const ntg_widget* widget);
ntg_widget* ntg_widget_get_parent_(ntg_widget* widget);

const ntg_object* ntg_widget_get_group_root(const ntg_widget* widget);
ntg_object* ntg_widget_get_group_root_(ntg_widget* widget);

size_t ntg_widget_get_children_count(const ntg_widget* widget);
size_t ntg_widget_get_children(const ntg_widget* widget,
        const ntg_widget** out_children, size_t cap);
size_t ntg_widget_get_children_(ntg_widget* widget,
        ntg_widget** out_children, size_t cap);

void ntg_widget_set_padding(ntg_widget* widget, ntg_decorator* padding);

void ntg_widget_set_border(ntg_widget* widget, ntg_decorator* border);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_widget_init(ntg_widget* widget,
        struct ntg_widget_layout_ops layout_ops,
        struct ntg_widget_hooks hooks);
void ntg_widget_deinit_fn(ntg_entity* _widget);

void ntg_widget_add_child(ntg_widget* parent, ntg_widget* child);
void ntg_widget_rm_child(ntg_widget* parent, ntg_widget* child);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// void _ntg_widget_make_scene_root(ntg_widget* widget, ntg_scene* scene);

void* _ntg_widget_layout_data_init_fn(const ntg_object* _widget);
void _ntg_widget_layout_data_deinit_fn(void* data, const ntg_object* _widget);

struct ntg_object_measure _ntg_widget_measure_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

void _ntg_widget_constrain_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena);

void _ntg_widget_arrange_fn(
        const ntg_object* _widget,
        void* _layout_data,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena);

void _ntg_widget_draw_fn(
        const ntg_object* _widget,
        void* _layout_data,
        struct ntg_xy size,
        ntg_temp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_WIDGET_H
