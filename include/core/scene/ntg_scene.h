#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

// TODO: clickable, focusable
// TODO: cancel focus?

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_attach_policy;

struct ntg_scene_hooks
{
    bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key);
    bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse);

    void (*on_root_chng_fn)(
            ntg_scene* scene,
            ntg_object* old_root,
            ntg_object* new_root);

    void (*on_size_chng_fn)(
            ntg_scene* scene,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    void (*on_stage_chng_fn)(
            ntg_scene* scene,
            ntg_stage* old_stage,
            ntg_stage* new_stage);

    // If object is root or layer root, this will trigger
    void (*on_object_register_fn)(ntg_scene* scene, ntg_object* object);
    void (*on_object_unregister_fn)(ntg_scene* scene, ntg_object* object);

    void (*on_layer_add_fn)(ntg_scene* scene, ntg_object* layer_root);
    void (*on_layer_rm_fn)(ntg_scene* scene, ntg_object* layer_root);
};

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

struct ntg_scene_layer_node;

struct ntg_scene
{
    ntg_stage* _stage;

    ntg_object* _root;

    struct ntg_xy _size;

    bool _dirty;

    struct ntg_scene_hooks hooks;

    ntg_focus_manager* _fm;

    void* data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes an empty scene and its focus manager.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `scene` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: focus-manager or scene resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: focus-manager initialization fails unexpectedly. */
NTG_API void
ntg_scene_init(ntg_scene* scene, int* out_status);

/* ------------------------------------------------------ */

/* Detaches the scene from its stage, releases focus-manager storage, and resets
 * the scene structure. The current implementation does not explicitly
 * unregister or detach an existing root before reset. */
NTG_API void
ntg_scene_deinit(ntg_scene* scene);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_scene_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_scene_deinit_(void* _scene);

/* ------------------------------------------------------ */

/* Marks the scene and its stage for a new layout/composition pass. A `NULL`
 * scene is ignored. */
NTG_API void
ntg_scene_mark_dirty(ntg_scene* scene);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

/* Finds the highest-z scene layer under `pos`, then recursively finds the
 * deepest object in that layer. When non-`NULL`, `out_object_pos` receives a
 * position local to the returned object.
 *
 * RETURN VALUE:
 * The hit object, or `NULL` when no object is hit or an error occurs.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `scene` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: temporary layer storage cannot be allocated. */
NTG_API ntg_object*
ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_xy* out_object_pos,
        int* out_status);

/* ------------------------------------------------------ */

/* Collects the scene root and every anchored layer root, sorted by ascending
 * `z_index`. Passing `NULL` for `out_layers` performs a count query; a limited
 * buffer receives at most `cap` entries.
 *
 * RETURN VALUE:
 * The total number of layers regardless of `cap`, or `0` for a `NULL` scene or
 * a scene without a root. */
NTG_API size_t
ntg_scene_collect_layers_by_z(
        ntg_scene* scene,
        ntg_object** out_layers,
        size_t cap);

/* ------------------------------------------------------ */

/* Replaces the scene root and updates registration hooks. A true root is
 * removed from its previous scene, while an anchored layer root is unanchored
 * before becoming the new root.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `scene` is `NULL`. */
NTG_API void
ntg_scene_set_root(ntg_scene* scene, ntg_object* root, int* out_status);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

/* Default scene key handler that forwards the event to the focus manager.
 *
 * RETURN VALUE:
 * `true` when focus handling consumes the event; otherwise `false`. */
NTG_API bool
ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key);

/* ------------------------------------------------------ */

/* Default scene mouse handler that forwards the event to the focus manager.
 *
 * RETURN VALUE:
 * `true` when focus handling consumes the event; otherwise `false`. */
NTG_API bool
ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

/* ------------------------------------------------------ */

/* Invokes the scene key hook when present.
 *
 * RETURN VALUE:
 * `true` when the hook handles the event; otherwise `false`. */
NTG_API bool
ntg_scene_feed_key(ntg_scene* scene, struct nt_key_event key);

/* ------------------------------------------------------ */

/* Invokes the scene mouse hook when present.
 *
 * RETURN VALUE:
 * `true` when the hook handles the event; otherwise `false`. */
NTG_API bool
ntg_scene_feed_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* Updates scene dimensions, marks the scene dirty, and invokes the size-change
 * hook. The root consumes the new size during a later layout pass. */
void _ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);
/* Runs all required measurement, constraint, fixup, arrangement, and draw
 * phases for the scene root and anchored layers. */
void _ntg_scene_layout(ntg_scene* scene, sarena* arena);
/* Clears the scene dirty flag after a completed composition pass. */
void _ntg_scene_clean(ntg_scene* scene);
/* Assigns the owning stage pointer when it changes. It does not update the
 * opposite stage link or invoke hooks. */
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

/* Marks every object in a tree dirty and marks the scene dirty, recursively
 * including ordinary children and anchored subtrees. It does not invoke
 * registration hooks. */
void _ntg_scene_add_object_tree(ntg_scene* scene, ntg_object* root);
/* Marks the scene dirty and invalidates focus for every object in a tree,
 * recursively including ordinary children and anchored subtrees. It does not
 * invoke unregistration hooks. */
void _ntg_scene_rm_object_tree(ntg_scene* scene, ntg_object* root);

/* Registers one root or layer-root object and invokes registration/layer hooks
 * as applicable. */
void _ntg_scene_register(ntg_scene* scene, ntg_object* root);
/* Unregisters one root or layer-root object and invokes unregistration/layer
 * hooks as applicable. */
void _ntg_scene_unregister(ntg_scene* scene, ntg_object* root);
/* Registers every object in a tree and invokes applicable object, scene-change,
 * and layer hooks, recursively including ordinary children and anchored
 * subtrees. */
void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root);
/* Unregisters every object in a tree and invokes applicable object,
 * scene-change, and layer hooks, recursively including ordinary children and
 * anchored subtrees. */
void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root);

#endif // NTG_SCENE_H
