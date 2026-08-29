#ifndef NTG_WIDGET_H
#define NTG_WIDGET_H

#include "nt_event.h"
#include "shared/ntg_shared.h"
#include "base/object/ntg_object.h"
#include "core/widget/ntg_widget_draw.h"
#include "core/widget/ntg_widget_vec.h"
#include "thirdparty/genc.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#define NTG_WIDGET_MAX_CHILDREN 1000
#define NTG_WIDGET_MAX_ANCHORED 100
#define NTG_WIDGET_POS_NODRAW NTG_XY_MAX

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* BORDER/PADDING */
/* ------------------------------------------------------ */

enum ntg_widget_dcr_enable
{
    NTG_WIDGET_DCR_ENABLE_MIN = 0,
    NTG_WIDGET_DCR_ENABLE_NAT,
    NTG_WIDGET_DCR_ENABLE_ALWAYS
};

struct ntg_bdr_opts
{
    enum ntg_widget_dcr_enable enable;
    ntg_insets pref_size;
    const ntg_border_style* style;
};

static const struct ntg_bdr_opts NTG_BDR_OPTS_ZERO = {0};

struct ntg_pad_opts
{
    enum ntg_widget_dcr_enable enable;
    ntg_insets pref_size;
};

static const struct ntg_pad_opts NTG_PAD_OPTS_ZERO = {0};

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

#define NTG_WIDGET_MINSZ_UNSET NTG_XY_ZERO
#define NTG_WIDGET_MAXSZ_UNSET NTG_XY_MAX
#define NTG_WIDGET_GROW_UNSET ntg_xy_new(1, 1)
#define NTG_WIDGET_ZIDX_UNSET 0

/* NOT ZERO-INITIALIZABLE */
struct ntg_lay_conf
{
    struct ntg_xy cont_min_size, cont_max_size, cont_grow;
};

NTG_API void
ntg_lay_conf_init(struct ntg_lay_conf* conf);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

struct ntg_widget_key
{
    nt_key key;
    ntg_widget* target;
};

struct ntg_widget_mouse
{
    nt_mouse mouse;
    ntg_widget* target;
    bool from_keybind;
};

/* ------------------------------------------------------ */
/* FOCUSABLE/CLICKABLE */
/* ------------------------------------------------------ */

enum ntg_widget_fcs_mode
{
    NTG_WIDGET_UNFCSABLE = 0,
    NTG_WIDGET_FCSABLE
};

enum ntg_widget_clk_mode
{
    NTG_WIDGET_UNCLKABLE = 0,
    NTG_WIDGET_CLKABLE_CONT,
    NTG_WIDGET_CLKABLE_BDR
};

/* ------------------------------------------------------ */
/* HIT TEST */
/* ------------------------------------------------------ */

enum ntg_widget_part
{
    NTG_WIDGET_HIT_CONT = 0,
    NTG_WIDGET_HIT_PAD,
    NTG_WIDGET_HIT_BDR
};

/* ------------------------------------------------------ */
/* NTG_WIDGET */
/* ------------------------------------------------------ */

struct ntg_widget_hit_res
{
    ntg_widget* widget;
    ntg_xy local_pos;
    enum ntg_widget_part part;
};

struct ntg_widget
{
    ntg_object _base;

    struct
    {
        void* data;
    } pub;

    struct
    {
        ntg_widget* parent;
        struct ntg_widget_vec children;

        struct ntg_widget_vec anchored;
        ntg_widget* base;
        const struct ntg_anchor_policy* anchor_policy;

        ntg_xy min_size, nat_size, max_size, grow;
        ntg_xy size;
        ntg_insets border_size;
        ntg_insets padding_size;
        ntg_xy pos;
        ntg_widget_draw drawing;
        uint32_t dirty;

        struct ntg_lay_conf lay_conf;
        int z_index;

        struct ntg_bdr_opts bdr_opts;
        struct ntg_pad_opts pad_opts;

        enum ntg_widget_clk_mode clickable;
        enum ntg_widget_fcs_mode focusable;

    } ro;

    struct
    {
        struct ntg_widget_layout_dt* layout_dt;

        ntg_scene* scene;
        struct ntg_vcell base_bg;

        bool skip_hborder, skip_hpadding;
        bool special_repeat;
        uint8_t repeat;

        ntg_xy old_pos, old_size, old_cont_size;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API bool
ntg_widget_feed_key(ntg_widget* widget, const struct ntg_widget_key* event);

NTG_API bool
ntg_widget_feed_mouse(ntg_widget* widget, const struct ntg_widget_mouse* event);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

NTG_API int
ntg_widget_set_lay_conf(ntg_widget* widget, const struct ntg_lay_conf* conf);

NTG_API int
ntg_widget_set_z_index(ntg_widget* widget, int z_index);

NTG_API int
ntg_widget_set_bdr_opts(ntg_widget* widget, const struct ntg_bdr_opts* opts);

NTG_API int
ntg_widget_set_pad_opts(ntg_widget* widget, const struct ntg_pad_opts* opts);

NTG_API int
ntg_widget_set_anchor_policy(ntg_widget* widget, const ntg_anchor_policy* policy);

/* ------------------------------------------------------ */

NTG_API struct ntg_widget_hit_res
ntg_widget_hit_test(ntg_widget* widget, ntg_xy pos);

/* ------------------------------------------------------ */

NTG_API bool
ntg_widget_is_focused(const ntg_widget* widget);

/* ------------------------------------------------------ */
/* WIDGET GRAPH OPERATIONS */
/* ------------------------------------------------------ */

NTG_API int
ntg_widget_detach(ntg_widget* widget);

NTG_API int
ntg_widget_anchor(ntg_widget* base, ntg_widget* root);

NTG_API int
ntg_widget_unanchor(ntg_widget* root);

NTG_API int
ntg_widget_remove_from_scene(ntg_widget* widget);

/* ------------------------------------------------------ */
/* WIDGET GRAPH QUERY */
/* ------------------------------------------------------ */

NTG_API const ntg_widget*
ntg_widget_get_graph_root(const ntg_widget* widget);

NTG_API ntg_widget*
ntg_widget_get_graph_root_(ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API const ntg_widget*
ntg_widget_get_tree_root(const ntg_widget* widget);

NTG_API ntg_widget*
ntg_widget_get_tree_root_(ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API ntg_scene*
ntg_widget_get_scene_(ntg_widget* widget);

NTG_API const ntg_scene*
ntg_widget_get_scene(const ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API bool
ntg_widget_is_true_root(const ntg_widget* widget);

NTG_API bool
ntg_widget_is_graph_root(const ntg_widget* widget);

NTG_API bool
ntg_widget_is_tree_root(const ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API bool
ntg_widget_is_in_tree(const ntg_widget* tree_root, const ntg_widget* desc);

NTG_API bool
ntg_widget_is_in_graph(const ntg_widget* graph_root, const ntg_widget* desc);

/* ------------------------------------------------------ */
/* SIZE & POS */
/* ------------------------------------------------------ */

NTG_API ntg_xy
ntg_widget_get_size(const ntg_widget* widget);

NTG_API ntg_xy
ntg_widget_get_size_cont(const ntg_widget* widget);

NTG_API ntg_xy
ntg_widget_get_size_pad(const ntg_widget* widget);

NTG_API ntg_xy
ntg_widget_get_abs_pos(const ntg_widget* widget);

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

NTG_API ntg_dxy
ntg_widget_map_to_ancs(
        const ntg_widget* widget,
        const ntg_widget* ancs,
        ntg_dxy point);

NTG_API ntg_dxy
ntg_widget_map_to_desc(
        const ntg_widget* widget,
        const ntg_widget* desc,
        ntg_dxy point);

NTG_API ntg_dxy
ntg_widget_map_to_scene(const ntg_widget* widget, ntg_dxy point);

NTG_API ntg_dxy
ntg_widget_map_from_scene(const ntg_widget* widget, ntg_dxy point);

/* ------------------------------------------------------ */
/* COLLECT */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_widget_tree_collect_pre(ntg_widget* root, ntg_widget** out, size_t cap);

NTG_API size_t
ntg_widget_tree_collect_post(ntg_widget* root, ntg_widget** out, size_t cap);

NTG_API size_t
ntg_widget_graph_collect_pre(ntg_widget* root, ntg_widget** out, size_t cap);

NTG_API size_t
ntg_widget_graph_collect_post(ntg_widget* root, ntg_widget** out, size_t cap);

NTG_API size_t
ntg_widget_graph_collect_roots_pre(ntg_widget* root, ntg_widget** out, size_t cap);

NTG_API size_t
ntg_widget_graph_collect_roots_post(ntg_widget* root, ntg_widget** out, size_t cap);

/* ------------------------------------------------------ */
/* CONVENIENCE */
/* ------------------------------------------------------ */

#define NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(fn_name, perform_fn)                  \
static void fn_name(ntg_widget* widget, void* data)                            \
{                                                                              \
    if(widget == NULL) return;                                                 \
    perform_fn(widget, data);                                                  \
    const struct ntg_widget_vec* children = &widget->ro.children;        \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
}                                                                              \

#define NTG_WIDGET_TREE_DEF_TRAVERSE_POST(fn_name, perform_fn)                 \
static void fn_name(ntg_widget* widget, void* data)                            \
{                                                                              \
    if(widget == NULL) return;                                                 \
    const struct ntg_widget_vec* children = &widget->ro.children;        \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    perform_fn(widget, data);                                                  \
}

#define NTG_WIDGET_GRAPH_DEF_TRAVERSE_PRE(fn_name, perform_fn)                 \
static void fn_name(ntg_widget* widget, void* data)                            \
{                                                                              \
    if(widget == NULL) return;                                                 \
    perform_fn(widget, data);                                                  \
    const struct ntg_widget_vec* children = &widget->ro.children;        \
    const struct ntg_widget_vec* anchored = &widget->ro.anchored;        \
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

#define NTG_WIDGET_GRAPH_DEF_TRAVERSE_POST(fn_name, perform_fn)                \
static void fn_name(ntg_widget* widget, void* data)                            \
{                                                                              \
    if(widget == NULL) return;                                                 \
    const struct ntg_widget_vec* children = &widget->ro.children;        \
    const struct ntg_widget_vec* anchored = &widget->ro.anchored;        \
    size_t i;                                                                  \
    for(i = 0; i < children->size; i++)                                        \
    {                                                                          \
        fn_name(children->data[i], data);                                      \
    }                                                                          \
    for(i = 0; i < anchored->size; i++)                                        \
    {                                                                          \
        fn_name(anchored->data[i], data);                                      \
    }                                                                          \
    perform_fn(widget, data);                                                  \
}

NTG_API int
ntg_widget_sort_by_z(ntg_widget** widgets, size_t size);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_widget_vtable
{
    struct ntg_object_vtable base;

    int (*layout_prepare_fn)(
            ntg_widget* widget,
            struct ntg_widget_layout_dt* layout_dt,
            sarena* arena);

    int (*measure_fn)(
            const ntg_widget* widget,
            struct ntg_widget_layout_dt* layout_dt,
            enum ntg_orient orient,
            sarena* arena,
            uint32_t* relayout,
            struct ntg_widget_measure* out_measure);

    int (*constrain_fn)(
            const ntg_widget* widget,
            struct ntg_widget_layout_dt* layout_dt,
            enum ntg_orient orient,
            ntg_widget_size_map* out_size_map,
            sarena* arena,
            uint32_t* relayout);

    int (*arrange_fn)(
            const ntg_widget* widget,
            struct ntg_widget_layout_dt* layout_dt,
            ntg_widget_pos_map* out_pos_map,
            sarena* arena,
            uint32_t* relayout);

    int (*draw_fn)(
            const ntg_widget* widget,
            struct ntg_widget_layout_dt* layout_dt,
            ntg_widget_tmp_draw* out_drawing,
            sarena* arena);

    /* Raised in layout finalize phase */
    void (*resize_cont_fn)(ntg_widget* widget, sarena* arena);
    void (*resize_fn)(ntg_widget* widget, sarena* arena);
    void (*pos_chng_fn)(ntg_widget* widget, sarena* arena);

    void (*rm_child_fn)(ntg_widget* widget, ntg_widget* child);

    void (*add_anchored_fn)(ntg_widget* widget, ntg_widget* anchored);
    void (*rm_anchored_fn)(ntg_widget* widget, ntg_widget* anchored);

    void (*set_base_fn)(ntg_widget* widget, ntg_widget* anchored);
    void (*rm_base_fn)(ntg_widget* widget, ntg_widget* anchored);

    void (*set_parent_fn)(ntg_widget* widget, ntg_widget* new_parent);
    void (*rm_parent_fn)(ntg_widget* widget, ntg_widget* old_parent);

    void (*enter_scene_fn)(ntg_widget* widget, ntg_scene* new_scene);
    void (*rm_scene_fn)(ntg_widget* widget, ntg_scene* old_scene);

    bool (*handle_key_fn)(ntg_widget* widget, const struct ntg_widget_key* event);
    bool (*handle_mouse_fn)(ntg_widget* widget, const struct ntg_widget_mouse* event);

    void (*focus_fn)(ntg_widget* widget);
    void (*unfocus_fn)(ntg_widget* widget);

    void (*chng_pad_pref_size_fn)(ntg_widget* widget);
    void (*chng_pad_enable_fn)(ntg_widget* widget);

    void (*chng_bdr_pref_size_fn)(ntg_widget* widget);
    void (*chng_bdr_style_fn)(ntg_widget* widget);
    void (*chng_bdr_enable_fn)(ntg_widget* widget);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_widget_init_inherit(
        ntg_widget* widget,
        const struct ntg_widget_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

NTG_API int
ntg_widget_deinit(ntg_widget* widget);

/* ------------------------------------------------------ */

NTG_API int
ntg_widget_attach(ntg_widget* parent, ntg_widget* child);

/* ------------------------------------------------------ */

NTG_API int
ntg_widget_set_base_bg(ntg_widget* widget, struct ntg_vcell base_bg);

NTG_API int
ntg_widget_set_focusable(ntg_widget* widget, enum ntg_widget_fcs_mode mode);

NTG_API int
ntg_widget_set_clickable(ntg_widget* widget, enum ntg_widget_clk_mode mode);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void ntg__widget_root_set_scene(ntg_widget* widget, ntg_scene* scene);

/* Called by scene when adding widget tree in ntg__scene_add_widget_tree() */
void ntg__widget_scene_enter(ntg_widget* widget, ntg_scene* scene);
void ntg__widget_scene_enter_notify(ntg_widget* widget, ntg_scene* scene);

/* Called by scene when removing widget tree in ntg__scene_rm_widget_tree() */
void ntg__widget_scene_leave(ntg_widget* widget, ntg_scene* scene);
void ntg__widget_scene_leave_notify(ntg_widget* widget, ntg_scene* scene);

void ntg__widget_focus(ntg_widget* widget);
void ntg__widget_focus_notify(ntg_widget* widget);

void ntg__widget_unfocus(ntg_widget* widget);
void ntg__widget_unfocus_notify(ntg_widget* widget);

#endif // NTG_WIDGET_H
