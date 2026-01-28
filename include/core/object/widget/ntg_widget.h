#ifndef NTG_WIDGET_H
#define NTG_WIDGET_H

#include "core/object/ntg_object.h"
#include "thirdparty/genc.h"

struct ntg_widget_layout_ops
{
    ntg_widget_layout_init init_fn;
    ntg_widget_layout_deinit deinit_fn;
    ntg_widget_measure_fn measure_fn;
    ntg_widget_constrain_fn constrain_fn;
    ntg_widget_arrange_fn arrange_fn;
    ntg_widget_draw_fn draw_fn;
};

struct ntg_widget_hooks
{
    void (*on_rm_child_fn)(ntg_widget* widget, ntg_widget* child);
};

GENC_VECTOR_GENERATE(ntg_widget_vec, ntg_widget*, 1.5, NULL);

struct ntg_widget
{
    ntg_object __base;

    struct ntg_widget_layout_ops __layout_ops;
    struct ntg_widget_hooks __hooks;

    struct ntg_vcell _background;
    struct ntg_xy _cont_min_size, _cont_max_size, _grow;

    ntg_decorator *_padding, *_border;

    struct
    {
        // void (*on_focus_gained_fn)(ntg_widget* widget);
        // void (*on_focus_lost_fn)(ntg_widget* widget);
        // void (*on_focus_out_of_scope_fn)(ntg_widget* widget);
        bool (*__on_key_fn)(ntg_widget* widget, struct nt_key_event key);
        bool (*__on_mouse_fn)(ntg_widget* widget, struct nt_mouse_event mouse);
    };

    void* data;
};

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

#define NTG_WIDGET_MIN_SIZE_UNSET 0
#define NTG_WIDGET_MAX_SIZE_UNSET NTG_SIZE_MAX
#define NTG_WIDGET_GROW_UNSET NTG_SIZE_MAX

void ntg_widget_set_cont_min_size(ntg_widget* widget, struct ntg_xy size);
void ntg_widget_set_cont_max_size(ntg_widget* widget, struct ntg_xy size);
void ntg_widget_set_grow(ntg_widget* widget, struct ntg_xy grow);
void ntg_widget_set_background(ntg_widget* widget, struct ntg_vcell background);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

void ntg_widget_set_on_key_fn(ntg_widget* widget,
        bool (*on_key_fn)(ntg_widget* widget, struct nt_key_event key));

void ntg_widget_set_on_mouse_fn(ntg_widget* widget,
        bool (*on_mouse_fn)(ntg_widget* widget, struct nt_mouse_event mouse));

bool ntg_widget_on_key(ntg_widget* widget, struct nt_key_event key);
bool ntg_widget_on_mouse(ntg_widget* widget, struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */
/* SCENE STATE */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_widget_get_min_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_min_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_nat_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_nat_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_max_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_max_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_grow(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_pos(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_pos_abs(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_pos(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_pos_abs(const ntg_widget* widget);

int ntg_widget_get_z_index(const ntg_widget* widget);
void ntg_widget_set_z_index(ntg_widget* widget, int z_index);

/* LAYOUT PROCESS - CONVENIENCE */

size_t ntg_widget_get_cont_for_size(
        const ntg_widget* widget,
        ntg_orient orient,
        bool constrain);

struct ntg_object_measure
ntg_widget_get_measure(const ntg_widget* widget, ntg_orient orient);

struct ntg_object_measure
ntg_widget_get_cont_measure(const ntg_widget* widget, ntg_orient orient);

size_t ntg_widget_get_size_1d(const ntg_widget* widget, ntg_orient orient);
size_t ntg_widget_get_cont_size_1d(const ntg_widget* widget, ntg_orient orient);

/* -------------------------------------------------------------------------- */
/* WIDGET TREE */
/* -------------------------------------------------------------------------- */

bool ntg_widget_is_ancestor(const ntg_widget* widget, const ntg_widget* ancestor);
bool ntg_widget_is_ancestor_eq(const ntg_widget* widget, const ntg_widget* ancestor);
bool ntg_widget_is_descendant(const ntg_widget* widget, const ntg_widget* descendant);
bool ntg_widget_is_descendant_eq(const ntg_widget* widget, const ntg_widget* descendant);

const ntg_widget* ntg_widget_get_parent(const ntg_widget* widget);
ntg_widget* ntg_widget_get_parent_(ntg_widget* widget);

const ntg_object* ntg_widget_get_group_root(const ntg_widget* widget);
ntg_object* ntg_widget_get_group_root_(ntg_widget* widget);

size_t ntg_widget_get_children(const ntg_widget* widget, ntg_widget** out_buff);
size_t ntg_widget_get_children_by_z(const ntg_widget* widget, ntg_widget** out_buff);

void ntg_widget_set_padding(ntg_widget* widget, ntg_decorator* padding);

void ntg_widget_set_border(ntg_widget* widget, ntg_decorator* border);

struct ntg_xy ntg_widget_map_to_ancestor_space(
        const ntg_widget* widget_space,
        const ntg_widget* ancestor_space,
        struct ntg_xy point);

struct ntg_xy ntg_widget_map_to_descendant_space(
        const ntg_widget* widget_space,
        const ntg_widget* descendant_space,
        struct ntg_xy point);

struct ntg_xy ntg_widget_map_to_scene_space(
        const ntg_widget* widget_space,
        struct ntg_xy point);

struct ntg_xy ntg_widget_map_from_scene_space(
        const ntg_widget* widget_space,
        struct ntg_xy point);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_widget_init(ntg_widget* widget,
        struct ntg_widget_layout_ops layout_ops,
        struct ntg_widget_hooks hooks);
void ntg_widget_deinit(ntg_widget* widget);

/* If child is already attached, performs full detachment. Then it updates the
 * object tree in accordance with the new state.
 * Called internally by types extending ntg_widget. */
void ntg_widget_attach(ntg_widget* parent, ntg_widget* child);

/* Performs full detaching of widget. Called internally by types extending ntg_widget. */
void ntg_widget_detach(ntg_widget* widget);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// void _ntg_widget_make_scene_root(ntg_widget* widget, ntg_scene* scene);

void* _ntg_widget_layout_init_fn(const ntg_object* _widget);
void _ntg_widget_layout_deinit_fn(void* data, const ntg_object* _widget);

struct ntg_object_measure _ntg_widget_measure_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void _ntg_widget_constrain_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena);

void _ntg_widget_arrange_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_object_xy_map* out_pos_map,
        sarena* arena);

void _ntg_widget_draw_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_WIDGET_H
