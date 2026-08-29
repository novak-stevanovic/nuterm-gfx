#include <limits.h>
#include <stdlib.h>
#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_widget_size_map
{
    ntg_widget** keys;
    size_t* vals;

    size_t size;
};

struct ntg_widget_pos_map
{
    ntg_widget** keys;
    ntg_xy* vals;

    size_t size;
};

enum ntg_widget_repeat_flag
{
    NTG_WIDGET_SKIP_HBORDER = (1u << 0),
    NTG_WIDGET_SKIP_HPADDING = (1u << 1),
    NTG_WIDGET_SPECIAL_REPEAT = (1u << 2)
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* LAYOUT WIDGET INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_widget_size_map* map,
        const struct ntg_widget_vec* children,
        sarena* arena);

static int pos_map_init(
        ntg_widget_pos_map* map,
        const struct ntg_widget_vec* children,
        sarena* arena);

static int tmp_draw_init(
        struct ntg_widget_tmp_draw* drawing,
        ntg_xy size,
        struct ntg_vcell base_bg,
        sarena* arena);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static struct ntg_widget_measure incorporate_lay_conf_measure(
        struct ntg_widget_measure measure,
        size_t conf_min_size,
        size_t conf_max_size,
        size_t conf_grow);
    
static int get_dcr_size(
        enum ntg_widget_dcr_enable enable,
        size_t we_pref_size[2],
        size_t size,
        struct ntg_widget_measure inner_measure,
        size_t we_out_size[2]);

static int vconstrain_border(ntg_widget* widget, bool* out_repeat);
static int vconstrain_padding(ntg_widget* widget, bool* out_repeat);
static int calculate_border_hsize(
        ntg_widget* widget, size_t* out_w, size_t* out_e);
static int calculate_border_vsize(
        ntg_widget* widget, size_t* out_n, size_t* out_s);
static int calculate_padding_hsize(
        ntg_widget* widget, size_t* out_w, size_t* out_e);
static int calculate_padding_vsize(
        ntg_widget* widget, size_t* out_n, size_t* out_s);

static int draw_optimized(ntg_widget* widget, sarena* arena);
static int draw_unoptimized(ntg_widget* widget, sarena* arena);
static void layout_reset(ntg_widget* widget);
static bool set_hmeasure_helper(ntg_widget* widget, struct ntg_widget_measure measure);
static bool set_vmeasure_helper(ntg_widget* widget, struct ntg_widget_measure measure);
static bool set_hsize_helper(ntg_widget* widget, size_t size);
static bool set_vsize_helper(ntg_widget* widget, size_t size);
static inline bool set_pos_helper(ntg_widget* widget, ntg_xy pos);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg_widget_feed_key(ntg_widget* widget, const struct ntg_widget_key* event)
{
    if(!widget) return false;
    if(!event) return false;
    if(!event->target) return false;
    if(nt_key_are_eql(event->key, NT_KEY_ZERO)) return false;

    bool consumed = false;

    if(ntg_wgt_vtbl(widget)->handle_key_fn)
    {
        consumed = ntg_wgt_vtbl(widget)->handle_key_fn(widget, event);
    }

    struct ntg_event_widget_key_dt event_dt = { .key = event };
    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_KEY, &event_dt);

    return consumed;
}

bool ntg_widget_feed_mouse(ntg_widget* widget, const struct ntg_widget_mouse* event)
{
    if(!widget) return false;
    if(!widget->ro.clickable) return false;
    if(!event) return false;
    if(!event->target) return false;
    if(nt_mouse_are_eql(event->mouse, NT_MOUSE_ZERO)) return false;

    ntg_xy size = ntg_widget_get_size(widget);
    if((event->mouse.x >= size.ro.x) || (event->mouse.y >= size.ro.y))
        return false;

    ntg_xy pos = ntg_xy_new(event->mouse.x, event->mouse.y);
    struct ntg_widget_hit_res res;
    res = ntg_widget_hit_test(widget, pos);

    if(!event->from_keybind)
    {
        bool cont_clickable = NTG_WIDGET_CLKABLE_CONT;
        bool cont_pad_click = (res.part == NTG_WIDGET_HIT_CONT) ||
                              (res.part == NTG_WIDGET_HIT_PAD);
        if(!(cont_clickable && cont_pad_click))
            return false;
    }

    bool consumed = false;

    if(ntg_wgt_vtbl(widget)->handle_mouse_fn)
    {
        consumed = ntg_wgt_vtbl(widget)->handle_mouse_fn(widget, event);
    }

    struct ntg_event_widget_mouse_dt event_dt = { .mouse = event };
    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_MOUSE, &event_dt);

    return consumed;
}

/* ------------------------------------------------------ */

void ntg_lay_conf_init(struct ntg_lay_conf* conf)
{
    if(!conf) return;

    (*conf) = (struct ntg_lay_conf) {
        .cont_min_size = NTG_WIDGET_MINSZ_UNSET,
        .cont_max_size = NTG_WIDGET_MAXSZ_UNSET,
        .cont_grow = NTG_WIDGET_GROW_UNSET
    };
}

int ntg_widget_set_lay_conf(
        ntg_widget* widget,
        const struct ntg_lay_conf* conf)
{
    if(!widget || !conf) return NTG_ERR_INV_ARG;

    if(ntg_xy_are_eql(widget->ro.lay_conf.cont_min_size, conf->cont_min_size) &&
       ntg_xy_are_eql(widget->ro.lay_conf.cont_max_size, conf->cont_max_size) &&
       ntg_xy_are_eql(widget->ro.lay_conf.cont_grow, conf->cont_grow))
    {
        return 0;
    }

    widget->ro.lay_conf = (*conf);
    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_FULL);

    return 0;
}

int ntg_widget_set_z_index(ntg_widget* widget, int z_index)
{
    if(!widget) return NTG_ERR_INV_ARG;

    if(widget->ro.z_index == z_index)
        return 0;

    widget->ro.z_index = z_index;
    ntg_widget_mark_dirty(widget, NTG__WIDGET_DIRTY_RENDER);

    return 0;
}

int ntg_widget_set_bdr_opts(ntg_widget* widget, const struct ntg_bdr_opts* opts)
{
    if(!widget) return NTG_ERR_INV_ARG;

    struct ntg_bdr_opts opts_final = (opts ? (*opts) : NTG_BDR_OPTS_ZERO);
    if(!opts_final.style)
        opts_final.style = &NTG_BORDER_STYLE_DEFAULT;

    bool enable_chg = widget->ro.bdr_opts.enable != opts_final.enable;
    bool pref_size_chg = !ntg_insets_are_eql(
            widget->ro.bdr_opts.pref_size,
            opts_final.pref_size);
    bool style_chg = widget->ro.bdr_opts.style != opts_final.style;

    if(!enable_chg && !pref_size_chg && !style_chg)
        return 0;

    widget->ro.bdr_opts = opts_final;

    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_FULL);

    if(pref_size_chg && ntg_wgt_vtbl(widget)->chng_bdr_pref_size_fn)
        ntg_wgt_vtbl(widget)->chng_bdr_pref_size_fn(widget);

    if(style_chg && ntg_wgt_vtbl(widget)->chng_bdr_style_fn)
        ntg_wgt_vtbl(widget)->chng_bdr_style_fn(widget);

    if(enable_chg && ntg_wgt_vtbl(widget)->chng_bdr_enable_fn)
        ntg_wgt_vtbl(widget)->chng_bdr_enable_fn(widget);

    return 0;
}

int ntg_widget_set_pad_opts(ntg_widget* widget, const struct ntg_pad_opts* opts)
{
    if(!widget) return NTG_ERR_INV_ARG;

    struct ntg_pad_opts opts_final = (opts ? (*opts) : NTG_PAD_OPTS_ZERO);

    bool enable_chg = widget->ro.pad_opts.enable != opts_final.enable;
    bool pref_size_chg = !ntg_insets_are_eql(
            widget->ro.pad_opts.pref_size,
            opts_final.pref_size);

    if(!enable_chg && !pref_size_chg)
        return 0;

    widget->ro.pad_opts = opts_final;

    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_FULL);

    if(pref_size_chg && ntg_wgt_vtbl(widget)->chng_pad_pref_size_fn)
        ntg_wgt_vtbl(widget)->chng_pad_pref_size_fn(widget);

    if(enable_chg && ntg_wgt_vtbl(widget)->chng_pad_enable_fn)
        ntg_wgt_vtbl(widget)->chng_pad_enable_fn(widget);

    return 0;
}

int ntg_widget_set_anchor_policy(ntg_widget* widget, const ntg_anchor_policy* policy)
{
    if(!widget) return NTG_ERR_INV_ARG;

    if(!policy)
        policy = &NTG_ANCHOR_POLICY_ROOT;

    widget->ro.anchor_policy = policy;

    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_FULL);

    return 0;
}

/* ------------------------------------------------------ */

struct ntg_widget_hit_res ntg_widget_hit_test(ntg_widget* widget, ntg_xy pos)
{
    struct ntg_widget_hit_res out = {0};

    if(!widget) return out;

    /* If not hit, return NULL */

    if(!ntg_xy_is_in_rect(pos, ntg_xy_new(0, 0), widget->ro.size))
        return out;

    /* Set `out_local_pos`, ancestors will choose the right one later */

    out.widget = widget;
    out.local_pos = pos;

    /* Set `out_hit_res`, ancestors will choose the right one later */

    ntg_insets padding_size = widget->ro.padding_size;
    ntg_insets border_size = widget->ro.border_size;
    ntg_xy cont_size = ntg_widget_get_size_cont(widget);

    if((pos.ro.x > (padding_size.ro.w + cont_size.ro.x + padding_size.ro.e)) ||
       (pos.ro.x < border_size.ro.w) ||
       (pos.ro.y > (padding_size.ro.n + cont_size.ro.y + padding_size.ro.s)) ||
       (pos.ro.y < border_size.ro.n))
    {
        out.part = NTG_WIDGET_HIT_BDR;
    }
    else if((pos.ro.x >= cont_size.ro.x) ||
            (pos.ro.x < (border_size.ro.w + padding_size.ro.w)) ||
            (pos.ro.y >= cont_size.ro.y) ||
            (pos.ro.y < (border_size.ro.n + padding_size.ro.n)))
    {
        out.part = NTG_WIDGET_HIT_PAD;
    }
    else
    {
        out.part = NTG_WIDGET_HIT_CONT;
    }

    size_t i;
    ntg_widget* it_child;
    struct ntg_widget_hit_res it_out;
    ntg_dxy it_adj_pos;
    int curr_z = INT_MIN;
    for(i = 0; i < widget->ro.children.size; i++)
    {
        it_child = widget->ro.children.data[i];
        it_adj_pos = ntg_dxy_sub(ntg_dxy_from_xy(pos), ntg_dxy_from_xy(it_child->ro.pos));

        it_out = ntg_widget_hit_test(it_child, ntg_xy_from_dxy(it_adj_pos));

        if(it_out.widget && (it_child->ro.z_index > curr_z))
        {
            out = it_out;
            curr_z = it_child->ro.z_index;
        }
    }

    return out;
}

/* ------------------------------------------------------ */

bool ntg_widget_is_focused(const ntg_widget* widget)
{
    if(!widget) return false;

    const ntg_scene* scene = ntg_widget_get_scene(widget);
    if(!scene) return false;

    return (scene->ro.fm->ro.focused == widget);
}

/* ------------------------------------------------------ */
/* WIDGET GRAPH OPERATIONS */
/* ------------------------------------------------------ */

int ntg_widget_detach(ntg_widget* widget)
{
    if(!widget) return NTG_ERR_INV_ARG;

    ntg_widget* parent = widget->ro.parent;
    if(parent == NULL) return 0;

    ntg_scene* scene = ntg_widget_get_scene_(widget);

    ntg_widget_vec_rm(&parent->ro.children, widget);

    widget->ro.parent = NULL;

    if(ntg_wgt_vtbl(parent)->rm_child_fn)
        ntg_wgt_vtbl(parent)->rm_child_fn(parent, widget);

    if(scene)
        ntg__scene_rm_widget_tree(scene, widget);

    ntg_widget_mark_dirty(parent, NTG_WIDGET_DIRTY_FULL);

    if(ntg_wgt_vtbl(widget)->rm_parent_fn)
        ntg_wgt_vtbl(widget)->rm_parent_fn(widget, parent);

    struct ntg_event_widget_chldrm_dt chldrm_dt = { .child = widget };
    struct ntg_event_widget_prntrm_dt prntrm_dt = { .parent = parent };

    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_PRNTRM, &prntrm_dt);
            
    ntg_object_event_raise(ntg_obj(parent), NTG_EVENT_WIDGET_CHLDRM, &chldrm_dt);

    if(scene)
        ntg__scene_rm_widget_tree_notify(scene, widget);

    return 0;
}

int ntg_widget_anchor(ntg_widget* base, ntg_widget* root)
{
    int _status;

    if(!base || !root || (base == root))
        return NTG_ERR_INV_ARG;

    if(base->ro.anchored.size >= NTG_WIDGET_MAX_ANCHORED)
        return NTG_ERR_NO_CAP;

    if(root->ro.parent)
    {
        ntg_widget_detach(root);
    }

    if(root->ro.base)
    {
        ntg_widget_unanchor(root);
    }

    _status = ntg_widget_vec_pushb(&base->ro.anchored, root);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;
            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    root->ro.base = base;

    ntg_scene* scene = ntg_widget_get_scene_(root);

    if(ntg_wgt_vtbl(base)->add_anchored_fn)
        ntg_wgt_vtbl(base)->add_anchored_fn(base, root);

    if(ntg_wgt_vtbl(root)->set_base_fn)
        ntg_wgt_vtbl(root)->set_base_fn(root, base);

    if(scene)
        ntg__scene_add_widget_tree(scene, root);

    struct ntg_event_widget_anchadd_dt anchadd_dt = { .anchored = root };
    ntg_object_event_raise(ntg_obj(base), NTG_EVENT_WIDGET_ANCHADD, &anchadd_dt);

    struct ntg_event_widget_bsset_dt bsset_dt = { .base = base };
    ntg_object_event_raise(ntg_obj(root), NTG_EVENT_WIDGET_BSSET, &bsset_dt);

    if(scene)
        ntg__scene_add_widget_tree_notify(scene, root);

    return 0;
}

int ntg_widget_unanchor(ntg_widget* root)
{
    if(!root) return NTG_ERR_INV_ARG;
    if(!root->ro.base) return 0;

    ntg_widget* base = root->ro.base;

    ntg_scene* scene = ntg_widget_get_scene_(root);

    ntg_widget_vec_rm(&root->ro.base->ro.anchored, root);
    root->ro.base = NULL;
    root->ro.anchor_policy = NULL;

    if(ntg_wgt_vtbl(base)->rm_anchored_fn)
        ntg_wgt_vtbl(base)->rm_anchored_fn(base, root);

    if(scene)
        ntg__scene_rm_widget_tree(scene, root);

    if(ntg_wgt_vtbl(root)->rm_base_fn)
        ntg_wgt_vtbl(root)->rm_base_fn(root, base);

    struct ntg_event_widget_anchrm_dt anchrm_dt = { .anchored = root };
    ntg_object_event_raise(ntg_obj(base), NTG_EVENT_WIDGET_ANCHRM, &anchrm_dt);

    struct ntg_event_widget_bsrm_dt bsrm_dt = { .base = base };
    ntg_object_event_raise(ntg_obj(root), NTG_EVENT_WIDGET_BSRM, &bsrm_dt);

    if(scene)
        ntg__scene_rm_widget_tree_notify(scene, root);

    return 0;
}

int ntg_widget_remove_from_scene(ntg_widget* widget)
{
    if(!widget) return NTG_ERR_INV_ARG;

    ntg_widget* parent = widget->ro.parent;
    if(parent)
    {
        ntg_widget_detach(widget);
        return 0;
    }

    ntg_widget* base = widget->ro.base;
    if(base)
    {
        ntg_widget_unanchor(widget);
        return 0;
    }

    if(ntg_widget_is_true_root(widget))
    {
        ntg_scene_rm_root(ntg_widget_get_scene_(widget), widget);
        return 0;
    }

    return 0;
}

/* ------------------------------------------------------ */
/* WIDGET GRAPH QUERY */
/* ------------------------------------------------------ */

const ntg_widget* ntg_widget_get_graph_root(const ntg_widget* widget)
{
    if(!widget) return NULL;

    const ntg_widget *it_obj = widget, *it_root, *it_base;
    while(true)
    {
        it_root = ntg_widget_get_tree_root(it_obj);
        it_base = it_root->ro.base;
        it_obj = it_base;

        if(!it_base) break;
    }

    return it_root;
}

ntg_widget* ntg_widget_get_graph_root_(ntg_widget* widget)
{
    if(!widget) return NULL;

    return (ntg_widget*)ntg_widget_get_graph_root(widget);
}

/* ------------------------------------------------------ */

const ntg_widget* ntg_widget_get_tree_root(const ntg_widget* widget)
{
    if(!widget) return NULL;

    while(widget->ro.parent) widget = widget->ro.parent;
    return widget;
}

ntg_widget* ntg_widget_get_tree_root_(ntg_widget* widget)
{
    if(!widget) return NULL;

    return (ntg_widget*)ntg_widget_get_tree_root(widget);
}

/* ------------------------------------------------------ */

ntg_scene* ntg_widget_get_scene_(ntg_widget* widget)
{
    if(!widget) return NULL;

    ntg_widget* root = ntg_widget_get_graph_root_(widget);

    return root->priv.scene;
}

const ntg_scene* ntg_widget_get_scene(const ntg_widget* widget)
{
    if(!widget) return NULL;

    const ntg_widget* root = ntg_widget_get_graph_root(widget);

    return root->priv.scene;
}

/* ------------------------------------------------------ */

bool ntg_widget_is_true_root(const ntg_widget* widget)
{
    if(!widget) return false;
    
    return (ntg_widget_is_graph_root(widget) && ntg_widget_get_scene(widget));
}

bool ntg_widget_is_graph_root(const ntg_widget* widget)
{
    if(!widget) return false;

    return ((!widget->ro.parent) && (!widget->ro.base));
}

bool ntg_widget_is_tree_root(const ntg_widget* widget)
{
    if(!widget) return false;

    return (!widget->ro.parent);
}

/* ------------------------------------------------------ */

bool ntg_widget_is_in_tree(const ntg_widget* tree_root, const ntg_widget* desc)
{
    if(!tree_root || !desc) return false;

    const ntg_widget* it = desc;
    while(it)
    {
        if(it == tree_root) return true;
        it = it->ro.parent;
    }

    return false;
}

bool ntg_widget_is_in_graph(const ntg_widget* graph_root, const ntg_widget* desc)
{
    if(!graph_root || !desc) return false;

    const ntg_widget* it = desc;
    while(it)
    {
        if(it == graph_root) return true;

        if(it->ro.parent) // If has parent keep advancing
            it = it->ro.parent;
        else // Else traverse graph (process next tree)
            it = it->ro.base;
    }

    return false;
}

/* ------------------------------------------------------ */
/* SIZE & POS */
/* ------------------------------------------------------ */

ntg_xy ntg_widget_get_size(const ntg_widget* widget)
{
    if(!widget)
        return ntg_xy_new(0, 0);

    return widget->ro.size;
}

ntg_xy ntg_widget_get_size_cont(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets border_size = widget->ro.border_size;
    ntg_insets padding_size = widget->ro.padding_size;

    ntg_xy sub = ntg_xy_new(
        ntg_insets_hsum(border_size) + ntg_insets_hsum(padding_size),
        ntg_insets_vsum(border_size) + ntg_insets_vsum(padding_size)
    );
    return ntg_xy_sub(widget->ro.size, sub);
}

ntg_xy ntg_widget_get_size_pad(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets border_size = widget->ro.border_size;

    ntg_xy sub = ntg_xy_new(
            ntg_insets_hsum(border_size),
            ntg_insets_vsum(border_size)
    );
    return ntg_xy_sub(widget->ro.size, sub);
}

ntg_xy ntg_widget_get_abs_pos(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    return ntg_xy_from_dxy(ntg_widget_map_to_scene(widget, ntg_dxy_new(0, 0)));
}

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

ntg_dxy ntg_widget_map_to_ancs(
        const ntg_widget* widget,
        const ntg_widget* ancs,
        ntg_dxy point)
{
    if(!widget) return NTG_DXY_MAX;

    if(widget == ancs)
        return point;

    ntg_dxy out = point;

    const ntg_widget* it = widget;
    while(it && (it != ancs))
    {
        out = ntg_dxy_add(out, ntg_dxy_from_xy(it->ro.pos));
        it = it->ro.parent;
    }

    return out;
}

ntg_dxy ntg_widget_map_to_desc(
        const ntg_widget* widget,
        const ntg_widget* desc,
        ntg_dxy point)
{
    if(!desc) return NTG_DXY_MAX;

    ntg_dxy desc_pos = ntg_widget_map_to_ancs(
            desc, widget, ntg_dxy_new(0, 0));

    return ntg_dxy_sub(point, desc_pos);
}

ntg_dxy 
ntg_widget_map_to_scene(const ntg_widget* widget, ntg_dxy point)
{
    return ntg_widget_map_to_ancs(widget, NULL, point);
}

ntg_dxy 
ntg_widget_map_from_scene(const ntg_widget* widget, ntg_dxy point)
{
    if(!widget)
        return NTG_DXY_MAX;

    return ntg_widget_map_to_desc(NULL, widget, point);
}

/* ------------------------------------------------------ */
/* COLLECT */
/* ------------------------------------------------------ */

struct count_data
{
    ntg_widget** out; // non-null
    size_t cap;
    size_t count;
};

static inline void collect_fn(ntg_widget* widget, void* _data)
{
    if(!widget) return;

    struct count_data* data = _data;

    if(data->out && (data->count < data->cap))
        data->out[data->count] = widget;        

    data->count++;
}

static inline void collect_root_fn(ntg_widget* widget, void* _data)
{
    if(!widget) return;
    if(!ntg_widget_is_tree_root(widget)) return;

    struct count_data* data = _data;

    if(data->out && (data->count < data->cap))
        data->out[data->count] = widget;        

    data->count++;
}

NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(pre_collect_tree, collect_fn)
NTG_WIDGET_TREE_DEF_TRAVERSE_POST(post_collect_tree, collect_fn)
NTG_WIDGET_GRAPH_DEF_TRAVERSE_PRE(pre_collect_graph, collect_fn)
NTG_WIDGET_GRAPH_DEF_TRAVERSE_POST(post_collect_graph, collect_fn)
NTG_WIDGET_GRAPH_DEF_TRAVERSE_PRE(pre_collect_roots_graph, collect_root_fn)
NTG_WIDGET_GRAPH_DEF_TRAVERSE_POST(post_collect_roots_graph, collect_root_fn)

size_t ntg_widget_tree_collect_pre(ntg_widget* root, ntg_widget** out, size_t cap)
{
    if(!root) return 0;

    struct count_data data = {
        .out = out,
        .count = 0,
        .cap = cap
    };

    pre_collect_tree(root, &data);

    return data.count;
}

size_t ntg_widget_tree_collect_post(ntg_widget* root, ntg_widget** out, size_t cap)
{
    if(!root) return 0;

    struct count_data data = {
        .out = out,
        .count = 0,
        .cap = cap
    };

    post_collect_tree(root, &data);

    return data.count;
}

size_t ntg_widget_graph_collect_pre(ntg_widget* root, ntg_widget** out, size_t cap)
{
    if(!root) return 0;

    struct count_data data = {
        .out = out,
        .count = 0,
        .cap = cap
    };

    pre_collect_graph(root, &data);

    return data.count;
}

size_t ntg_widget_graph_collect_post(ntg_widget* root, ntg_widget** out, size_t cap)
{
    if(!root) return 0;

    struct count_data data = {
        .out = out,
        .count = 0,
        .cap = cap
    };

    post_collect_graph(root, &data);

    return data.count;
}

size_t ntg_widget_graph_collect_roots_pre(ntg_widget* root, ntg_widget** out, size_t cap)
{
    if(!root) return 0;

    struct count_data data = {
        .out = out,
        .count = 0,
        .cap = cap
    };

    pre_collect_roots_graph(root, &data);

    return data.count;
}

size_t ntg_widget_graph_collect_roots_post(ntg_widget* root, ntg_widget** out, size_t cap)
{
    if(!root) return 0;

    struct count_data data = {
        .out = out,
        .count = 0,
        .cap = cap
    };

    post_collect_roots_graph(root, &data);

    return data.count;
}

/* ------------------------------------------------------ */
/* CONVENIENCE */
/* ------------------------------------------------------ */

int ntg_widget_sort_by_z(ntg_widget** widgets, size_t size)
{
    if(!widgets) return NTG_ERR_INV_ARG;

    if(size == 0) return 0;
    
    size_t i, j;

    ntg_widget *tmp_obj;
    for(i = 0; i < size - 1; i++)
    {
        for(j = i + 1; j < size; j++)
        {
            if(widgets[j]->ro.z_index < widgets[i]->ro.z_index)
            {
                tmp_obj = widgets[i];
                widgets[i] = widgets[j];
                widgets[j] = tmp_obj;
            }
        }
    }

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_widget* widget)
{
    ntg_object_zero(widget);

    widget->ro.anchor_policy = &NTG_ANCHOR_POLICY_ROOT;
    widget->ro.bdr_opts.style = &NTG_BORDER_STYLE_DEFAULT;

    ntg_lay_conf_init(&widget->ro.lay_conf);
    widget->ro.z_index = NTG_WIDGET_ZIDX_UNSET;
    widget->ro.pos = NTG_WIDGET_POS_NODRAW; // Not drawn
}

int ntg_widget_init_inherit(
        ntg_widget* widget,
        const struct ntg_widget_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!widget || !vtable || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_WIDGET))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_object_init_inherit(ntg_obj(widget), &vtable->base, type);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    init_default(widget);

    widget->priv.layout_dt = layout_dt;
    ntg_widget_draw_init(&widget->ro.drawing);

    return 0;
}

int ntg_widget_deinit(ntg_widget* widget)
{
    ntg_not_null(widget);

    ntg_scene* scene = ntg_widget_get_scene_(widget);
    
    if(ntg_widget_is_true_root(widget))
    {
        ntg_scene_rm_root(scene, widget);
    }

    if(widget->ro.parent)
    {
        ntg_widget_detach(widget);
    }

    if(widget->ro.base)
    {
        ntg_widget_unanchor(widget);
    }

    while(widget->ro.children.size > 0)
    {
        ntg_widget_detach(widget->ro.children.data[0]);
    }

    while(widget->ro.anchored.size > 0)
    {
        ntg_widget_unanchor(widget->ro.anchored.data[0]);
    }

    ntg_widget_vec_deinit(&widget->ro.children);

    ntg_widget_vec_deinit(&widget->ro.anchored);

    ntg_widget_draw_deinit(&widget->ro.drawing);

    if(widget->priv.layout_dt && widget->priv.layout_dt->free_fn)
        widget->priv.layout_dt->free_fn(widget->priv.layout_dt);

    ntg_object_zero(widget);
    ntg_object_deinit(ntg_obj(widget));

    return 0;
}

/* ------------------------------------------------------ */

int ntg_widget_attach(ntg_widget* parent, ntg_widget* child)
{
    int _status;

    if(!parent || !child || (child == parent))
        return NTG_ERR_INV_ARG;

    if(parent->ro.children.size >= NTG_WIDGET_MAX_CHILDREN)
        return NTG_ERR_NO_CAP;

    if(child->ro.parent != NULL)
        ntg_widget_detach(child);

    if(child->priv.scene) // child is root
        ntg_scene_rm_root(child->priv.scene, child);

    if(child->ro.base)
        ntg_widget_unanchor(child);

    ntg_scene* scene = ntg_widget_get_scene_(parent);

    _status = ntg_widget_vec_pushb(&parent->ro.children, child);
    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                return NTG_ERR_ALLOC_FAIL;
            default:
                return NTG_ERR_UNEXPECTED;
        }
    }

    child->ro.parent = parent;

    if(scene)
        ntg__scene_add_widget_tree(scene, child);

    if(ntg_wgt_vtbl(child)->set_parent_fn)
        ntg_wgt_vtbl(child)->set_parent_fn(child, parent);

    struct ntg_event_widget_chldadd_dt chldadd_dt = { .child = child };
    struct ntg_event_widget_prntset_dt prntset_dt = { .parent = parent };

    ntg_object_event_raise(ntg_obj(parent), NTG_EVENT_WIDGET_CHLDADD, &chldadd_dt);

    ntg_object_event_raise(ntg_obj(child), NTG_EVENT_WIDGET_PRNTSET, &prntset_dt);

    if(scene)
        ntg__scene_add_widget_tree_notify(scene, child);

    ntg_widget_mark_dirty(parent, NTG_WIDGET_DIRTY_FULL);
    return 0;
}

/* ------------------------------------------------------ */

int ntg_widget_set_base_bg(ntg_widget* widget, struct ntg_vcell base_bg)
{
    if(!widget) return NTG_ERR_INV_ARG;

    widget->priv.base_bg = base_bg;

    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_DRAW);

    return 0;
}

int ntg_widget_set_focusable(ntg_widget* widget, enum ntg_widget_fcs_mode mode)
{
    if(!widget) return NTG_ERR_INV_ARG;

    if(widget->ro.focusable == NTG_WIDGET_FCSABLE)
    {
        const ntg_scene* scene = ntg_widget_get_scene(widget);
        if(scene)
        {
            ntg_fcs_manager* fm = scene->ro.fm;
            if(fm && (fm->ro.focused == widget))
                ntg_fcs_manager_request_focus(fm, NULL);
        }
    }

    widget->ro.focusable = mode;

    return 0;
}

int ntg_widget_set_clickable(ntg_widget* widget, enum ntg_widget_clk_mode mode)
{
    if(!widget) return NTG_ERR_INV_ARG;

    widget->ro.clickable = mode;

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void ntg__widget_root_set_scene(ntg_widget* widget, ntg_scene* scene)
{
    if(!widget) return;
    if(widget->ro.parent || widget->ro.base) return;

    widget->priv.scene = scene;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* MEASURE PHASE */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* CONSTRAIN PHASE */
/* ------------------------------------------------------ */

size_t ntg_widget_size_map_get(
        const ntg_widget_size_map* map,
        const ntg_widget* widget)
{
    if(!map || !widget)
        return NTG_SIZE_MAX;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == widget)
            return map->vals[i];
    }

    return NTG_SIZE_MAX;
}

int ntg_widget_size_map_set(
        ntg_widget_size_map* map,
        const ntg_widget* widget,
        size_t size)
{
    if(!map || !widget) return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == widget)
        {
            map->vals[i] = size;
            break;
        }
    }

    return 0;
}

int ntg_widget_zero_constrain(const ntg_widget* widget, ntg_widget_size_map* map)
{
    if(!widget || !map)
        return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < widget->ro.children.size; i++)
        ntg_widget_size_map_set(map, widget, 0);

    return 0;
}

/* ------------------------------------------------------ */
/* ARRANGE PHASE */
/* ------------------------------------------------------ */

ntg_xy ntg_widget_pos_map_get(
        const ntg_widget_pos_map* map,
        const ntg_widget* widget)
{
    if(!map || !widget)
        return NTG_XY_MAX;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == widget)
            return map->vals[i];
    }

    return NTG_XY_MAX;
}

int ntg_widget_pos_map_set(
        ntg_widget_pos_map* map,
        const ntg_widget* widget,
        ntg_xy pos)
{
    if(!map || !widget) return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == widget)
        {
            map->vals[i] = pos;
            break;
        }
    }

    return 0;
}

int ntg_widget_zero_arrange(const ntg_widget* widget, ntg_widget_pos_map* map)
{
    if(!widget || !map)
        return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < widget->ro.children.size; i++)
        ntg_widget_pos_map_set(map, widget, ntg_xy_new(0, 0));

    return 0;
}

/* ------------------------------------------------------ */
/* DRAW PHASE */
/* ------------------------------------------------------ */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_widget_mark_dirty(ntg_widget* widget, uint32_t dirty)
{
    if(!widget) return NTG_ERR_INV_ARG;
    if(!dirty) return 0;

    widget->ro.dirty |= dirty;

    ntg_scene* scene = ntg_widget_get_scene_(widget);
    if(scene)
        ntg_scene_mark_dirty(scene);

    return 0;
}

/* ------------------------------------------------------ */
/* MEASURE & SIZE HELPERS */
/* ------------------------------------------------------ */

ntg_xy ntg_widget_get_min_size(const ntg_widget* widget)
{
    if(!widget)
        return ntg_xy_new(0, 0);

    return widget->ro.min_size;
}

ntg_xy ntg_widget_get_nat_size(const ntg_widget* widget)
{
    if(!widget)
        return ntg_xy_new(0, 0);

    return widget->ro.nat_size;
}

ntg_xy ntg_widget_get_max_size(const ntg_widget* widget)
{
    if(!widget)
        return ntg_xy_new(0, 0);

    return widget->ro.max_size;
}

struct ntg_widget_measure
ntg_widget_get_measure(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget)
        return (struct ntg_widget_measure) {0};

    return (struct ntg_widget_measure) {
        .min_size = ntg_xy_get(widget->ro.min_size, orient),
        .nat_size = ntg_xy_get(widget->ro.nat_size, orient),
        .max_size = ntg_xy_get(widget->ro.max_size, orient),
        .grow = ntg_xy_get(widget->ro.grow, orient)
    };
}

size_t ntg_widget_get_size_1d(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget)
        return 0;
        
    return ntg_xy_get(widget->ro.size, orient);
}

ntg_xy ntg_widget_get_min_size_cont(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;
    ntg_insets pref_padding_size = widget->ro.pad_opts.pref_size;

    ntg_xy sub = ntg_xy_new(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(widget->ro.min_size, sub);
}

ntg_xy ntg_widget_get_nat_size_cont(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;
    ntg_insets pref_padding_size = widget->ro.pad_opts.pref_size;

    ntg_xy sub = ntg_xy_new(
        ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
        ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(widget->ro.nat_size, sub);
}

ntg_xy ntg_widget_get_max_size_cont(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;
    ntg_insets pref_padding_size = widget->ro.pad_opts.pref_size;

    ntg_xy sub = ntg_xy_new(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(widget->ro.max_size, sub);
}

struct ntg_widget_measure
ntg_widget_get_measure_cont(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget)
    {
        return (struct ntg_widget_measure) {0};
    }

    struct ntg_widget_measure m = ntg_widget_get_measure(widget, orient);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;
    ntg_insets pref_padding_size = widget->ro.pad_opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient) +
            ntg_insets_sum(pref_padding_size, orient);

    m.min_size = ntg_sub2_size(m.min_size, sub);
    m.nat_size = ntg_sub2_size(m.nat_size, sub);
    m.max_size = ntg_sub2_size(m.max_size, sub);

    return m;
}

size_t ntg_widget_get_size_1d_cont(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget) return 0;

    size_t s = ntg_xy_get(widget->ro.size, orient);

    ntg_insets border_size = widget->ro.border_size;
    ntg_insets padding_size = widget->ro.padding_size;

    size_t sub = ntg_insets_sum(border_size, orient) +
            ntg_insets_sum(padding_size, orient);

    return ntg_sub2_size(s, sub);
}

size_t ntg_widget_get_for_size_cont(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget) return 0;

    return (orient == NTG_ORIENT_H) ?
            NTG_SIZE_MAX :
            ntg_widget_get_size_1d_cont(widget, NTG_ORIENT_H);
}

ntg_xy ntg_widget_get_min_size_pad(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;

    ntg_xy sub = ntg_xy_new(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(widget->ro.min_size, sub);
}

ntg_xy ntg_widget_get_nat_size_pad(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;

    ntg_xy sub = ntg_xy_new(
        ntg_insets_hsum(pref_border_size),
        ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(widget->ro.nat_size, sub);
}

ntg_xy ntg_widget_get_max_size_pad(const ntg_widget* widget)
{
    if(!widget) return ntg_xy_new(0, 0);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;

    ntg_xy sub = ntg_xy_new(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(widget->ro.max_size, sub);
}

struct ntg_widget_measure
ntg_widget_get_measure_pad(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget)
    {
        return (struct ntg_widget_measure) {0};
    }

    struct ntg_widget_measure m = ntg_widget_get_measure(widget, orient);

    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient);

    m.min_size = ntg_sub2_size(m.min_size, sub);
    m.nat_size = ntg_sub2_size(m.nat_size, sub);
    m.max_size = ntg_sub2_size(m.max_size, sub);

    return m;
}

size_t ntg_widget_get_size_1d_pad(const ntg_widget* widget, enum ntg_orient orient)
{
    if(!widget) return 0;

    size_t s = ntg_xy_get(widget->ro.size, orient);

    ntg_insets border_size = widget->ro.border_size;

    size_t sub = ntg_insets_sum(border_size, orient);

    return ntg_sub2_size(s, sub);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__widget_layout_prepare(ntg_widget* widget, sarena* arena)
{
    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    if(ntg_wgt_vtbl(widget)->layout_prepare_fn)
    {
        return ntg_wgt_vtbl(widget)->layout_prepare_fn(
                widget, widget->priv.layout_dt, arena);
    }

    return 0;
}

int ntg__widget_hmeasure(ntg_widget* widget, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    struct ntg_widget_measure measure = {0};
    int _status = 0;
    if(ntg_wgt_vtbl(widget)->measure_fn)
    {
        _status = ntg_wgt_vtbl(widget)->measure_fn(
                widget,
                widget->priv.layout_dt,
                NTG_ORIENT_H,
                arena,
                relayout,
                &measure);
        if(_status)
        {
            set_hmeasure_helper(widget, measure);
            return _status;
        }

        size_t extra = ntg_insets_hsum(widget->ro.pad_opts.pref_size) +
                       ntg_insets_hsum(widget->ro.bdr_opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_lay_conf_measure(
                measure,
                widget->ro.lay_conf.cont_min_size.ro.x + extra,
                widget->ro.lay_conf.cont_max_size.ro.x + extra,
                widget->ro.lay_conf.cont_grow.ro.x);

        if(measure.min_size == extra)
            measure.min_size = 0;

        if(measure.nat_size == extra)
            measure.nat_size = 0;

        if(measure.max_size == extra)
            measure.max_size = 0;
    }

    set_hmeasure_helper(widget, measure);
    return 0;
}

int ntg__widget_hconstrain(ntg_widget* widget, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    if(widget->priv.skip_hborder)
    {
        widget->ro.border_size = ntg_insets_set(widget->ro.border_size, 0, NTG_DIR_W);
        widget->ro.border_size = ntg_insets_set(widget->ro.border_size, 0, NTG_DIR_E);
    }
    else
    {
        size_t w, e;
        int status = calculate_border_hsize(widget, &w, &e);
        if(status != 0)
            return status;

        widget->ro.border_size = ntg_insets_set(widget->ro.border_size, w, NTG_DIR_W);
        widget->ro.border_size = ntg_insets_set(widget->ro.border_size, e, NTG_DIR_E);
    }
    if(widget->priv.skip_hpadding)
    {
        widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, 0, NTG_DIR_W);
        widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, 0, NTG_DIR_E);
    }
    else
    {
        size_t w, e;
        int status = calculate_padding_hsize(widget, &w, &e);
        if(status != 0)
            return status;

        widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, w, NTG_DIR_W);
        widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, e, NTG_DIR_E);
    }

    ntg_widget_mark_dirty(widget,
            NTG_WIDGET_DIRTY_VCONSTRAIN |
            NTG_WIDGET_DIRTY_ARRANGE |
            NTG_WIDGET_DIRTY_DRAW);

    int _status;

    size_t i;
    size_t content_size = ntg_widget_get_size_1d_cont(widget, NTG_ORIENT_H);

    ntg_widget_size_map map;
    _status = size_map_init(&map, &widget->ro.children, arena);
    if(_status != 0)
    {
        for(i = 0; i < widget->ro.children.size; i++)
            set_hsize_helper(widget->ro.children.data[i], 0);

        return _status;
    }

    if(ntg_wgt_vtbl(widget)->constrain_fn)
    {
        _status = ntg_wgt_vtbl(widget)->constrain_fn(
                widget,
                widget->priv.layout_dt,
                NTG_ORIENT_H,
                &map,
                arena,
                relayout);
        if(_status)
        {
            for(i = 0; i < widget->ro.children.size; i++)
                set_hsize_helper(widget->ro.children.data[i], 0);

            return _status;
        }
    }

    ntg_widget* it_child;
    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = ntg_min2_size(content_size, map.vals[i]);

        set_hsize_helper(it_child, it_size);
    }
    return 0;
}

int ntg__widget_vmeasure(ntg_widget* widget, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    struct ntg_widget_measure measure = {0};
    int _status = 0;
    if(ntg_wgt_vtbl(widget)->measure_fn)
    {
        _status = ntg_wgt_vtbl(widget)->measure_fn(
                widget,
                widget->priv.layout_dt,
                NTG_ORIENT_V,
                arena,
                relayout,
                &measure);
        if(_status)
        {
            set_vmeasure_helper(widget, measure);
            return _status;
        }

        size_t extra = ntg_insets_vsum(widget->ro.pad_opts.pref_size) +
                ntg_insets_vsum(widget->ro.bdr_opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_lay_conf_measure(
                measure,
                widget->ro.lay_conf.cont_min_size.ro.y + extra,
                widget->ro.lay_conf.cont_max_size.ro.y + extra,
                widget->ro.lay_conf.cont_grow.ro.y);

        if(measure.min_size == extra)
            measure.min_size = 0;

        if(measure.nat_size == extra)
            measure.nat_size = 0;

        if(measure.max_size == extra)
            measure.max_size = 0;
    }

    set_vmeasure_helper(widget, measure);
    return 0;
}

int ntg__widget_vconstrain(ntg_widget* widget, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    ntg_widget_mark_dirty(widget,
            NTG_WIDGET_DIRTY_ARRANGE |
            NTG_WIDGET_DIRTY_DRAW);

    bool repeat_border = false;
    bool repeat_padding = false;
    int _status = vconstrain_border(widget, &repeat_border);
    if(_status != 0)
        return _status;
    _status = vconstrain_padding(widget, &repeat_padding);
    if(_status != 0)
        return _status;
    if(relayout && (repeat_border || repeat_padding))
        (*relayout) |= NTG_WIDGET_DIRTY_HCONSTRAIN;

    size_t i;
    size_t content_size = ntg_widget_get_size_1d_cont(widget, NTG_ORIENT_V);

    ntg_widget_size_map map;
    _status = size_map_init(&map, &widget->ro.children, arena);
    if(_status)
    {
        for(i = 0; i < widget->ro.children.size; i++)
            set_vsize_helper(widget->ro.children.data[i], 0);

        return _status;
    }

    if(ntg_wgt_vtbl(widget)->constrain_fn)
    {
        _status = ntg_wgt_vtbl(widget)->constrain_fn(
                widget,
                widget->priv.layout_dt,
                NTG_ORIENT_V,
                &map,
                arena,
                relayout);
        if(relayout && (repeat_border || repeat_padding))
            (*relayout) |= NTG_WIDGET_DIRTY_HCONSTRAIN;
        if(_status)
        {
            for(i = 0; i < widget->ro.children.size; i++)
                set_vsize_helper(widget->ro.children.data[i], 0);

            return _status;
        }
    }

    ntg_widget* it_child;
    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = ntg_min2_size(content_size, map.vals[i]);

        set_vsize_helper(it_child, it_size);
    }
    return 0;
}

int ntg__widget_arrange(ntg_widget* widget, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    int _status;
    size_t i;

    ntg_widget_pos_map map;
    _status = pos_map_init(&map, &widget->ro.children, arena);
    if(_status)
    {
        for(i = 0; i < widget->ro.children.size; i++)
            set_pos_helper(widget->ro.children.data[i], NTG_WIDGET_POS_NODRAW);

        return _status;
    }

    if(ntg_wgt_vtbl(widget)->arrange_fn)
    {
        _status = ntg_wgt_vtbl(widget)->arrange_fn(
                widget,
                widget->priv.layout_dt,
                &map,
                arena,
                relayout);
        if(_status)
        {
            for(i = 0; i < widget->ro.children.size; i++)
                set_pos_helper(widget->ro.children.data[i], NTG_WIDGET_POS_NODRAW);

            return _status;
        }
    }

    ntg_xy dcr_sum = ntg_xy_new(
            widget->ro.border_size.ro.w + widget->ro.padding_size.ro.w,
            widget->ro.border_size.ro.n + widget->ro.padding_size.ro.n);

    ntg_widget* it_child;
    ntg_xy it_pos;

    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_pos = ntg_xy_add(map.vals[i], dcr_sum);
        it_pos = ntg_xy_pos_clamp(it_pos, it_child->ro.size, widget->ro.size);

        set_pos_helper(it_child, it_pos);
    }
    return 0;
}

void ntg__widget_layout_finalize(ntg_widget* widget, sarena* arena)
{
    if(!widget) return;

    (void)arena;

    ntg_xy old_size = widget->priv.old_size;
    ntg_xy new_size = widget->ro.size;

    if(ntg_xy_is_zero_any(new_size))
        widget->ro.pos = NTG_WIDGET_POS_NODRAW;

    if(!ntg_xy_are_eql(old_size, new_size))
    {
        if(ntg_wgt_vtbl(widget)->resize_fn)
            ntg_wgt_vtbl(widget)->resize_fn(widget, arena);

        struct ntg_event_widget_szchg_dt event_dt = {
            .old_size = &old_size,
            .new_size = &new_size
        };

        ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_SZCHG, &event_dt);
    }

    ntg_xy old_cont_size = widget->priv.old_cont_size;
    ntg_xy new_cont_size = ntg_widget_get_size_cont(widget);
    if(!ntg_xy_are_eql(old_cont_size, new_cont_size))
    {
        if(ntg_wgt_vtbl(widget)->resize_cont_fn)
            ntg_wgt_vtbl(widget)->resize_cont_fn(widget, arena);

        struct ntg_event_widget_contszchg_dt event_dt = {
            .old_size = &old_size,
            .new_size = &new_size
        };

        ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_CONTSZCHG, &event_dt);
    }

    ntg_xy old_pos = widget->priv.old_pos;
    ntg_xy new_pos = ntg_widget_get_abs_pos(widget);
    if(!ntg_xy_are_eql(old_pos, new_pos))
    {
        if(ntg_wgt_vtbl(widget)->pos_chng_fn)
            ntg_wgt_vtbl(widget)->pos_chng_fn(widget, arena);

        struct ntg_event_widget_poschg_dt event_dt = {
            .old_pos = &old_pos,
            .new_pos = &new_pos
        };

        ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_POSCHG, &event_dt);
    }
}

int ntg__widget_draw(ntg_widget* widget, sarena* arena)
{
    if(!widget || !arena)
        return NTG_ERR_INV_ARG;

    ntg_widget_mark_dirty(widget, NTG__WIDGET_DIRTY_RENDER);

    const ntg_scene* scene = ntg_widget_get_scene_(widget);
    if(!scene)
        return NTG_ERR_INV_ARG;

    int _status = ntg_widget_draw_set_size(&widget->ro.drawing, widget->ro.size);
    if(_status != 0) return _status;

    if(ntg_insets_hsum(widget->ro.border_size) || ntg_insets_vsum(widget->ro.border_size))
        _status = draw_unoptimized(widget, arena);
    else
        _status = draw_optimized(widget, arena);

    return _status;
}

void ntg__widget_root_set_hsize(ntg_widget* widget, size_t size)
{
    if(!widget) return;
    if(widget->ro.parent) return;

    set_hsize_helper(widget, size);
}

void ntg__widget_root_set_vsize(ntg_widget* widget, size_t size)
{
    if(!widget) return;
    if(widget->ro.parent) return;

    set_vsize_helper(widget, size);
}

void ntg__widget_root_set_pos(ntg_widget* widget, ntg_xy pos)
{
    if(!widget) return;
    if(widget->ro.parent) return;

    set_pos_helper(widget, pos);
}

void ntg__widget_clean(ntg_widget* widget, uint32_t clean)
{
    if(!widget) return;

    widget->ro.dirty &= (~clean);
}

void ntg__widget_scene_enter(ntg_widget* widget, ntg_scene* scene)
{
    if(!widget) return;

    layout_reset(widget);
    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_FULL);

    if(ntg_wgt_vtbl(widget)->enter_scene_fn)
        ntg_wgt_vtbl(widget)->enter_scene_fn(widget, scene);
}

void ntg__widget_scene_enter_notify(ntg_widget* widget, ntg_scene* scene)
{
    if(!widget) return;

    struct ntg_event_widget_scnset_dt event_dt = { .scene = scene };
    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_SCNSET, &event_dt);
}

void ntg__widget_scene_leave(ntg_widget* widget, ntg_scene* scene)
{
    if(!widget) return;

    layout_reset(widget);

    if(ntg_wgt_vtbl(widget)->rm_scene_fn)
        ntg_wgt_vtbl(widget)->rm_scene_fn(widget, scene);
}

void ntg__widget_scene_leave_notify(ntg_widget* widget, ntg_scene* scene)
{
    if(!widget) return;

    struct ntg_event_widget_scnrm_dt event_dt = { .scene = scene };
    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_SCNRM, &event_dt);
}

void ntg__widget_focus(ntg_widget* widget)
{
    if(ntg_wgt_vtbl(widget)->focus_fn)
        ntg_wgt_vtbl(widget)->focus_fn(widget);
}

void ntg__widget_focus_notify(ntg_widget* widget)
{
    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_FCS, NULL);
}

void ntg__widget_unfocus(ntg_widget* widget)
{
    if(ntg_wgt_vtbl(widget)->unfocus_fn)
        ntg_wgt_vtbl(widget)->unfocus_fn(widget);
}

void ntg__widget_unfocus_notify(ntg_widget* widget)
{
    ntg_object_event_raise(ntg_obj(widget), NTG_EVENT_WIDGET_UNFCS, NULL);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* LAYOUT WIDGET INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_widget_size_map* map,
        const struct ntg_widget_vec* children,
        sarena* arena)
{
    if(children->size == 0)
    {
        map->keys = NULL;
        map->vals = NULL;
        map->size = 0;
        return 0;
    }

    map->size = children->size;
    map->keys = sarena_malloc(arena, sizeof(ntg_widget*) * children->size);
    if(!map->keys) return NTG_ERR_ALLOC_FAIL;
    map->vals = sarena_malloc(arena, sizeof(size_t) * children->size);
    if(!map->vals) return NTG_ERR_ALLOC_FAIL;
    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = 0;
    }

    return 0;
}

static int pos_map_init(
        ntg_widget_pos_map* map,
        const struct ntg_widget_vec* children,
        sarena* arena)
{
    if(children->size == 0)
    {
        map->keys = NULL;
        map->vals = NULL;
        map->size = 0;
        return 0;
    }

    map->size = children->size;
    map->keys = sarena_malloc(arena, sizeof(ntg_widget*) * children->size);
    if(!map->keys) return NTG_ERR_ALLOC_FAIL;
    map->vals = sarena_malloc(arena, sizeof(ntg_xy) * children->size);
    if(!map->vals) return NTG_ERR_ALLOC_FAIL;

    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = ntg_xy_new(0, 0);
    }

    return 0;
}

static int tmp_draw_init(
        struct ntg_widget_tmp_draw* drawing,
        ntg_xy size,
        struct ntg_vcell base_bg,
        sarena* arena)
{
    if(!drawing || !arena)
        return NTG_ERR_INV_ARG;

    drawing->data = NULL;
    drawing->size = ntg_xy_new(0, 0);

    drawing->data = sarena_malloc(arena, 1 +
        sizeof(struct ntg_vcell) * size.ro.x * size.ro.y);
    if(!drawing->data) return NTG_ERR_ALLOC_FAIL;
    drawing->size = size;

    size_t i, j;
    for(i = 0; i < size.ro.y; i++)
    {
        for(j = 0; j < size.ro.x; j++)
            ntg_widget_tmp_draw_set(drawing, base_bg, ntg_xy_new(j, i));
        
    }

    return 0;
}

static void layout_reset(ntg_widget* widget)
{
    if(!widget) return;

    widget->ro.min_size = ntg_xy_new(0, 0);
    widget->ro.nat_size = ntg_xy_new(0, 0);
    widget->ro.max_size = ntg_xy_new(0, 0);
    widget->ro.grow = ntg_xy_new(0, 0);
    widget->priv.skip_hborder = false;
    ntg__widget_clean(widget, NTG_WIDGET_DIRTY_FULL | NTG__WIDGET_DIRTY_RENDER);
    widget->priv.skip_hpadding = false;
    widget->priv.special_repeat = false;
    widget->ro.size = ntg_xy_new(0, 0);
    widget->ro.border_size = ntg_insets_new(0, 0, 0, 0);
    widget->ro.padding_size = ntg_insets_new(0, 0, 0, 0);
    widget->priv.old_size = ntg_xy_new(0, 0);
    widget->priv.old_pos = ntg_xy_new(0, 0);
    widget->priv.old_cont_size = ntg_xy_new(0, 0);

    ntg_widget_draw_set_size(&widget->ro.drawing, ntg_xy_new(0, 0));

    if(widget->priv.layout_dt && widget->priv.layout_dt->reset_fn)
        widget->priv.layout_dt->reset_fn(widget->priv.layout_dt);
}

static bool set_hmeasure_helper(ntg_widget* widget, struct ntg_widget_measure measure)
{
    struct ntg_widget_measure old = ntg_widget_get_measure(widget, NTG_ORIENT_H);

    if(!ntg_widget_measure_are_eql(measure, old))
    {
        widget->ro.min_size = ntg_xy_set_x(widget->ro.min_size, measure.min_size);
        widget->ro.nat_size = ntg_xy_set_x(widget->ro.nat_size, measure.nat_size);
        widget->ro.max_size = ntg_xy_set_x(widget->ro.max_size, measure.max_size);
        widget->ro.grow = ntg_xy_set_x(widget->ro.grow, measure.grow);
        ntg_widget_mark_dirty(widget,
                NTG_WIDGET_DIRTY_HCONSTRAIN |
                NTG_WIDGET_DIRTY_VMEASURE);
        if(widget->ro.parent)
        {
            ntg_widget_mark_dirty(widget->ro.parent,
                    NTG_WIDGET_DIRTY_MEASURE |
                    NTG_WIDGET_DIRTY_CONSTRAIN);
        }
        return true;
    }

    return false;
}

static bool set_vmeasure_helper(ntg_widget* widget, struct ntg_widget_measure measure)
{
    struct ntg_widget_measure old = ntg_widget_get_measure(widget, NTG_ORIENT_V);

    if(!ntg_widget_measure_are_eql(measure, old))
    {
        widget->ro.min_size = ntg_xy_set_y(widget->ro.min_size, measure.min_size);
        widget->ro.nat_size = ntg_xy_set_y(widget->ro.nat_size, measure.nat_size);
        widget->ro.max_size = ntg_xy_set_y(widget->ro.max_size, measure.max_size);
        widget->ro.grow = ntg_xy_set_y(widget->ro.grow, measure.grow);

        ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_VCONSTRAIN);
        if(widget->ro.parent)
        {
            ntg_widget_mark_dirty(widget->ro.parent,
                    NTG_WIDGET_DIRTY_VMEASURE |
                    NTG_WIDGET_DIRTY_VCONSTRAIN);
        }

        return true;
    }
    return false;
}

static bool set_hsize_helper(ntg_widget* widget, size_t size)
{
    if(widget->ro.size.ro.x != size)
    {
        widget->ro.size = ntg_xy_set_x(widget->ro.size, size);
        ntg_widget_mark_dirty(widget, 
                NTG_WIDGET_DIRTY_HCONSTRAIN |
                NTG_WIDGET_DIRTY_VMEASURE |
                NTG_WIDGET_DIRTY_VCONSTRAIN);
        widget->priv.skip_hborder = false;
        widget->priv.skip_hpadding = false;
        return true;
    }
    else return false;
}

static bool set_vsize_helper(ntg_widget* widget, size_t size)
{
    if(widget->ro.size.ro.x == 0)
        size = 0;

    if(widget->ro.size.ro.y != size)
    {
        widget->ro.size = ntg_xy_set_y(widget->ro.size, size);

        bool hborder_missing = 
            (ntg_insets_hsum(widget->ro.bdr_opts.pref_size) > 0) &&
            (ntg_insets_hsum(widget->ro.border_size) == 0);
        bool hpadding_missing = 
            (ntg_insets_hsum(widget->ro.pad_opts.pref_size) > 0) &&
            (ntg_insets_hsum(widget->ro.padding_size) == 0);

        if(hborder_missing || hpadding_missing)
        {
            widget->priv.skip_hborder = false;
            widget->priv.skip_hpadding = false;
            widget->priv.special_repeat = true;
            ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_VCONSTRAIN);
        }
        else
        {
            ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_VCONSTRAIN);
        }
        return true;
    }
    else return false;
}

static inline bool set_pos_helper(ntg_widget* widget, ntg_xy pos)
{
    if(ntg_xy_is_zero_any(widget->ro.size))
        pos = NTG_WIDGET_POS_NODRAW;

    if(!ntg_xy_are_eql(widget->ro.pos, pos))
    {
        widget->ro.pos = pos;
        return true;
    }
    else return false;
}

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static struct ntg_widget_measure incorporate_lay_conf_measure(
        struct ntg_widget_measure measure,
        size_t conf_min_size,
        size_t conf_max_size,
        size_t conf_grow)
{
    conf_min_size = ntg_min2_size(conf_min_size, conf_max_size);
 
    if(conf_max_size < measure.min_size)
        measure.min_size = conf_max_size;
    else
        measure.min_size = ntg_max2_size(measure.min_size, conf_min_size);
 
    if(conf_min_size > measure.max_size)
        measure.max_size = conf_min_size;
    else
        measure.max_size = ntg_min2_size(measure.max_size, conf_max_size);
 
    measure.nat_size = ntg_clamp_size(
            measure.min_size,
            measure.nat_size,
            measure.max_size);
 
    measure.grow = conf_grow;

    return measure;
}

static int get_dcr_size(
        enum ntg_widget_dcr_enable enable,
        size_t pref_size[2],
        size_t size,
        struct ntg_widget_measure inner_measure,
        size_t out_size[2])
{
    size_t extra;

    if(enable == NTG_WIDGET_DCR_ENABLE_MIN)
        extra = ntg_sub2_size(size, inner_measure.min_size);
    else if(enable == NTG_WIDGET_DCR_ENABLE_NAT)
        extra = ntg_sub2_size(size, inner_measure.nat_size);
    else extra = size;

    out_size[0] = 0;
    out_size[1] = 0;
    double scratch_buffer[2];
    return ntg_sap_cap_round_robin(pref_size, NULL, extra, 2,
                                   scratch_buffer, out_size, NULL);
}

static int calculate_border_hsize(
        ntg_widget* widget, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = widget->ro.bdr_opts.pref_size.ro.w;
    we_pref_size[1] = widget->ro.bdr_opts.pref_size.ro.e;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            widget->ro.bdr_opts.enable,
            we_pref_size,
            widget->ro.size.ro.x,
            ntg_widget_get_measure_pad(widget, NTG_ORIENT_H),
            _sizes);
    if(status != 0)
        return status;

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
    return 0;
}

static int vconstrain_border(ntg_widget* widget, bool* out_repeat)
{
    ntg_insets pref_border_size = widget->ro.bdr_opts.pref_size;

    bool hborder_missing = 
            (ntg_insets_hsum(pref_border_size) > 0) &&
            (ntg_insets_hsum(widget->ro.border_size) == 0);

    if(!out_repeat) return NTG_ERR_INV_ARG;
    *out_repeat = false;

    size_t n = 0, s = 0;
    int status = calculate_border_vsize(widget, &n, &s);
    if(status != 0) return status;

    widget->ro.border_size = ntg_insets_set(widget->ro.border_size, n, NTG_DIR_N);
    widget->ro.border_size = ntg_insets_set(widget->ro.border_size, s, NTG_DIR_S);

    bool vborder_missing = 
            (ntg_insets_vsum(pref_border_size) > 0) &&
            (ntg_insets_vsum(widget->ro.border_size) == 0);

    if(hborder_missing)
    {
        if(widget->priv.skip_hborder) // Missing cuz skipped
        {
            // widget->priv.skip_hborder = false;
            if(vborder_missing) // Vborder missing
                return 0;
            else // Vborder present
            {
                widget->ro.border_size = ntg_insets_set(widget->ro.border_size, 0, NTG_DIR_N);
                widget->ro.border_size = ntg_insets_set(widget->ro.border_size, 0, NTG_DIR_S);
                return 0;
            }
        }
        else
        {
            if(vborder_missing) // Vborder missing
                return 0;
            else // Vborder present
            {
                widget->ro.border_size = ntg_insets_set(widget->ro.border_size, 0, NTG_DIR_N);
                widget->ro.border_size = ntg_insets_set(widget->ro.border_size, 0, NTG_DIR_S);
                if(widget->priv.special_repeat)
                {
                    widget->priv.special_repeat = false;
                    *out_repeat = true;
                    return 0;
                }
                else return 0;
            }
        }
    }
    else
    {
        if(widget->priv.skip_hborder)  // INVALID STATE
            assert(0);
        else
        {
            if(vborder_missing)
            {
                widget->priv.skip_hborder = true;
                *out_repeat = true;
                    return 0;
            }
            else return 0;
        }
    }
}

static int vconstrain_padding(ntg_widget* widget, bool* out_repeat)
{
    ntg_insets pref_padding_size = widget->ro.pad_opts.pref_size;

    bool hpadding_missing = 
            (ntg_insets_hsum(pref_padding_size) > 0) &&
            (ntg_insets_hsum(widget->ro.padding_size) == 0);

    if(!out_repeat) return NTG_ERR_INV_ARG;
    *out_repeat = false;

    size_t n = 0, s = 0;
    int status = calculate_padding_vsize(widget, &n, &s);
    if(status != 0) return status;

    widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, n, NTG_DIR_N);
    widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, s, NTG_DIR_S);

    bool vpadding_missing = 
            (ntg_insets_vsum(pref_padding_size) > 0) &&
            (ntg_insets_vsum(widget->ro.padding_size) == 0);

    if(hpadding_missing)
    {
        if(widget->priv.skip_hpadding) // Missing cuz skipped
        {
            // widget->priv.skip_hpadding = false;
            if(vpadding_missing) // Vpadding missing
                return 0;
            else // Vpadding present
            {
                widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, 0, NTG_DIR_N);
                widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, 0, NTG_DIR_S);
                return 0;
            }
        }
        else
        {
            if(vpadding_missing) // Vpadding missing
                return 0;
            else // Vpadding present
            {
                widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, 0, NTG_DIR_N);
                widget->ro.padding_size = ntg_insets_set(widget->ro.padding_size, 0, NTG_DIR_S);
                if(widget->priv.special_repeat)
                {
                    widget->priv.special_repeat = false;
                    *out_repeat = true;
                    return 0;
                }
                else return 0;
            }
        }
    }
    else
    {
        if(widget->priv.skip_hpadding) // INVALID STATE
            assert(0);
        else
        {
            if(vpadding_missing)
            {
                widget->priv.skip_hpadding = true;
                *out_repeat = true;
                return 0;
            }
            else return 0;
        }
    }
}

static int calculate_border_vsize(ntg_widget* widget, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = widget->ro.bdr_opts.pref_size.ro.n;
    ns_pref_size[1] = widget->ro.bdr_opts.pref_size.ro.s;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            widget->ro.bdr_opts.enable,
            ns_pref_size, 
            widget->ro.size.ro.y,
            ntg_widget_get_measure_pad(widget, NTG_ORIENT_V),
            _sizes);
    if(status != 0) return status;

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
    return 0;
}

static int calculate_padding_hsize(ntg_widget* widget, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = widget->ro.pad_opts.pref_size.ro.w;
    we_pref_size[1] = widget->ro.pad_opts.pref_size.ro.e;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            widget->ro.pad_opts.enable,
            we_pref_size,  
            ntg_widget_get_size_pad(widget).ro.x,
            ntg_widget_get_measure_cont(widget, NTG_ORIENT_H),
            _sizes);
    if(status != 0) return status;

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
    return 0;
}

static int calculate_padding_vsize(ntg_widget* widget, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = widget->ro.pad_opts.pref_size.ro.n;
    ns_pref_size[1] = widget->ro.pad_opts.pref_size.ro.s;

    size_t _sizes[2] = {0, 0};

     int status = get_dcr_size(
             widget->ro.pad_opts.enable,
             ns_pref_size,  
             ntg_widget_get_size_pad(widget).ro.y,
             ntg_widget_get_measure_cont(widget, NTG_ORIENT_V),
             _sizes);
    if(status != 0) return status;

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
    return 0;
}
static int draw_optimized(ntg_widget* widget, sarena* arena)
{
    ntg_xy content_size = ntg_widget_get_size_cont(widget);
    ntg_xy object_size = widget->ro.drawing.ro.size;

    struct ntg_vcell bg = widget->priv.base_bg;
    ntg_insets psize = widget->ro.padding_size;

    struct ntg_widget_tmp_draw content_drawing;
    int _status = tmp_draw_init(&content_drawing, content_size, bg, arena);
    if(_status != 0)
        return _status;

    if(ntg_wgt_vtbl(widget)->draw_fn)
    {
        _status = ntg_wgt_vtbl(widget)->draw_fn(
                widget,
                widget->priv.layout_dt,
                &content_drawing,
                arena);
        if(_status != 0) return _status;
    }

    struct ntg_vcell it_src_cell;

    ntg_xy offset = ntg_xy_new(psize.ro.w, psize.ro.n);

    size_t i, j;
    ntg_xy ji;
    for(i = 0; i < object_size.ro.y; i++)
    {
        for(j = 0; j < object_size.ro.x; j++)
        {
            ji = ntg_xy_new(j, i);
            ntg_widget_draw_set(&widget->ro.drawing, bg, ji);
        }
    }

    for(i = 0; i < content_size.ro.y; i++)
    {
        for(j = 0; j < content_size.ro.x; j++)
        {
            ji = ntg_xy_new(j, i);

            it_src_cell = ntg_widget_tmp_draw_get(&content_drawing, ji);
            ntg_widget_draw_set(&widget->ro.drawing, it_src_cell, ntg_xy_add(offset, ji));
        }
    }
    return 0;
}

static int 
draw_unoptimized(ntg_widget* widget, sarena* arena)
{
    ntg_xy content_size = ntg_widget_get_size_cont(widget);
    ntg_xy object_size = widget->ro.drawing.ro.size;

    struct ntg_vcell bg = widget->priv.base_bg;
    ntg_insets bsize = widget->ro.border_size;
    ntg_insets psize = widget->ro.padding_size;
    const struct ntg_border_style* border_style = widget->ro.bdr_opts.style;

    struct ntg_widget_tmp_draw content_drawing;
    int _status = tmp_draw_init(&content_drawing, content_size, bg, arena);
    if(_status != 0)
        return _status;

    struct ntg_widget_tmp_draw object_drawing;
    _status = tmp_draw_init(&object_drawing, object_size, bg, arena);
    if(_status != 0)
        return _status;

    ntg__border_style_draw(border_style, object_size, bsize, &object_drawing);

    size_t i, j;

    for(i = bsize.ro.n; i < (object_size.ro.y - bsize.ro.s); i++)
    {
        for(j = bsize.ro.w; j < (object_size.ro.x - bsize.ro.e); j++)
        {
            ntg_widget_tmp_draw_set(&object_drawing, bg, ntg_xy_new(j, i));
        }
    }

    if(ntg_wgt_vtbl(widget)->draw_fn)
    {
        _status = ntg_wgt_vtbl(widget)->draw_fn(
                widget,
                widget->priv.layout_dt,
                &content_drawing,
                arena);

        if(_status != 0)
            return _status;
    }

    struct ntg_vcell it_src_cell;

    ntg_xy offset = ntg_xy_new(bsize.ro.w + psize.ro.w, bsize.ro.n + psize.ro.n);
    ntg_xy ji;
    for(i = 0; i < content_size.ro.y; i++)
    {
        for(j = 0; j < content_size.ro.x; j++)
        {
            ji = ntg_xy_new(j, i);

            it_src_cell = ntg_widget_tmp_draw_get(&content_drawing, ji);

            ntg_widget_tmp_draw_set(
                    &object_drawing,
                    it_src_cell,
                    ntg_xy_add(offset, ji));
        }
    }

    for(i = 0; i < object_size.ro.y; i++)
    {
        for(j = 0; j < object_size.ro.x; j++)
        {
            ji = ntg_xy_new(j, i);

            it_src_cell = ntg_widget_tmp_draw_get(&object_drawing, ji);
            ntg_widget_draw_set(&widget->ro.drawing, it_src_cell, ji);
        }
    }

    return 0;
}
