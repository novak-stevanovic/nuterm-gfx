#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object_drawing.h"
#include "thirdparty/genc.h"
#include "core/object/ntg_object_layout.h"
#include "core/object/ntg_border_style.h"
#include "core/object/ntg_anchor_policy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

GENC_VECTOR_GENERATE(ntg_object_vec, ntg_object*, 1.5, NULL);

/* -------------------------------------------------------------------------- */
/* VTABLE */
/* -------------------------------------------------------------------------- */

struct ntg_object_vtable
{
    struct ntg_object_measure (*measure_fn)(
            const ntg_object* object,
            ntg_orient orient,
            void* layout_ch,
            sarena* arena);

    void (*constrain_fn)(
            const ntg_object* object,
            ntg_orient orient,
            ntg_object_size_map* out_size_map,
            void* layout_ch,
            sarena* arena);

    bool (*fixup_fn)(
            const ntg_object*,
            void* layout_ch,
            sarena* arena);

    void (*arrange_fn)(
            const ntg_object* object,
            ntg_object_pos_map* out_pos_map,
            void* layout_ch,
            sarena* arena);

    void (*draw_fn)(
            const ntg_object* object,
            ntg_object_tmp_drawing* out_drawing,
            void* layout_ch,
            sarena* arena);

    void (*on_child_rm_fn)(ntg_object* object, ntg_object* child);
};

/* -------------------------------------------------------------------------- */
/* BORDER & PADDING */
/* -------------------------------------------------------------------------- */

enum ntg_object_dcr_enable
{
    NTG_OBJECT_DCR_ENABLE_MIN = 0,
    NTG_OBJECT_DCR_ENABLE_NAT,
    NTG_OBJECT_DCR_ENABLE_ALWAYS
};

struct ntg_border_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
    const struct ntg_border_style* style;
};

struct ntg_border_opts ntg_border_opts_def();

struct ntg_padding_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
};

struct ntg_padding_opts ntg_padding_opts_def();

/* -------------------------------------------------------------------------- */
/* OBJECT */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    const ntg_type* _type;

    struct
    {
        ntg_scene* __scene; // only root holds
        ntg_object* _parent;
        ntg_object_vec _children;
    };

    struct
    {
        ntg_object_vec _anchored;
        ntg_object* _base;
        const struct ntg_anchor_policy* _anchor_policy;
        // z-index is taken into consideration only if the object is root
        int _z_index;
    };

    struct
    {
        struct ntg_xy _user_min_size_cont, _user_max_size_cont, _user_grow;
        struct ntg_vcell _def_bg;
    };

    struct
    {
        struct ntg_object_vtable __vtable;
        void* layout_cache;
        struct ntg_xy _min_size, _nat_size, _max_size, _grow;
        struct ntg_xy _size;
        struct ntg_xy _pos;
        ntg_object_drawing _drawing;
        bool __skip_hborder, __skip_hpadding, __repeat;
        uint8_t _dirty;
    };

    struct
    {
        struct
        {
            struct ntg_border_opts opts;
            struct ntg_insets size;
        } _border;

        struct
        {
            struct ntg_padding_opts opts;
            struct ntg_insets size;
        } _padding;
    };

    struct
    {
        bool (*__on_key_fn)(ntg_object* object, struct nt_key_event key);
        bool (*__on_mouse_fn)(ntg_object* object, struct nt_mouse_event mouse);
    };
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* OBJECT TREE */
/* -------------------------------------------------------------------------- */

// Returns total count of objects in tree, including root
size_t ntg_object_get_tree_size(const ntg_object* root);

const ntg_object* ntg_object_get_root(const ntg_object* object);
ntg_object* ntg_object_get_root_(ntg_object* object);

const ntg_object* ntg_object_get_layer_root(const ntg_object* object);
ntg_object* ntg_object_get_layer_root_(ntg_object* object);

ntg_scene* ntg_object_get_scene_(ntg_object* object);
const ntg_scene* ntg_object_get_scene(const ntg_object* object);

ntg_scene* ntg_object_get_scene_(ntg_object* object);
const ntg_scene* ntg_object_get_scene(const ntg_object* object);

/* -------------------------------------------------------------------------- */

bool ntg_object_is_descendant(
        const ntg_object* object,
        const ntg_object* descendant);
bool ntg_object_is_descendant_eq(
        const ntg_object* object,
        const ntg_object* descendant);

/* -------------------------------------------------------------------------- */

size_t ntg_object_get_children_by_z(
        const ntg_object* object,
        ntg_object** out_buff,
        size_t cap);

ntg_object* ntg_object_hit_test(
        ntg_object* object,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos);

/* -------------------------------------------------------------------------- */

/* Updates only the tree.
 * Called internally by types extending ntg_object. */
void ntg_object_detach(ntg_object* object);

void ntg_object_anchor(
        ntg_object* base,
        ntg_object* root,
        const struct ntg_anchor_policy* policy);

void ntg_object_unanchor(ntg_object* root);

// If has parent, detach. If anchored, unanchor. If root, set scene root to NULL.
void ntg_object_remove_from_scene(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

#define NTG_OBJECT_MIN_SIZE_UNSET 0
#define NTG_OBJECT_MAX_SIZE_UNSET NTG_SIZE_MAX
#define NTG_OBJECT_GROW_UNSET NTG_SIZE_MAX

void ntg_object_set_user_min_size_cont(ntg_object* object, struct ntg_xy size);
void ntg_object_set_user_max_size_cont(ntg_object* object, struct ntg_xy size);
void ntg_object_set_user_grow(ntg_object* object, struct ntg_xy grow);
void ntg_object_set_z_index(ntg_object* object, int z_index);
void ntg_object_set_def_bg(ntg_object* object, struct ntg_vcell def_bg);

void ntg_object_set_border_opts(
        ntg_object* object,
        const struct ntg_border_opts* opts);
void ntg_object_set_padding_opts(
        ntg_object* object,
        const struct ntg_padding_opts* opts);

/* -------------------------------------------------------------------------- */
/* SPACE MAPPING */
/* -------------------------------------------------------------------------- */

struct ntg_dxy ntg_object_map_to_ancestor(
        const ntg_object* object,
        const ntg_object* ancestor,
        struct ntg_dxy point);

struct ntg_dxy ntg_object_map_to_descendant(
        const ntg_object* object,
        const ntg_object* descendant,
        struct ntg_dxy point);

struct ntg_dxy 
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point);

struct ntg_dxy 
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

void ntg_object_set_on_key_fn(ntg_object* object,
        bool (*on_key_fn)(ntg_object* object, struct nt_key_event key));
bool ntg_object_on_key(ntg_object* object, struct nt_key_event key);

void ntg_object_set_on_mouse_fn(ntg_object* object,
        bool (*on_mouse_fn)(ntg_object* object, struct nt_mouse_event mouse));
bool ntg_object_on_mouse(ntg_object* object, struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */
/* TRAVERSE HELPERS */
/* -------------------------------------------------------------------------- */

#define NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(fn_name, perform_fn)               \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    perform_fn(object, data);                                                  \
    const ntg_object_vec* children = &object->_children;                       \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
}                                                                              \

#define NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(fn_name, perform_fn)              \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    const ntg_object_vec* children = &object->_children;                       \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    perform_fn(object, data);                                                  \
}                                                                              \

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

void ntg_object_init(
        ntg_object* object,
        const struct ntg_object_vtable* layout_ops,
        const ntg_type* type);
void ntg_object_deinit(ntg_object* object);

/* Updates only the tree.
 * Called internally by types extending ntg_object. */
void ntg_object_attach(ntg_object* parent, ntg_object* child);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

// Called by scene scene. Updates the root's field only
void _ntg_object_root_set_scene(ntg_object* object, ntg_scene* scene);

// Called by scene scene when registering/unregistering objects from the scene.
void _ntg_object_on_scene_change(ntg_object* object, ntg_scene* scene);

#endif // NTG_OBJECT_H
