#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object_drawing.h"
#include "thirdparty/genc.h"
#include "core/object/ntg_object_layout.h"
#include "core/object/ntg_border_style.h"
#include "core/object/ntg_anchor_policy.h"
#include "core/object/ntg_object_decorator.h"

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

GENC_VECTOR_GENERATE(ntg_object_vec, ntg_object*, 1.5, NULL);

struct ntg_layout_opts
{
    struct ntg_xy min_cont_size, max_cont_size, grow;
    int z_index;
};

/* Creates default layout options: unset minimum, maximum, and growth values on
 * both axes, with the default z-index. */
NTG_API struct ntg_layout_opts
ntg_layout_opts_def();

/* Compares two object layout option values. Pointer identity counts as equal;
 * otherwise a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_layout_opts_are_eql(
        const struct ntg_layout_opts* opts1,
        const struct ntg_layout_opts* opts2);

/* ------------------------------------------------------ */

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

    bool (*process_key_fn)(ntg_object* object, struct nt_key_event key);
    bool (*process_mouse_fn)(
            ntg_object* object,
            struct nt_mouse_event mouse,
            ntg_object_mouse_type type);

    void (*focus_fn)(ntg_object* object, ntg_object* old_focused);
    void (*unfocus_fn)(ntg_object* object, ntg_object* new_focused);

    /* Triggers not immeadietely after parent's hconstrain and vconstrain but
     * after constrain phase (in fixup phase). */
    void (*cont_resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    void (*resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);
};

/* ------------------------------------------------------ */

struct ntg_object_hooks
{
    // Called in ntg_object_feed_key() fn
    void (*on_key_fn)(ntg_object* object, struct nt_key_event key);
    // Called in ntg_object_feed_mouse() fn
    void (*on_mouse_fn)(
            ntg_object* object,
            struct nt_mouse_event mouse,
            ntg_object_mouse_type type);

    // Called in _ntg_object_focus() fn
    void (*on_focus_fn)(ntg_object* object, ntg_object* old_focused);
    // Called in _ntg_object_unfocus() fn
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

    /* Triggers not immeadietely after parent's hconstrain and vconstrain but
     * after constrain phase (in fixup phase). */
    void (*on_cont_resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    /* Triggers not immeadietely after parent's hconstrain and vconstrain but
     * after constrain phase (in fixup phase). */
    void (*on_resize_fn)(
            ntg_object* object,
            struct ntg_xy old_size,
            struct ntg_xy new_size);
};

/* ------------------------------------------------------ */

enum ntg_object_mouse_type
{
    NTG_OBJECT_MOUSE_TRUE,
    NTG_OBJECT_MOUSE_SCENE
};

/* ------------------------------------------------------ */

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
        bool _focusable, _clickable, _border_clickable;
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

/* Counts every object in the ordinary parent/child subtree rooted at `root`,
 * including `root` itself. Anchored layer trees are not traversed.
 *
 * RETURN VALUE:
 * The object count, or `0` when `root` is `NULL`. */
NTG_API size_t
ntg_object_get_tree_size(const ntg_object* root);

/* ------------------------------------------------------ */

/* Finds the true root by climbing to each layer root and then following
 * anchor-base links until no base remains.
 *
 * RETURN VALUE:
 * The read-only true root, or `NULL` when `object` is `NULL`. */
NTG_API const ntg_object*
ntg_object_get_root(const ntg_object* object);

/* ------------------------------------------------------ */

/* Mutable counterpart of `ntg_object_get_root`.
 *
 * RETURN VALUE:
 * The true root, or `NULL` when `object` is `NULL`. */
NTG_API ntg_object*
ntg_object_get_root_(ntg_object* object);

/* ------------------------------------------------------ */

/* Climbs only parent links to find the root of the object's current layer.
 *
 * RETURN VALUE:
 * The read-only layer root, or `NULL` when `object` is `NULL`. */
NTG_API const ntg_object*
ntg_object_get_layer_root(const ntg_object* object);

/* ------------------------------------------------------ */

/* Mutable counterpart of `ntg_object_get_layer_root`.
 *
 * RETURN VALUE:
 * The layer root, or `NULL` when `object` is `NULL`. */
NTG_API ntg_object*
ntg_object_get_layer_root_(ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the scene registered on the object's true root.
 *
 * RETURN VALUE:
 * The scene pointer, or `NULL` when the object is `NULL` or not in a scene. */
NTG_API ntg_scene*
ntg_object_get_scene_(ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the read-only scene registered on the object's true root.
 *
 * RETURN VALUE:
 * The scene pointer, or `NULL` when the object is `NULL` or not in a scene. */
NTG_API const ntg_scene*
ntg_object_get_scene(const ntg_object* object);

/* ------------------------------------------------------ */

/* Checks whether an object has neither a parent nor an anchor base. */
NTG_API bool
ntg_object_is_true_root(const ntg_object* object);

/* ------------------------------------------------------ */

/* Checks whether an object has no ordinary parent. Anchored layer roots
 * therefore count as roots. */
NTG_API bool
ntg_object_is_root(const ntg_object* object);

/* ------------------------------------------------------ */

/* Checks whether an object is an anchored layer root rather than the true
 * root. */
NTG_API bool
ntg_object_is_only_layer_root(const ntg_object* object);

/* ------------------------------------------------------ */

/* Checks whether an object has a scene, and is currently focused. */
NTG_API bool
ntg_object_is_focused(const ntg_object* object);

/* ------------------------------------------------------ */

/* Checks whether `descendant` is `object` itself or lies below it through
 * parent links. */
NTG_API bool
ntg_object_is_descendant(
        const ntg_object* object,
        const ntg_object* descendant);

/* ------------------------------------------------------ */

/* Checks whether two objects are equal or the second lies below the first
 * through parent links. This currently has the same inclusive semantics as
 * `ntg_object_is_descendant`. */
NTG_API bool
ntg_object_is_descendant_eq(
        const ntg_object* object,
        const ntg_object* descendant);

/* ------------------------------------------------------ */

/* Gets direct children sorted by ascending `z_index`. Passing `NULL` for
 * `out_buff` performs a count query; when a buffer is supplied, `cap` must hold
 * every child.
 *
 * RETURN VALUE:
 * The child count. Returns `0` for a `NULL` object, no children, or an
 * undersized output buffer. */
NTG_API size_t
ntg_object_get_children_by_z(
        const ntg_object* object,
        ntg_object** out_buff,
        size_t cap);

/* ------------------------------------------------------ */

/* Recursively finds the deepest object under `pos`, preferring the direct child
 * with the highest `z_index`. `pos` is expressed in the starting object's local
 * coordinates.
 *
 * RETURN VALUE:
 * The hit object, or `NULL` when the starting object is `NULL` or `pos` is
 * outside it. `out_local_pos` receives coordinates local to the returned
 * object. */
NTG_API ntg_object*
ntg_object_hit_test(
        ntg_object* object,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos);

/* ------------------------------------------------------ */

/* Removes an object from its ordinary parent, updates scene registration when
 * needed, invokes parent/child hooks, and marks layout dirty. A root or `NULL`
 * object is ignored. */
NTG_API void
ntg_object_detach(ntg_object* object);

/* ------------------------------------------------------ */

/* Attaches `root` to `base` as an anchored layer using `policy`. Any existing
 * parent or anchor relationship on `root` is removed first, and the tree is
 * registered in the base scene.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `base`, `root`, or `policy` is `NULL`, or `base ==
 *   root`.
 * - `NTG_ERR_MAX_ANCHORED`: `base` already has the maximum number of anchored
 *   roots.
 * - `NTG_ERR_ALLOC_FAIL`: the anchored-object vector cannot grow.
 * - `NTG_ERR_UNEXPECTED`: the vector insertion fails unexpectedly. */
NTG_API void
ntg_object_anchor(
        ntg_object* base,
        ntg_object* root,
        const struct ntg_anchor_policy* policy,
        int* out_status);

/* ------------------------------------------------------ */

/* Removes an anchored root from its base, updates scene bookkeeping, and
 * invokes anchor and base-change hooks. A `NULL` or non-anchored object is
 * ignored. */
NTG_API void
ntg_object_unanchor(ntg_object* root);

/* ------------------------------------------------------ */

/* Removes an object by detaching it from a parent, unanchoring its layer, or
 * clearing it as the scene root, whichever relationship applies. */
NTG_API void
ntg_object_remove_from_scene(ntg_object* object);

/* ------------------------------------------------------ */
/* CONTROL */
/* ------------------------------------------------------ */

/* Replaces layout options, using defaults for `NULL`. A changed value marks the
 * complete object pipeline dirty and invokes the layout-options hook; a `NULL`
 * object is ignored. */
NTG_API void
ntg_object_set_layout_opts(
        ntg_object* object,
        const struct ntg_layout_opts* opts);

/* ------------------------------------------------------ */

/* Replaces border options, using defaults for `NULL`; a `NULL` style is
 * replaced with the default style. A changed value marks the complete object
 * pipeline dirty and invokes the border-options hook; a `NULL` object is
 * ignored. */
NTG_API void
ntg_object_set_border_opts(
        ntg_object* object,
        const struct ntg_border_opts* opts);

/* ------------------------------------------------------ */

/* Replaces padding options, using defaults for `NULL`. A changed value marks
 * the complete object pipeline dirty and invokes the padding-options hook; a
 * `NULL` object is ignored. */
NTG_API void
ntg_object_set_padding_opts(
        ntg_object* object,
        const struct ntg_padding_opts* opts);

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

/* Maps a point from `object` coordinates toward `ancestor` coordinates by
 * adding each visited object position while climbing parent links. Passing
 * `NULL` as `ancestor` maps to the top of the current parent chain.
 *
 * RETURN VALUE:
 * `NTG_DXY_MAX` when `object` is `NULL`; otherwise the accumulated point. A
 * non-`NULL` ancestor is not validated, so accumulation stops at the layer root
 * if it is never reached. */
NTG_API struct ntg_dxy
ntg_object_map_to_ancestor(
        const ntg_object* object,
        const ntg_object* ancestor,
        struct ntg_dxy point);

/* ------------------------------------------------------ */

/* Maps a point into `descendant` coordinates by subtracting the position
 * accumulated while walking from `descendant` toward `object`.
 *
 * RETURN VALUE:
 * `NTG_DXY_MAX` when `descendant` is `NULL`; otherwise the computed point. The
 * function does not validate that `object` is an ancestor. */
NTG_API struct ntg_dxy
ntg_object_map_to_descendant(
        const ntg_object* object,
        const ntg_object* descendant,
        struct ntg_dxy point);

/* ------------------------------------------------------ */

/* Maps a point from object-local coordinates to the top of its ordinary parent
 * chain. Layer-root positions are treated as scene-space positions.
 *
 * RETURN VALUE:
 * The mapped signed coordinate, or `NTG_DXY_MAX` when `object` is `NULL`. */
NTG_API struct ntg_dxy
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point);

/* ------------------------------------------------------ */

/* Maps a scene-space point into object-local coordinates by subtracting
 * positions along the object parent chain.
 *
 * RETURN VALUE:
 * The mapped signed coordinate, or `NTG_DXY_MAX` when `object` is `NULL`. */
NTG_API struct ntg_dxy
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

/* Passes a key event to the object's key hook when present.
 *
 * RETURN VALUE:
 * `true` when the hook reports the event handled; otherwise `false`. */
NTG_API bool
ntg_object_feed_key(ntg_object* object, struct nt_key_event key);

/* ------------------------------------------------------ */

/* Passes a mouse event to the object's mouse hook when present.
 *
 * RETURN VALUE:
 * `true` when the hook reports the event handled; otherwise `false`. */
NTG_API bool
ntg_object_feed_mouse(
        ntg_object* object,
        struct nt_mouse_event mouse,
        ntg_object_mouse_type type);

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

/* Initializes the base object, copies its virtual table and type descriptor,
 * and creates empty child, anchor, decorator, and drawing state.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `object` or `type` is `NULL`.
 * - `NTG_ERR_INVALID_TYPE`: `type` is not `NTG_TYPE_OBJECT` or derived from it.
 * - `NTG_ERR_BAD_VTABLE`: `vtable` is `NULL` or `deinit_fn` is NULL.
 * - `NTG_ERR_ALLOC_FAIL`: an object-owned vector cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: vector initialization fails for another reason. */
NTG_API void
ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

/* ------------------------------------------------------ */

/* Removes the object from its scene, parent, or base; detaches all ordinary
 * children; releases its child vectors and drawing; then resets the base
 * object. Anchored objects stored by this object and derived `layout_cache`
 * memory are not deinitialized or freed here. */
NTG_API void
ntg_object_deinit(ntg_object* object);

/* ------------------------------------------------------ */

/* Attaches `child` to `parent`, first detaching the child from any parent,
 * scene-root, or anchor relationship. It updates scene registration and invokes
 * tree hooks; intended for derived container types.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `parent` or `child` is `NULL`, or `parent == child`.
 * - `NTG_ERR_MAX_CHILDREN`: `parent` already has the maximum number of
 *   children.
 * - `NTG_ERR_ALLOC_FAIL`: the child vector cannot grow.
 * - `NTG_ERR_UNEXPECTED`: the vector insertion fails unexpectedly. */
NTG_API void
ntg_object_attach(ntg_object* parent, ntg_object* child, int* out_status);

/* ------------------------------------------------------ */

/* Assigns the base virtual cell unconditionally and marks drawing and rendering
 * dirty. A `NULL` object is ignored. */
NTG_API void
ntg_object_set_base_bg(ntg_object* object, struct ntg_vcell base_bg);

NTG_API void
ntg_object_set_focusable(ntg_object* object, bool focusable);

NTG_API void
ntg_object_set_clickable(ntg_object* object, bool clickable, bool border_clickable);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* Sets the scene pointer stored by a true root. This low-level helper does not
 * register or unregister the tree. */
void _ntg_object_root_set_scene(ntg_object* object, ntg_scene* scene);

/* Resets the object’s temporary horizontal-decorator skip flags and
 * layout-repeat flag. The supplied scene pointer is currently unused. */
void _ntg_object_on_scene_change(ntg_object* object, ntg_scene* scene);

void _ntg_object_focus(ntg_object* object, ntg_object* old_focused);
void _ntg_object_unfocus(ntg_object* object, ntg_object* new_focused);

#endif // NTG_OBJECT_H
