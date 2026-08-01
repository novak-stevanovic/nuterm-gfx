#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include "nt_event.h"
#include "shared/ntg_shared.h"
#include "core/object/ntg_object_drawing.h"
#include "thirdparty/genc.h"
#include "core/object/ntg_object_layout.h"

#define NTG_OBJECT_MAX_CHILDREN 500
#define NTG_OBJECT_MAX_ANCHORED 200

#define NTG_OBJECT_MIN_SIZE_UNSET 0
#define NTG_OBJECT_MAX_SIZE_UNSET NTG_SIZE_MAX
#define NTG_OBJECT_GROW_UNSET NTG_SIZE_MAX
#define NTG_OBJECT_Z_INDEX_UNSET 0

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* BORDER/PADDING */
/* ------------------------------------------------------ */

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

NTG_API struct ntg_border_opts ntg_border_opts_default();

NTG_API bool
ntg_border_opts_are_eql(const struct ntg_border_opts* o1, const struct ntg_border_opts* o2);

struct ntg_padding_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
};

NTG_API struct ntg_padding_opts ntg_padding_opts_default();

NTG_API bool
ntg_padding_opts_are_eql(const struct ntg_padding_opts* o1, const struct ntg_padding_opts* o2);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

struct ntg_layout_opts
{
    struct ntg_xy min_cont_size, max_cont_size, grow;
    int z_index;
};

NTG_API struct ntg_layout_opts ntg_layout_opts_default();

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

struct ntg_object_key
{
    struct nt_key_event key;
    ntg_object* target;
};

struct ntg_object_mouse
{
    struct nt_mouse_event mouse;
    ntg_object* target;
    bool from_keybind;
};

/* ------------------------------------------------------ */
/* FOCUSABLE/CLICKABLE */
/* ------------------------------------------------------ */

enum ntg_object_focusable_mode
{
    NTG_OBJECT_UNFOCUSABLE,
    NTG_OBJECT_FOCUSABLE
};

enum ntg_object_clickable_mode
{
    NTG_OBJECT_UNCLICKABLE,
    NTG_OBJECT_CLICKABLE_CONT,
    NTG_OBJECT_CLICKABLE_BORDER
};

enum ntg_object_hit_result
{
    NTG_OBJECT_HIT_CONT,
    NTG_OBJECT_HIT_PAD,
    NTG_OBJECT_HIT_BORD
};

/* ------------------------------------------------------ */
/* HOOKS */
/* ------------------------------------------------------ */

struct ntg_object_hooks
{
    void (*on_key_fn)(ntg_object* object, const struct ntg_object_key* event);
    void (*on_mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event);
    
    void (*on_focus_fn)(ntg_object* object, ntg_object* old_focused);
    
    void (*on_unfocus_fn)(ntg_object* object, ntg_object* new_focused);

    void (*on_child_rm_fn)(ntg_object* object, ntg_object* child);
    void (*on_child_add_fn)(ntg_object* object, ntg_object* child);

    void (*on_anchored_add_fn)(ntg_object* object, ntg_object* anchored);
    void (*on_anchored_rm_fn)(ntg_object* object, ntg_object* anchored);

    void (*on_parent_set_fn)(ntg_object* object, ntg_object* new_parent);
    void (*on_parent_rm_fn)(ntg_object* object, ntg_object* old_parent);

    void (*on_scene_set_fn)(ntg_object* object, ntg_scene* new_scene);
    void (*on_scene_rm_fn)(ntg_object* object, ntg_scene* old_scene);

    void (*on_base_set_fn)(ntg_object* object, ntg_object* new_base);
    void (*on_base_rm_fn)(ntg_object* object, ntg_object* old_base);

    void (*on_border_opts_chng_fn)(
            ntg_object* object,
            const struct ntg_border_opts* old_opts,
            const struct ntg_border_opts* new_opts);

    void (*on_padding_opts_chng_fn)(
            ntg_object* object,
            const struct ntg_padding_opts* old_opts,
            const struct ntg_padding_opts* new_opts);

    void (*on_layout_opts_chng_fn)(
            ntg_object* object,
            const struct ntg_layout_opts* old_opts,
            const struct ntg_layout_opts* new_opts);

    
    void (*on_cont_resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    
    void (*on_resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);
};

/* ------------------------------------------------------ */
/* NTG_OBJECT */
/* ------------------------------------------------------ */

GENC_VECTOR_GENERATE(ntg_object_vec, ntg_object*, 1.5, NULL);

struct ntg_object
{
    const ntg_type* _type;

    struct
    {
        ntg_scene* __scene; 
        ntg_object* _parent;
        ntg_object_vec _children;
    };

    struct
    {
        ntg_object_vec _anchored;
        ntg_object* _base;
        const struct ntg_anchor_policy* _anchor_policy;
    };

    struct ntg_layout_opts _layout_opts;
    struct ntg_vcell __base_bg;

    struct
    {
        void* layout_cache;
        struct ntg_xy _min_size, _nat_size, _max_size, _grow;
        struct ntg_xy _size;
        struct ntg_xy _pos;
        ntg_object_drawing _drawing;
        bool __skip_hborder, __skip_hpadding, __repeat;
        uint8_t _dirty;
    };

    const struct ntg_object_vtable* __vtable;
    struct ntg_object_hooks hooks;

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
        ntg_object_clickable_mode _clickable;
        ntg_object_focusable_mode _focusable;
    };

    struct
    {
        struct ntg_xy __old_size, __old_cont_size;
    };
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_object_vdeinit(ntg_object* object);

/* ------------------------------------------------------ */
/* OBJECT TREE */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_get_tree_size(const ntg_object* root);

/* ------------------------------------------------------ */

NTG_API const ntg_object*
ntg_object_get_root(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API ntg_object*
ntg_object_get_root_(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API const ntg_object*
ntg_object_get_layer_root(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API ntg_object*
ntg_object_get_layer_root_(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API ntg_scene*
ntg_object_get_scene_(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API const ntg_scene*
ntg_object_get_scene(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_true_root(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_root(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_only_layer_root(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_focused(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_descendant(
        const ntg_object* object,
        const ntg_object* descendant);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_descendant_eq(
        const ntg_object* object,
        const ntg_object* descendant);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_get_children_by_z(
        const ntg_object* object,
        ntg_object** out_buff,
        size_t cap);

/* ------------------------------------------------------ */

NTG_API ntg_object*
ntg_object_hit_test(
        ntg_object* object,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos,
        ntg_object_hit_result* out_hit);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_detach(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_anchor(
        ntg_object* base,
        ntg_object* root,
        const struct ntg_anchor_policy* policy,
        int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_unanchor(ntg_object* root);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_remove_from_scene(ntg_object* object);

/* ------------------------------------------------------ */
/* CONTROL */
/* ------------------------------------------------------ */

NTG_API void
ntg_object_set_layout_opts(
        ntg_object* object,
        const struct ntg_layout_opts* opts);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_set_border_opts(
        ntg_object* object,
        const struct ntg_border_opts* opts);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_set_padding_opts(
        ntg_object* object,
        const struct ntg_padding_opts* opts);

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

NTG_API struct ntg_dxy
ntg_object_map_to_ancestor(
        const ntg_object* object,
        const ntg_object* ancestor,
        struct ntg_dxy point);

/* ------------------------------------------------------ */

NTG_API struct ntg_dxy
ntg_object_map_to_descendant(
        const ntg_object* object,
        const ntg_object* descendant,
        struct ntg_dxy point);

/* ------------------------------------------------------ */

NTG_API struct ntg_dxy
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point);

/* ------------------------------------------------------ */

NTG_API struct ntg_dxy
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_object_feed_key(ntg_object* object, const struct ntg_object_key* event);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_feed_mouse(ntg_object* object, const struct ntg_object_mouse* event);

/* ------------------------------------------------------ */
/* TRAVERSE HELPERS */
/* ------------------------------------------------------ */

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

/* -------------------------------------------------------------------------- */
/* TYPES */
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
            const ntg_object* object,
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

    void (*deinit_fn)(ntg_object* object);

    void (*rm_child_fn)(ntg_object* object, ntg_object* child);

    bool (*process_key_fn)(ntg_object* object, const struct ntg_object_key* event);
    bool (*process_mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event);

    void (*focus_fn)(ntg_object* object, ntg_object* old_focused);
    void (*unfocus_fn)(ntg_object* object, ntg_object* new_focused);

    void (*cont_resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    void (*resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_deinit(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_attach(ntg_object* parent, ntg_object* child, int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_set_base_bg(ntg_object* object, struct ntg_vcell base_bg);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_set_focusable(ntg_object* object, ntg_object_focusable_mode mode);

/* ------------------------------------------------------ */

NTG_API void
ntg_object_set_clickable(ntg_object* object, ntg_object_clickable_mode mode);

/* ------------------------------------------------------ */

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_object_root_set_scene(ntg_object* object, ntg_scene* scene);

void _ntg_object_on_scene_change(ntg_object* object, ntg_scene* scene);

void _ntg_object_focus(ntg_object* object, ntg_object* old_focused);
void _ntg_object_unfocus(ntg_object* object, ntg_object* new_focused);

#endif // NTG_OBJECT_H
