// #ifndef NTG_WIDGET_H
// #define NTG_WIDGET_H
// 
// #include "shared/ntg_shared.h"
// #include "core/object/ntg_object.h"
// #include "thirdparty/genc.h"
// 
// /* ========================================================================== */
// /* PUBLIC DEFINITIONS */
// /* ========================================================================== */
// 
// struct ntg_widget_hooks
// {
//     void (*on_rm_child_fn)(ntg_widget* widget, ntg_widget* child);
// };
// 
// struct ntg_widget
// {
//     ntg_object __base;
// 
//     struct ntg_widget_layout_ops __layout_ops;
//     struct ntg_widget_hooks __hooks;
// 
//     struct
//     {
//         // void (*on_focus_gained_fn)(ntg_widget* widget);
//         // void (*on_focus_lost_fn)(ntg_widget* widget);
//         // void (*on_focus_out_of_scope_fn)(ntg_widget* widget);
//         bool (*__on_key_fn)(ntg_widget* widget, struct nt_key_event key);
//         bool (*__on_mouse_fn)(ntg_widget* widget, struct nt_mouse_event mouse);
//     };
// 
//     ntg_decorator* _padding;
//     ntg_decorator* _border;
// 
//     void* data;
// };
// 
// GENC_VECTOR_GENERATE(ntg_widget_vec, ntg_widget*, 1.5, NULL);
// 
// /* ========================================================================== */
// /* PUBLIC FUNCTIONS */
// /* ========================================================================== */
// 
// /* -------------------------------------------------------------------------- */
// /* DECORATOR */
// /* -------------------------------------------------------------------------- */
// 
// const ntg_object* ntg_widget_get_outer(const ntg_widget* widget);
// ntg_object* ntg_widget_get_outer_(ntg_widget* widget);
// 
// void ntg_widget_set_padding(ntg_widget* widget, ntg_decorator* padding);
// void ntg_widget_set_border(ntg_widget* widget, ntg_decorator* border);
// 
// ntg_object* ntg_widget_hit_test_dcr(
//         ntg_widget* widget,
//         struct ntg_xy pos,
//         struct ntg_xy* out_local_pos);
// 
// /* -------------------------------------------------------------------------- */
// /* WIDGET TREE */
// /* -------------------------------------------------------------------------- */
// 
// bool ntg_widget_is_ancestor(const ntg_widget* widget, const ntg_widget* ancestor);
// bool ntg_widget_is_ancestor_eq(const ntg_widget* widget, const ntg_widget* ancestor);
// bool ntg_widget_is_descendant(const ntg_widget* widget, const ntg_widget* descendant);
// bool ntg_widget_is_descendant_eq(const ntg_widget* widget, const ntg_widget* descendant);
// 
// const ntg_widget* ntg_widget_get_parent(const ntg_widget* widget);
// ntg_widget* ntg_widget_get_parent_(ntg_widget* widget);
// 
size_t ntg_widget_get_children(
        ntg_widget* widget,
        ntg_widget** out_buff,
        size_t cap);
size_t ntg_widget_get_children_by_z(
        ntg_widget* widget,
        ntg_widget** out_buff,
        size_t cap);

ntg_widget* ntg_widget_hit_test(
        ntg_widget* widget,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos);

ntg_scene_layer* ntg_widget_get_scene_layer_(ntg_widget* widget);
const ntg_scene_layer* ntg_widget_get_scene_layer(const ntg_widget* widget);

ntg_scene* ntg_widget_get_scene_(ntg_widget* widget);
const ntg_scene* ntg_widget_get_scene(const ntg_widget* widget);

/* -------------------------------------------------------------------------- */
/* SCENE STATE */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_widget_get_outer_min_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_min_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_outer_nat_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_nat_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_outer_max_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_max_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_grow(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_outer_size(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_size(const ntg_widget* widget);

struct ntg_xy ntg_widget_get_outer_pos(const ntg_widget* widget);
struct ntg_xy ntg_widget_get_cont_pos(const ntg_widget* widget);

int ntg_widget_get_z_index(const ntg_widget* widget);
void ntg_widget_set_z_index(ntg_widget* widget, int z_index);

/* -------------------------------------------------------------------------- */
/* SPACE MAPPING */
/* -------------------------------------------------------------------------- */

struct ntg_dxy ntg_widget_map_to_ancestor_space(
        const ntg_widget* widget,
        const ntg_widget* ancestor,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_to_descendant_space(
        const ntg_widget* widget,
        const ntg_widget* descendant,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_to_scene_layer_space(
        const ntg_widget* widget,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_from_scene_layer_space(
        const ntg_widget* widget,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_to_scene_space(
        const ntg_widget* widget,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_from_scene_space(
        const ntg_widget* widget,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_from_cont_space(
        const ntg_widget* widget,
        struct ntg_dxy point);

struct ntg_dxy ntg_widget_map_to_cont_space(
        const ntg_widget* widget,
        struct ntg_dxy point);

/* -------------------------------------------------------------------------- */
/* LAYOUT HELPERS */
/* ------------------------------------------------------------------------- */

// Measure
struct ntg_object_measure ntg_widget_get_outer_hmeasure(const ntg_widget* widget);
struct ntg_object_measure ntg_widget_get_outer_vmeasure(const ntg_widget* widget);

void ntg_widget_set_cont_hmeasure(ntg_widget* widget, struct ntg_object_measure measure);
void ntg_widget_set_cont_vmeasure(ntg_widget* widget, struct ntg_object_measure measure);

// Constrain
size_t ntg_widget_get_cont_hsize(const ntg_widget* widget);
size_t ntg_widget_get_cont_vsize(const ntg_widget* widget);
void ntg_widget_set_outer_hsize(ntg_widget* widget, size_t size);
void ntg_widget_set_outer_vsize(ntg_widget* widget, size_t size);

// Arrange
void ntg_widget_set_outer_pos(ntg_widget* widget, struct ntg_xy pos);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

void ntg_widget_set_on_key_fn(
        ntg_widget* widget,
        bool (*on_key_fn)(ntg_widget* widget, struct nt_key_event key));

void ntg_widget_set_on_mouse_fn(
        ntg_widget* widget,
        bool (*on_mouse_fn)(ntg_widget* widget, struct nt_mouse_event mouse));

bool ntg_widget_on_key(ntg_widget* widget, struct nt_key_event key);
bool ntg_widget_on_mouse(ntg_widget* widget, struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_widget_init(
        ntg_widget* widget,
        struct ntg_object_layout_ops layout_ops,
        struct ntg_widget_hooks hooks,
        const ntg_type* type);
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

#endif // NTG_WIDGET_H
