#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object_drawing.h"
#include "thirdparty/genc.h"
#include "core/object/ntg_object_layout.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

GENC_VECTOR_GENERATE(ntg_object_vec, ntg_object*, 1.5, NULL);

/* -------------------------------------------------------------------------- */
/* BORDER & PADDING */
/* -------------------------------------------------------------------------- */

enum ntg_object_dcr_enable
{
    NTG_OBJECT_DCR_ENABLE_MIN,
    NTG_OBJECT_DCR_ENABLE_NAT,
    NTG_OBJECT_DCR_ENABLE_ALWAYS
};

struct ntg_border_style
{
    void* data;
    void (*draw_fn)(
            void* data,
            struct ntg_xy size,
            struct ntg_insets border_size,
            ntg_object_tmp_drawing* out_drawing);
    void (*free_fn)(void* data);
};

struct ntg_border_opts
{
    struct ntg_border_style style;
    struct ntg_insets pref_size;
    ntg_object_dcr_enable enable;
};

struct ntg_padding_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
};

/* -------------------------------------------------------------------------- */
/* OBJECT */
/* -------------------------------------------------------------------------- */

struct ntg_object_hooks
{
    void (*on_child_rm_fn)(ntg_object* object, ntg_object* child);
};

struct ntg_object
{
    const ntg_type* _type;

    struct
    {
        ntg_scene_layer* __scene_layer; // only root holds
        ntg_object* _parent;
        ntg_object_vec _children;
    };

    struct
    {
        struct ntg_xy _user_min_size_cont, _user_max_size_cont, _user_grow;
        struct ntg_vcell _def_bg;
        int _z_index;
    };

    struct
    {
        struct ntg_object_layout_ops __layout_ops;
        void* layout_ch;
        struct ntg_xy _min_size, _nat_size, _max_size, _grow;
        struct ntg_xy _size;
        struct ntg_xy _pos;
        ntg_object_drawing _drawing;
        uint8_t _dirty;
        uint8_t __dirty_hdcr, __dirty_vdcr, __skip_fix;
    };

    struct ntg_object_hooks __hooks;

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
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* OBJECT TREE */
/* -------------------------------------------------------------------------- */

const ntg_object* ntg_object_get_root(const ntg_object* object);
ntg_object* ntg_object_get_root_(ntg_object* object);

ntg_scene_layer* ntg_object_get_scene_layer_(ntg_object* object);
const ntg_scene_layer* ntg_object_get_scene_layer(const ntg_object* object);

ntg_scene* ntg_object_get_scene_(ntg_object* object);
const ntg_scene* ntg_object_get_scene(const ntg_object* object);

bool ntg_object_is_descendant(
        const ntg_object* object,
        const ntg_object* descendant);
bool ntg_object_is_descendant_eq(
        const ntg_object* object,
        const ntg_object* descendant);

// Returns total count of objects in tree, including root
size_t ntg_object_get_tree_size(const ntg_object* root);

size_t ntg_object_get_children_by_z(
        const ntg_object* object,
        ntg_object** out_buff,
        size_t cap);

ntg_object* ntg_object_hit_test(
        ntg_object* object,
        struct ntg_xy pos,
        struct ntg_xy* out_local_pos);

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
        struct ntg_border_opts opts);
void ntg_object_set_padding_opts(
        ntg_object* object,
        struct ntg_padding_opts opts);

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
ntg_object_map_to_scene_layer(const ntg_object* object, struct ntg_dxy point);

struct ntg_dxy 
ntg_object_map_from_scene_layer(const ntg_object* object, struct ntg_dxy point);

struct ntg_dxy 
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point);

struct ntg_dxy 
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point);

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

/* -------------------------------------------------------------------------- */
/* BORDER STYLE */
/* -------------------------------------------------------------------------- */

void ntg_object_mark_dirty(ntg_object* object, uint8_t dirty);

struct ntg_def_border_style_dt
{
    struct ntg_vcell top_left, top,
    top_right, right, bottom_right,
    bottom, bottom_left, left, padding;
};

struct ntg_border_style
ntg_def_border_style_new(struct ntg_def_border_style_dt data);

struct ntg_def_border_style_dt
ntg_def_border_style_monochrome(nt_color color);

struct ntg_def_border_style_dt
ntg_def_border_style_uniform(struct nt_gfx gfx, uint32_t codepoint);

struct ntg_def_border_style_dt
ntg_def_border_style_uniform_edge(struct nt_gfx gfx, uint32_t codepoint);

struct ntg_def_border_style_dt
ntg_def_border_style_single(struct nt_gfx gfx);

struct ntg_def_border_style_dt
ntg_def_border_style_double(struct nt_gfx gfx);

struct ntg_def_border_style_dt
ntg_def_border_style_rounded(struct nt_gfx gfx);

struct ntg_def_border_style_dt
ntg_def_border_style_heavy(struct nt_gfx gfx);

struct ntg_def_border_style_dt
ntg_def_border_style_dashed(struct nt_gfx gfx);

struct ntg_def_border_style_dt
ntg_def_border_style_ascii(struct nt_gfx gfx);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

void ntg_object_init(
        ntg_object* object,
        struct ntg_object_layout_ops layout_ops,
        struct ntg_object_hooks hooks,
        const ntg_type* type);
void ntg_object_deinit(ntg_object* object);

/* Updates only the tree.
 * Called internally by types extending ntg_object. */
void ntg_object_attach(ntg_object* parent, ntg_object* child);

/* Updates only the tree.
 * Called internally by types extending ntg_object. */
void ntg_object_detach(ntg_object* object);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_object_clean(ntg_object* object, uint8_t clean);

void _ntg_object_root_set_scene_layer(ntg_object* object, ntg_scene_layer* layer);

#endif // NTG_OBJECT_H
