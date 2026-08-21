#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include "nt_event.h"
#include "shared/ntg_shared.h"
#include "base/ntg_event.h"
#include "core/object/ntg_object_draw.h"
#include "core/object/ntg_objptr_vec.h"
#include "thirdparty/genc.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#define NTG_OBJECT_MAX_CHILDREN 1000
#define NTG_OBJECT_MAX_ANCHORED 100

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* BORDER/PADDING */
/* ------------------------------------------------------ */

enum ntg_object_dcr_enable
{
    NTG_OBJECT_DCR_ENABLE_MIN = 0,
    NTG_OBJECT_DCR_ENABLE_NAT,
    NTG_OBJECT_DCR_ENABLE_ALWAYS
};

struct ntg_bdr_opts
{
    enum ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
    const struct ntg_border_style* style;
};

NTG_API struct ntg_bdr_opts
ntg_bdr_opts_default(void);

NTG_API bool
ntg_bdr_opts_are_eql(const struct ntg_bdr_opts* o1, const struct ntg_bdr_opts* o2);

struct ntg_pad_opts
{
    enum ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
};

NTG_API struct ntg_pad_opts ntg_padding_opts_default(void);

NTG_API bool
ntg_pad_opts_are_eql(const struct ntg_pad_opts* o1, const struct ntg_pad_opts* o2);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

#define NTG_OBJECT_MIN_SIZE_UNSET 0
#define NTG_OBJECT_MAX_SIZE_UNSET NTG_SIZE_MAX
#define NTG_OBJECT_GROW_UNSET NTG_SIZE_MAX
#define NTG_OBJECT_Z_INDEX_UNSET 0

struct ntg_lay_opts
{
    struct ntg_xy min_cont_size, max_cont_size, grow;
    int z_index;
};

NTG_API struct ntg_lay_opts
ntg_lay_opts_default(void);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

struct ntg_object_key
{
    struct nt_key key;
    ntg_object* target;
};

struct ntg_object_mouse
{
    struct nt_mouse mouse;
    ntg_object* target;
    bool from_keybind;
};

/* ------------------------------------------------------ */
/* FOCUSABLE/CLICKABLE */
/* ------------------------------------------------------ */

enum ntg_object_focus_mode
{
    NTG_OBJECT_UNFOCUSABLE,
    NTG_OBJECT_FOCUSABLE
};

enum ntg_object_click_mode
{
    NTG_OBJECT_UNCLICKABLE,
    NTG_OBJECT_CLICKABLE_CONT,
    NTG_OBJECT_CLICKABLE_BDR
};

/* ------------------------------------------------------ */
/* HIT TEST */
/* ------------------------------------------------------ */

enum ntg_object_part
{
    NTG_OBJECT_HIT_CONT,
    NTG_OBJECT_HIT_PAD,
    NTG_OBJECT_HIT_BDR
};

/* ------------------------------------------------------ */
/* NTG_OBJECT */
/* ------------------------------------------------------ */

struct ntg_object_hit_res
{
    ntg_object* object;
    struct ntg_xy local_pos;
    enum ntg_object_part part;
};

struct ntg_object
{
    struct
    {
        void* data;
        struct ntg_object_layout_dt* layout_dt;
    } pub;

    struct
    {
        const ntg_type* type;

        ntg_object* parent;
        struct ntg_objptr_vec children;

        struct ntg_objptr_vec anchored;
        ntg_object* base;
        const struct ntg_anchor_policy* anchor_policy;

        struct ntg_lay_opts layout_opts;

        struct ntg_xy min_size, nat_size, max_size, grow;
        struct ntg_xy size;
        struct ntg_xy pos;
        ntg_object_draw drawing;
        uint8_t dirty;

        struct ntg_bdr_opts border_opts;
        struct ntg_insets border_size;

        struct ntg_pad_opts padding_opts;
        struct ntg_insets padding_size;

        enum ntg_object_click_mode clickable;
        enum ntg_object_focus_mode focusable;

        ntg_event_delegate event_dlgt;
    } ro;

    struct
    {
        ntg_scene* scene;
        struct ntg_vcell base_bg;

        bool skip_hborder, skip_hpadding;
        bool special_repeat;
        uint8_t repeat;

        struct ntg_xy old_pos, old_size, old_cont_size;

        const struct ntg_object_vtable* vtable;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

void ntg_object_vdeinit(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_feed_key(ntg_object* object, const struct ntg_object_key* event);

NTG_API bool
ntg_object_feed_mouse(ntg_object* object, const struct ntg_object_mouse* event);

/* ------------------------------------------------------ */

NTG_API int
ntg_object_set_lay_opts(ntg_object* object, const struct ntg_lay_opts* opts_cp);

NTG_API int
ntg_object_set_bdr_opts(ntg_object* object, const struct ntg_bdr_opts* opts_cp);

NTG_API int
ntg_object_set_pad_opts(ntg_object* object, const struct ntg_pad_opts* opts_cp);

NTG_API int
ntg_object_set_anchor_policy(ntg_object* object, const ntg_anchor_policy* policy);

/* ------------------------------------------------------ */

NTG_API struct ntg_object_hit_res
ntg_object_hit_test(ntg_object* object, struct ntg_xy pos);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_focused(const ntg_object* object);

/* ------------------------------------------------------ */
/* OBJECT GRAPH OPERATIONS */
/* ------------------------------------------------------ */

NTG_API int
ntg_object_detach(ntg_object* object);

NTG_API int
ntg_object_anchor(ntg_object* base, ntg_object* root);

NTG_API int
ntg_object_unanchor(ntg_object* root);

NTG_API int
ntg_object_remove_from_scene(ntg_object* object);

/* ------------------------------------------------------ */
/* OBJECT GRAPH QUERY */
/* ------------------------------------------------------ */

NTG_API const ntg_object*
ntg_object_get_graph_root(const ntg_object* object);

NTG_API ntg_object*
ntg_object_get_graph_root_(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API const ntg_object*
ntg_object_get_tree_root(const ntg_object* object);

NTG_API ntg_object*
ntg_object_get_tree_root_(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API ntg_scene*
ntg_object_get_scene_(ntg_object* object);

NTG_API const ntg_scene*
ntg_object_get_scene(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_true_root(const ntg_object* object);

NTG_API bool
ntg_object_is_graph_root(const ntg_object* object);

NTG_API bool
ntg_object_is_tree_root(const ntg_object* object);

/* ------------------------------------------------------ */

NTG_API bool
ntg_object_is_in_tree(const ntg_object* tree_root, const ntg_object* desc);

NTG_API bool
ntg_object_is_in_graph(const ntg_object* graph_root, const ntg_object* desc);

/* ------------------------------------------------------ */
/* SIZE & POS */
/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_get_size(const ntg_object* object);

NTG_API struct ntg_xy
ntg_object_get_size_cont(const ntg_object* object);

NTG_API struct ntg_xy
ntg_object_get_size_pad(const ntg_object* object);

struct ntg_xy ntg_object_get_abs_pos(const ntg_object* object);

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

// NTG_API struct ntg_xy
// ntg_object_get_abs_pos(const ntg_object* object);
// 
// /* ------------------------------------------------------ */
// 
// NTG_API struct ntg_dxy
// ntg_object_map_to_ancs_tree(
//         const ntg_object* object,
//         const ntg_object* ancestor,
//         struct ntg_dxy point);
// 
// NTG_API struct ntg_dxy
// ntg_object_map_to_desc_tree(
//         const ntg_object* object,
//         const ntg_object* descendant,
//         struct ntg_dxy point);
// 
// /* ------------------------------------------------------ */
// 
// NTG_API struct ntg_dxy
// ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point);
// 
// NTG_API struct ntg_dxy
// ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point);

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

/* ------------------------------------------------------ */
/* COLLECT */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_object_tree_collect_pre(ntg_object* root, ntg_object** out, size_t cap);

NTG_API size_t
ntg_object_tree_collect_post(ntg_object* root, ntg_object** out, size_t cap);

NTG_API size_t
ntg_object_graph_collect_pre(ntg_object* root, ntg_object** out, size_t cap);

NTG_API size_t
ntg_object_graph_collect_post(ntg_object* root, ntg_object** out, size_t cap);

NTG_API size_t
ntg_object_graph_collect_roots_pre(ntg_object* root, ntg_object** out, size_t cap);

NTG_API size_t
ntg_object_graph_collect_roots_post(ntg_object* root, ntg_object** out, size_t cap);

/* ------------------------------------------------------ */
/* CONVENIENCE */
/* ------------------------------------------------------ */

#define NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(fn_name, perform_fn)                  \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    perform_fn(object, data);                                                  \
    const struct ntg_objptr_vec* children = &object->ro.children;              \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
}                                                                              \

#define NTG_OBJECT_TREE_DEF_TRAVERSE_POST(fn_name, perform_fn)                 \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    const struct ntg_objptr_vec* children = &object->ro.children;              \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    perform_fn(object, data);                                                  \
}

#define NTG_OBJECT_GRAPH_DEF_TRAVERSE_PRE(fn_name, perform_fn)                 \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    perform_fn(object, data);                                                  \
    const struct ntg_objptr_vec* children = &object->ro.children;              \
    const struct ntg_objptr_vec* anchored = &object->ro.anchored;              \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    for(i = 0; i < anchored->size; i++)                                        \
    {                                                                          \
        fn_name(anchored->data[i], data);                                      \
    }                                                                          \
}

#define NTG_OBJECT_GRAPH_DEF_TRAVERSE_POST(fn_name, perform_fn)                \
static void fn_name(ntg_object* object, void* data)                            \
{                                                                              \
    if(object == NULL) return;                                                 \
    const struct ntg_objptr_vec* children = &object->ro.children;              \
    const struct ntg_objptr_vec* anchored = &object->ro.anchored;              \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    for(i = 0; i < anchored->size; i++)                                        \
    {                                                                          \
        fn_name(anchored->data[i], data);                                      \
    }                                                                          \
    perform_fn(object, data);                                                  \
}

NTG_API int
ntg_object_sort_by_z(ntg_object** objects, size_t size);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_object_vtable
{
    int (*layout_prepare_fn)(
            ntg_object* object,
            struct ntg_object_layout_dt* layout_dt,
            sarena* arena);

    int (*measure_fn)(
            const ntg_object* object,
            struct ntg_object_layout_dt* layout_dt,
            enum ntg_orient orient,
            sarena* arena,
            uint32_t* relayout,
            struct ntg_object_measure* out_measure);

    int (*constrain_fn)(
            const ntg_object* object,
            struct ntg_object_layout_dt* layout_dt,
            enum ntg_orient orient,
            ntg_object_size_map* out_size_map,
            sarena* arena,
            uint32_t* relayout);

    int (*arrange_fn)(
            const ntg_object* object,
            struct ntg_object_layout_dt* layout_dt,
            ntg_object_pos_map* out_pos_map,
            sarena* arena,
            uint32_t* relayout);

    int (*draw_fn)(
            const ntg_object* object,
            struct ntg_object_layout_dt* layout_dt,
            ntg_object_tmp_draw* out_drawing,
            sarena* arena);

    void (*deinit_fn)(ntg_object* object);

    void (*cont_resize_fn)(ntg_object* object, sarena* arena);
    void (*resize_fn)(ntg_object* object, sarena* arena);
    void (*pos_chng_fn)(ntg_object* object, sarena* arena);

    void (*rm_child_fn)(ntg_object* object, ntg_object* child);

    void (*add_anchored_fn)(ntg_object* object, ntg_object* anchored);
    void (*rm_anchored_fn)(ntg_object* object, ntg_object* anchored);

    void (*set_base_fn)(ntg_object* object, ntg_object* anchored);
    void (*rm_base_fn)(ntg_object* object, ntg_object* anchored);

    void (*set_parent_fn)(ntg_object* object, ntg_object* new_parent);
    void (*rm_parent_fn)(ntg_object* object, ntg_object* old_parent);

    void (*enter_scene_fn)(ntg_object* object, ntg_scene* new_scene);
    void (*rm_scene_fn)(ntg_object* object, ntg_scene* old_scene);

    bool (*handle_key_fn)(ntg_object* object, const struct ntg_object_key* event);
    bool (*handle_mouse_fn)(ntg_object* object, const struct ntg_object_mouse* event);

    void (*focus_fn)(ntg_object* object);
    void (*unfocus_fn)(ntg_object* object);

    void (*chng_bdr_opts_fn)(
            ntg_object* object,
            const struct ntg_bdr_opts* old_opts,
            const struct ntg_bdr_opts* new_opts);

    void (*chng_pad_opts_fn)(
            ntg_object* object,
            const struct ntg_pad_opts* old_opts,
            const struct ntg_pad_opts* new_opts);

    void (*chng_lay_opts_fn)(
            ntg_object* object,
            const struct ntg_lay_opts* old_opts,
            const struct ntg_lay_opts* new_opts);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt);

NTG_API int
ntg_object_deinit(ntg_object* object);

/* ------------------------------------------------------ */

NTG_API int
ntg_object_attach(ntg_object* parent, ntg_object* child);

/* ------------------------------------------------------ */

NTG_API int
ntg_object_set_base_bg(ntg_object* object, struct ntg_vcell base_bg);

NTG_API int
ntg_object_set_focusable(ntg_object* object, enum ntg_object_focus_mode mode);

NTG_API int
ntg_object_set_clickable(ntg_object* object, enum ntg_object_click_mode mode);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void ntg__object_root_set_scene(ntg_object* object, ntg_scene* scene);

/* Called by scene when adding object tree in ntg__scene_add_object_tree() */
void ntg__object_scene_enter(ntg_object* object, ntg_scene* scene);

/* Called by scene when removing object tree in ntg__scene_rm_object_tree() */
void ntg__object_scene_leave(ntg_object* object, ntg_scene* scene);

void ntg__object_focus(ntg_object* object);
void ntg__object_unfocus(ntg_object* object);

#endif // NTG_OBJECT_H
