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

struct ntg_object_size_map
{
    ntg_object** keys;
    size_t* vals;

    size_t size;
};

struct ntg_object_pos_map
{
    ntg_object** keys;
    struct ntg_xy* vals;

    size_t size;
};

enum ntg_object_repeat_flag
{
    NTG_OBJECT_SKIP_HBORDER = (1u << 0),
    NTG_OBJECT_SKIP_HPADDING = (1u << 1),
    NTG_OBJECT_SPECIAL_REPEAT = (1u << 2)
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* LAYOUT OBJECT INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_object_size_map* map,
        const struct ntg_objptr_vec* children,
        sarena* arena);

static int pos_map_init(
        ntg_object_pos_map* map,
        const struct ntg_objptr_vec* children,
        sarena* arena);

static int tmp_draw_init(
        struct ntg_object_tmp_draw* drawing,
        struct ntg_xy size,
        struct ntg_vcell base_bg,
        sarena* arena);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static struct ntg_object_measure incorporate_user_measure(
        struct ntg_object_measure measure,
        size_t user_min_size,
        size_t user_max_size,
        size_t user_grow);
    
static int get_dcr_size(
        enum ntg_object_dcr_enable enable,
        size_t we_pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t we_out_size[2]);

static int vconstrain_border(ntg_object* object, bool* out_repeat);
static int vconstrain_padding(ntg_object* object, bool* out_repeat);
static int calculate_border_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e);
static int calculate_border_vsize(
        ntg_object* object, size_t* out_n, size_t* out_s);
static int calculate_padding_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e);
static int calculate_padding_vsize(
        ntg_object* object, size_t* out_n, size_t* out_s);

static int draw_optimized(ntg_object* object, sarena* arena);
static int draw_unoptimized(ntg_object* object, sarena* arena);
static void layout_reset(ntg_object* object);
static bool set_hmeasure_helper(ntg_object* object, struct ntg_object_measure measure);
static bool set_vmeasure_helper(ntg_object* object, struct ntg_object_measure measure);
static bool set_hsize_helper(ntg_object* object, size_t size);
static bool set_vsize_helper(ntg_object* object, size_t size);
static inline bool set_pos_helper(ntg_object* object, struct ntg_xy pos);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_bdr_opts ntg_bdr_opts_default(void)
{
    return (struct ntg_bdr_opts) {
        .style = &NTG_BORDER_STYLE_DEFAULT,
        .pref_size = ntg_insets(0, 0, 0, 0),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN
    };
}

bool ntg_bdr_opts_are_eql(
        const struct ntg_bdr_opts* opts1,
        const struct ntg_bdr_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->enable == opts2->enable) &&
            (ntg_insets_are_eql(opts1->pref_size, opts2->pref_size)) &&
            (opts1->style == opts2->style));
}

struct ntg_pad_opts ntg_padding_opts_default(void)
{
    return (struct ntg_pad_opts) {
        .pref_size = ntg_insets(0, 0, 0, 0),
        .enable = NTG_OBJECT_DCR_ENABLE_MIN
    };
}

bool ntg_pad_opts_are_eql(
        const struct ntg_pad_opts* opts1,
        const struct ntg_pad_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->enable == opts2->enable) &&
    ntg_insets_are_eql(opts1->pref_size, opts2->pref_size));
}

struct ntg_lay_opts ntg_lay_opts_default(void)
{
    struct ntg_xy min_size, max_size, grow;
    min_size = ntg_xy(NTG_OBJECT_MIN_SIZE_UNSET, NTG_OBJECT_MIN_SIZE_UNSET);
    max_size = ntg_xy(NTG_OBJECT_MAX_SIZE_UNSET, NTG_OBJECT_MAX_SIZE_UNSET);
    grow = ntg_xy(NTG_OBJECT_GROW_UNSET, NTG_OBJECT_GROW_UNSET);
    return (struct ntg_lay_opts) {
        .min_cont_size = min_size,
        .max_cont_size = max_size,
        .grow = grow,
        .z_index = NTG_OBJECT_Z_INDEX_UNSET
    };
}

bool ntg_layout_opts_are_eql(
        const struct ntg_lay_opts* opts1,
        const struct ntg_lay_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return (ntg_xy_are_eql(opts1->min_cont_size, opts2->min_cont_size) &&
            ntg_xy_are_eql(opts1->max_cont_size, opts2->max_cont_size) &&
            ntg_xy_are_eql(opts1->grow, opts2->grow) &&
            opts1->z_index == opts2->z_index);
}

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

void ntg_object_vdeinit(ntg_object* object)
{
    if(!object) return;

    if(object->priv.vtable->deinit_fn)
        object->priv.vtable->deinit_fn(object);
}

/* ------------------------------------------------------ */

bool ntg_object_feed_key(ntg_object* object, const struct ntg_object_key* event)
{
    if(!object) return false;
    if(!event) return false;
    if(!event->target) return false;
    if(nt_key_are_eql(event->key, NT_KEY_ZERO)) return false;

    bool consumed = false;

    if(object->priv.vtable->handle_key_fn)
    {
        consumed = object->priv.vtable->handle_key_fn(object, event);
    }

    struct ntg_event_object_key_dt event_dt = { .key = event };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_KEY, object, &event_dt));

    return consumed;
}

bool ntg_object_feed_mouse(ntg_object* object, const struct ntg_object_mouse* event)
{
    if(!object) return false;
    if(!object->ro.clickable) return false;
    if(!event) return false;
    if(!event->target) return false;
    if(nt_mouse_are_eql(event->mouse, NT_MOUSE_ZERO)) return false;

    struct ntg_xy size = ntg_object_get_size(object);
    if((event->mouse.x >= size.x) || (event->mouse.y >= size.y))
        return false;

    struct ntg_xy pos = ntg_xy(event->mouse.x, event->mouse.y);
    struct ntg_object_hit_res res;
    res = ntg_object_hit_test(object, pos);

    if(!(object->ro.clickable == NTG_OBJECT_CLICKABLE_BDR) &&
        (res.part == NTG_OBJECT_HIT_BDR))
    {
        return false;
    }

    bool consumed = false;

    if(object->priv.vtable->handle_mouse_fn)
    {
        consumed = object->priv.vtable->handle_mouse_fn(object, event);
    }

    struct ntg_event_object_mouse_dt event_dt = { .mouse = event };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_MOUSE, object, &event_dt));

    return consumed;
}

/* ------------------------------------------------------ */

int ntg_object_set_lay_opts(ntg_object* object, const struct ntg_lay_opts* opts_cp)
{
    if(!object) return NTG_ERR_INV_ARG;

    struct ntg_lay_opts old_opts = object->ro.layout_opts;
    struct ntg_lay_opts new_opts = (opts_cp ? (*opts_cp) : ntg_lay_opts_default());

    if(ntg_layout_opts_are_eql(&old_opts, &new_opts))
        return 0;

    object->ro.layout_opts = new_opts;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->priv.vtable->chng_lay_opts_fn)
    {
        object->priv.vtable->chng_lay_opts_fn(
                object,
                &old_opts,
                &object->ro.layout_opts);
    }

    struct ntg_event_object_layoptchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &object->ro.layout_opts
    };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_LAYOPTCHG, object, &event_dt));

    return 0;
}

int ntg_object_set_bdr_opts(ntg_object* object, const struct ntg_bdr_opts* opts_cp)
{
    if(!object) return NTG_ERR_INV_ARG;

    struct ntg_bdr_opts old_opts = object->ro.border_opts;
    struct ntg_bdr_opts new_opts = (opts_cp ? (*opts_cp) : ntg_bdr_opts_default());

    if(ntg_bdr_opts_are_eql(&old_opts, &new_opts))
        return 0;

    object->ro.border_opts = new_opts;
    if(!object->ro.border_opts.style)
        object->ro.border_opts.style = &NTG_BORDER_STYLE_DEFAULT;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->priv.vtable->chng_bdr_opts_fn)
    {
        object->priv.vtable->chng_bdr_opts_fn(
                object,
                &old_opts,
                &object->ro.border_opts);
    }

    struct ntg_event_object_bdroptchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &object->ro.border_opts
    };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_BDROPTCHG, object, &event_dt));

    return 0;
}

int ntg_object_set_pad_opts(ntg_object* object, const struct ntg_pad_opts* opts_cp)
{
    if(!object) return NTG_ERR_INV_ARG;

    struct ntg_pad_opts old_opts = object->ro.padding_opts;
    struct ntg_pad_opts new_opts = (opts_cp ? (*opts_cp) : ntg_padding_opts_default());

    if(ntg_pad_opts_are_eql(&old_opts, &new_opts))
        return 0;

    object->ro.padding_opts = new_opts;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->priv.vtable->chng_pad_opts_fn)
    {
        object->priv.vtable->chng_pad_opts_fn(
                object,
                &old_opts,
                &object->ro.padding_opts);
    }

    struct ntg_event_object_padoptchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &object->ro.padding_opts
    };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_PADOPTCHG, object, &event_dt));

    return 0;
}

int ntg_object_set_anchor_policy(ntg_object* object, const ntg_anchor_policy* policy)
{
    ntg_not_null(object);

    object->ro.anchor_policy = policy;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    return 0;
}

/* ------------------------------------------------------ */

NTG_API struct ntg_object_hit_res
ntg_object_hit_test(ntg_object* object, struct ntg_xy pos)
{
    struct ntg_object_hit_res out = {0};

    if(!object) return out;

    /* If not hit, return NULL */

    if(!ntg_xy_is_in_rectangle(pos, ntg_xy(0, 0), object->ro.size))
        return out;

    /* Set `out_local_pos`, ancestors will choose the right one later */

    out.object = object;
    out.local_pos = pos;

    /* Set `out_hit_res`, ancestors will choose the right one later */

    struct ntg_insets padding_size = object->ro.padding_size;
    struct ntg_insets border_size = object->ro.border_size;
    struct ntg_xy cont_size = ntg_object_get_size_cont(object);

    if((pos.x > (padding_size.w + cont_size.x + padding_size.e)) ||
       (pos.x < border_size.w) ||
       (pos.y > (padding_size.n + cont_size.y + padding_size.s)) ||
       (pos.y < border_size.n))
    {
        out.part = NTG_OBJECT_HIT_BDR;
    }
    else if((pos.x >= cont_size.x) ||
            (pos.x < (border_size.w + padding_size.w)) ||
            (pos.y >= cont_size.y) ||
            (pos.y < (border_size.n + padding_size.n)))
    {
        out.part = NTG_OBJECT_HIT_PAD;
    }
    else
    {
        out.part = NTG_OBJECT_HIT_CONT;
    }

    size_t i;
    ntg_object* it_child;
    struct ntg_object_hit_res it_out;
    struct ntg_dxy it_adj_pos;
    int curr_z = INT_MIN;
    for(i = 0; i < object->ro.children.size; i++)
    {
        it_child = object->ro.children.data[i];
        it_adj_pos = ntg_dxy_sub(ntg_dxy_from_xy(pos), ntg_dxy_from_xy(it_child->ro.pos));

        it_out = ntg_object_hit_test(it_child, ntg_xy_from_dxy(it_adj_pos));

        if(it_out.object && (it_child->ro.layout_opts.z_index > curr_z))
        {
            out = it_out;
            curr_z = it_child->ro.layout_opts.z_index;
        }
    }

    return out;
}

/* ------------------------------------------------------ */

bool ntg_object_is_focused(const ntg_object* object)
{
    if(!object) return false;

    const ntg_scene* scene = ntg_object_get_scene(object);
    if(!scene) return false;

    return (scene->ro.fm->ro.focused == object);
}

/* ------------------------------------------------------ */
/* OBJECT GRAPH OPERATIONS */
/* ------------------------------------------------------ */

int ntg_object_detach(ntg_object* object)
{
    if(!object) return NTG_ERR_INV_ARG;

    ntg_object* parent = object->ro.parent;
    if(parent == NULL) return 0;

    ntg_scene* scene = ntg_object_get_scene_(object);

    ntg_objptr_vec_rm(&parent->ro.children, object);

    object->ro.parent = NULL;

    if(parent->priv.vtable->rm_child_fn)
        parent->priv.vtable->rm_child_fn(parent, object);

    if(scene)
        ntg__scene_rm_object_tree(scene, object);

    ntg_object_mark_dirty(parent, NTG_OBJECT_DIRTY_FULL);

    if(object->priv.vtable->rm_parent_fn)
        object->priv.vtable->rm_parent_fn(object, parent);

    struct ntg_event_object_chldrm_dt chldrm_dt = { .child = object };
    struct ntg_event_object_prntrm_dt prntrm_dt = { .parent = parent };

    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_PRNTRM, object, &prntrm_dt));
            
    ntg_event_raise(
            &parent->ro.event_dlgt,
           ntg_event_new(NTG_EVENT_OBJECT_CHLDRM, parent, &chldrm_dt));

    if(scene)
        ntg__scene_on_rm_object_tree(scene, object);

    return 0;
}

int ntg_object_anchor(ntg_object* base, ntg_object* root)
{
    int _status;

    if(!base || !root || (base == root))
        return NTG_ERR_INV_ARG;

    if(base->ro.anchored.size >= NTG_OBJECT_MAX_ANCHORED)
        return NTG_ERR_MAX_ANCHORED;

    if(root->ro.parent)
    {
        ntg_object_detach(root);
    }

    if(root->ro.base)
    {
        ntg_object_unanchor(root);
    }

    _status = ntg_objptr_vec_pushb(&base->ro.anchored, root);
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

    ntg_scene* scene = ntg_object_get_scene_(root);

    if(base->priv.vtable->add_anchored_fn)
        base->priv.vtable->add_anchored_fn(base, root);

    if(root->priv.vtable->set_base_fn)
        root->priv.vtable->set_base_fn(root, base);

    if(scene)
        ntg__scene_add_object_tree(scene, root);

    struct ntg_event_object_anchadd_dt anchadd_dt = { .anchored = root };
    ntg_event_raise(
            &base->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_ANCHADD, base, &anchadd_dt));

    struct ntg_event_object_bsset_dt bsset_dt = { .base = base };
    ntg_event_raise(
            &root->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_BSSET, root, &bsset_dt));

    if(scene)
        ntg__scene_on_add_object_tree(scene, root);

    return 0;
}

int ntg_object_unanchor(ntg_object* root)
{
    if(!root) return NTG_ERR_INV_ARG;
    if(!root->ro.base) return 0;

    ntg_object* base = root->ro.base;

    ntg_scene* scene = ntg_object_get_scene_(root);

    ntg_objptr_vec_rm(&root->ro.base->ro.anchored, root);
    root->ro.base = NULL;
    root->ro.anchor_policy = NULL;

    if(base->priv.vtable->rm_anchored_fn)
        base->priv.vtable->rm_anchored_fn(base, root);

    if(scene)
        ntg__scene_rm_object_tree(scene, root);

    if(root->priv.vtable->rm_base_fn)
        root->priv.vtable->rm_base_fn(root, base);

    struct ntg_event_object_anchrm_dt anchrm_dt = { .anchored = root };
    ntg_event_raise(
            &base->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_ANCHRM, base, &anchrm_dt));

    struct ntg_event_object_bsrm_dt bsrm_dt = { .base = base };
    ntg_event_raise(
            &root->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_BSRM, root, &bsrm_dt));

    if(scene)
        ntg__scene_on_rm_object_tree(scene, root);

    return 0;
}

int ntg_object_remove_from_scene(ntg_object* object)
{
    if(!object) return NTG_ERR_INV_ARG;

    ntg_object* parent = object->ro.parent;
    if(parent)
    {
        ntg_object_detach(object);
        return 0;
    }

    ntg_object* base = object->ro.base;
    if(base)
    {
        ntg_object_unanchor(object);
        return 0;
    }

    if(ntg_object_is_true_root(object))
    {
        ntg_scene_rm_root(ntg_object_get_scene_(object), object);
        return 0;
    }

    return 0;
}

/* ------------------------------------------------------ */
/* OBJECT GRAPH QUERY */
/* ------------------------------------------------------ */

const ntg_object* ntg_object_get_graph_root(const ntg_object* object)
{
    if(!object) return NULL;

    const ntg_object *it_obj = object, *it_root, *it_base;
    while(true)
    {
        it_root = ntg_object_get_tree_root(it_obj);
        it_base = it_root->ro.base;
        it_obj = it_base;

        if(!it_base) break;
    }

    return it_root;
}

ntg_object* ntg_object_get_graph_root_(ntg_object* object)
{
    if(!object) return NULL;

    return (ntg_object*)ntg_object_get_graph_root(object);
}

/* ------------------------------------------------------ */

const ntg_object* ntg_object_get_tree_root(const ntg_object* object)
{
    if(!object) return NULL;

    while(object->ro.parent) object = object->ro.parent;
    return object;
}

ntg_object* ntg_object_get_tree_root_(ntg_object* object)
{
    if(!object) return NULL;

    return (ntg_object*)ntg_object_get_tree_root(object);
}

/* ------------------------------------------------------ */

ntg_scene* ntg_object_get_scene_(ntg_object* object)
{
    if(!object) return NULL;

    ntg_object* root = ntg_object_get_graph_root_(object);

    return root->priv.scene;
}

const ntg_scene* ntg_object_get_scene(const ntg_object* object)
{
    if(!object) return NULL;

    const ntg_object* root = ntg_object_get_graph_root(object);

    return root->priv.scene;
}

/* ------------------------------------------------------ */

bool ntg_object_is_true_root(const ntg_object* object)
{
    if(!object) return false;
    
    return (ntg_object_is_graph_root(object) && ntg_object_get_scene(object));
}

bool ntg_object_is_graph_root(const ntg_object* object)
{
    if(!object) return false;

    return ((!object->ro.parent) && (!object->ro.base));
}

bool ntg_object_is_tree_root(const ntg_object* object)
{
    if(!object) return false;

    return (!object->ro.parent);
}

/* ------------------------------------------------------ */

bool ntg_object_is_in_tree(const ntg_object* tree_root, const ntg_object* desc)
{
    if(!tree_root || !desc) return false;

    const ntg_object* it = desc;
    while(it)
    {
        if(it == tree_root) return true;
        it = it->ro.parent;
    }

    return false;
}

bool ntg_object_is_in_graph(const ntg_object* graph_root, const ntg_object* desc)
{
    if(!graph_root || !desc) return false;

    const ntg_object* it = desc;
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

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->ro.size;
}

struct ntg_xy ntg_object_get_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets border_size = object->ro.border_size;
    struct ntg_insets padding_size = object->ro.padding_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(border_size) + ntg_insets_hsum(padding_size),
        ntg_insets_vsum(border_size) + ntg_insets_vsum(padding_size)
    );
    return ntg_xy_sub(object->ro.size, sub);
}

struct ntg_xy ntg_object_get_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets border_size = object->ro.border_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(border_size),
            ntg_insets_vsum(border_size)
    );
    return ntg_xy_sub(object->ro.size, sub);
}

struct ntg_xy ntg_object_get_abs_pos(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    return ntg_xy_from_dxy(ntg_object_map_to_scene(object, ntg_dxy(0, 0)));
}

/* ------------------------------------------------------ */
/* SPACE MAPPING */
/* ------------------------------------------------------ */

struct ntg_dxy ntg_object_map_to_ancs(
        const ntg_object* object,
        const ntg_object* ancs,
        struct ntg_dxy point)
{
    if(!object) return NTG_DXY_MAX;

    if(object == ancs)
        return point;

    struct ntg_dxy out = point;

    const ntg_object* it = object;
    while(it && (it != ancs))
    {
        out = ntg_dxy_add(out, ntg_dxy_from_xy(it->ro.pos));
        it = it->ro.parent;
    }

    return out;
}

struct ntg_dxy ntg_object_map_to_desc(
        const ntg_object* object,
        const ntg_object* desc,
        struct ntg_dxy point)
{
    if(!desc) return NTG_DXY_MAX;

    struct ntg_dxy desc_pos = ntg_object_map_to_ancs(
            desc, object, ntg_dxy(0, 0));

    return ntg_dxy_sub(point, desc_pos);
}

struct ntg_dxy 
ntg_object_map_to_scene(const ntg_object* object, struct ntg_dxy point)
{
    return ntg_object_map_to_ancs(object, NULL, point);
}

struct ntg_dxy 
ntg_object_map_from_scene(const ntg_object* object, struct ntg_dxy point)
{
    if(!object)
        return NTG_DXY_MAX;

    return ntg_object_map_to_desc(NULL, object, point);
}

/* ------------------------------------------------------ */
/* COLLECT */
/* ------------------------------------------------------ */

struct count_data
{
    ntg_object** out; // non-null
    size_t cap;
    size_t count;
};

static inline void collect_fn(ntg_object* object, void* _data)
{
    if(!object) return;

    struct count_data* data = _data;

    if(data->out && (data->count < data->cap))
        data->out[data->count] = object;        

    data->count++;
}

static inline void collect_root_fn(ntg_object* object, void* _data)
{
    if(!object) return;
    if(!ntg_object_is_tree_root(object)) return;

    struct count_data* data = _data;

    if(data->out && (data->count < data->cap))
        data->out[data->count] = object;        

    data->count++;
}

NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(pre_collect_tree, collect_fn)
NTG_OBJECT_TREE_DEF_TRAVERSE_POST(post_collect_tree, collect_fn)
NTG_OBJECT_GRAPH_DEF_TRAVERSE_PRE(pre_collect_graph, collect_fn)
NTG_OBJECT_GRAPH_DEF_TRAVERSE_POST(post_collect_graph, collect_fn)
NTG_OBJECT_GRAPH_DEF_TRAVERSE_PRE(pre_collect_roots_graph, collect_root_fn)
NTG_OBJECT_GRAPH_DEF_TRAVERSE_POST(post_collect_roots_graph, collect_root_fn)

size_t ntg_object_tree_collect_pre(ntg_object* root, ntg_object** out, size_t cap)
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

size_t ntg_object_tree_collect_post(ntg_object* root, ntg_object** out, size_t cap)
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

size_t ntg_object_graph_collect_pre(ntg_object* root, ntg_object** out, size_t cap)
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

size_t ntg_object_graph_collect_post(ntg_object* root, ntg_object** out, size_t cap)
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

size_t ntg_object_graph_collect_roots_pre(ntg_object* root, ntg_object** out, size_t cap)
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

size_t ntg_object_graph_collect_roots_post(ntg_object* root, ntg_object** out, size_t cap)
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

int ntg_object_sort_by_z(ntg_object** objects, size_t size)
{
    if(!objects) return NTG_ERR_INV_ARG;

    if(size == 0) return 0;
    
    size_t i, j;

    ntg_object *tmp_obj;
    for(i = 0; i < size - 1; i++)
    {
        for(j = i + 1; j < size; j++)
        {
            if(objects[j]->ro.layout_opts.z_index < objects[i]->ro.layout_opts.z_index)
            {
                tmp_obj = objects[i];
                objects[i] = objects[j];
                objects[j] = tmp_obj;
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

static void init_default(ntg_object* object)
{
    (*object) = (ntg_object) {0};

    object->ro.layout_opts = ntg_lay_opts_default();

    object->ro.border_opts = ntg_bdr_opts_default();
    object->ro.padding_opts = ntg_padding_opts_default();
    object->ro.anchor_policy = &NTG_ANCHOR_POLICY_ROOT;

    object->priv.base_bg = ntg_vcell_new_default();

    object->ro.clickable = NTG_OBJECT_UNCLICKABLE;
    object->ro.focusable = NTG_OBJECT_UNFOCUSABLE;
}

int ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!object)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_OBJECT))
        return NTG_ERR_INV_TYPE;

    if(!vtable ||!vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    init_default(object);

    object->ro.type = type;
    object->priv.vtable = vtable;
    object->priv.layout_dt = layout_dt;

    ntg_object_draw_init(&object->ro.drawing);

    ntg_event_delegate_init(&object->ro.event_dlgt);

    return 0;
}

int ntg_object_deinit(ntg_object* object)
{
    ntg_not_null(object);

    ntg_scene* scene = ntg_object_get_scene_(object);
    
    if(ntg_object_is_true_root(object))
    {
        ntg_scene_rm_root(scene, object);
    }

    if(object->ro.parent)
    {
        ntg_object_detach(object);
    }

    if(object->ro.base)
    {
        ntg_object_unanchor(object);
    }

    while(object->ro.children.size > 0)
    {
        ntg_object_detach(object->ro.children.data[0]);
    }

    while(object->ro.anchored.size > 0)
    {
        ntg_object_unanchor(object->ro.anchored.data[0]);
    }

    ntg_objptr_vec_deinit(&object->ro.children);

    ntg_objptr_vec_deinit(&object->ro.anchored);

    ntg_object_draw_deinit(&object->ro.drawing);

    ntg_event_delegate_deinit(&object->ro.event_dlgt);

    if(object->priv.layout_dt && object->priv.layout_dt->free_fn)
        object->priv.layout_dt->free_fn(object->priv.layout_dt);

    init_default(object);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_object_attach(ntg_object* parent, ntg_object* child)
{
    int _status;

    if(!parent || !child || (child == parent))
        return NTG_ERR_INV_ARG;

    if(parent->ro.children.size >= NTG_OBJECT_MAX_CHILDREN)
        return NTG_ERR_MAX_CHILDREN;

    if(child->ro.parent != NULL)
        ntg_object_detach(child);

    if(child->priv.scene) // child is root
        ntg_scene_rm_root(child->priv.scene, child);

    if(child->ro.base)
        ntg_object_unanchor(child);

    ntg_scene* scene = ntg_object_get_scene_(parent);

    _status = ntg_objptr_vec_pushb(&parent->ro.children, child);
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
        ntg__scene_add_object_tree(scene, child);

    if(child->priv.vtable->set_parent_fn)
        child->priv.vtable->set_parent_fn(child, parent);

    struct ntg_event_object_chldadd_dt chldadd_dt = { .child = child };
    struct ntg_event_object_prntset_dt prntset_dt = { .parent = parent };

    ntg_event_raise(
            &parent->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_CHLDADD, parent, &chldadd_dt));

    ntg_event_raise(
            &child->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_PRNTSET, child, &prntset_dt));

    if(scene)
        ntg__scene_on_add_object_tree(scene, child);

    ntg_object_mark_dirty(parent, NTG_OBJECT_DIRTY_FULL);
    return 0;
}

/* ------------------------------------------------------ */

int ntg_object_set_base_bg(ntg_object* object, struct ntg_vcell base_bg)
{
    if(!object) return NTG_ERR_INV_ARG;

    object->priv.base_bg = base_bg;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_DRAW);

    return 0;
}

int ntg_object_set_focusable(ntg_object* object, enum ntg_object_focus_mode mode)
{
    if(!object) return NTG_ERR_INV_ARG;

    if(object->ro.focusable == NTG_OBJECT_FOCUSABLE)
    {
        const ntg_scene* scene = ntg_object_get_scene(object);
        if(scene)
        {
            ntg_fcs_manager* fm = scene->ro.fm;
            if(fm && (fm->ro.focused == object))
                ntg_fcs_manager_request_focus(fm, NULL);
        }
    }

    object->ro.focusable = mode;

    return 0;
}

int ntg_object_set_clickable(ntg_object* object, enum ntg_object_click_mode mode)
{
    if(!object) return NTG_ERR_INV_ARG;

    object->ro.clickable = mode;

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

void ntg__object_root_set_scene(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;
    if(object->ro.parent || object->ro.base) return;

    object->priv.scene = scene;
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

size_t ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object)
{
    if(!map || !object)
        return NTG_SIZE_MAX;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
            return map->vals[i];
    }

    return NTG_SIZE_MAX;
}

int ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size)
{
    if(!map || !object) return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
        {
            map->vals[i] = size;
            break;
        }
    }

    return 0;
}

int ntg_object_zero_constrain(const ntg_object* object, ntg_object_size_map* map)
{
    if(!object || !map)
        return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < object->ro.children.size; i++)
        ntg_object_size_map_set(map, object, 0);

    return 0;
}

/* ------------------------------------------------------ */
/* ARRANGE PHASE */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_pos_map_get(
        const ntg_object_pos_map* map,
        const ntg_object* object)
{
    if(!map || !object)
        return NTG_XY_MAX;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
            return map->vals[i];
    }

    return NTG_XY_MAX;
}

int ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos)
{
    if(!map || !object) return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < map->size; i++)
    {
        if(map->keys[i] == object)
        {
            map->vals[i] = pos;
            break;
        }
    }

    return 0;
}

int ntg_object_zero_arrange(const ntg_object* object, ntg_object_pos_map* map)
{
    if(!object || !map)
        return NTG_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < object->ro.children.size; i++)
        ntg_object_pos_map_set(map, object, ntg_xy(0, 0));

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

int ntg_object_mark_dirty(ntg_object* object, uint32_t dirty)
{
    if(!object) return NTG_ERR_INV_ARG;
    if(!dirty) return 0;

    object->ro.dirty |= dirty;

    ntg_scene* scene = ntg_object_get_scene_(object);
    if(scene)
        ntg_scene_mark_dirty(scene);

    return 0;
}

/* ------------------------------------------------------ */
/* MEASURE & SIZE HELPERS */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_min_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->ro.min_size;
}

struct ntg_xy ntg_object_get_nat_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->ro.nat_size;
}

struct ntg_xy ntg_object_get_max_size(const ntg_object* object)
{
    if(!object)
        return ntg_xy(0, 0);

    return object->ro.max_size;
}

struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
        return (struct ntg_object_measure) {0};

    return (struct ntg_object_measure) {
        .min_size = ntg_xy_get(object->ro.min_size, orient),
        .nat_size = ntg_xy_get(object->ro.nat_size, orient),
        .max_size = ntg_xy_get(object->ro.max_size, orient),
        .grow = ntg_xy_get(object->ro.grow, orient)
    };
}

size_t ntg_object_get_size_1d(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
        return 0;
        
    return ntg_xy_get(object->ro.size, orient);
}

struct ntg_xy ntg_object_get_min_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->ro.padding_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->ro.min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->ro.padding_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
        ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->ro.nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_cont(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->ro.padding_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size) + ntg_insets_hsum(pref_padding_size),
            ntg_insets_vsum(pref_border_size) + ntg_insets_vsum(pref_padding_size)
    );

    return ntg_xy_sub(object->ro.max_size, sub);
}

struct ntg_object_measure
ntg_object_get_measure_cont(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    struct ntg_object_measure m = ntg_object_get_measure(object, orient);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;
    struct ntg_insets pref_padding_size = object->ro.padding_opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient) +
            ntg_insets_sum(pref_padding_size, orient);

    m.min_size = _sub2_size(m.min_size, sub);
    m.nat_size = _sub2_size(m.nat_size, sub);
    m.max_size = _sub2_size(m.max_size, sub);

    return m;
}

size_t ntg_object_get_size_1d_cont(const ntg_object* object, enum ntg_orient orient)
{
    if(!object) return 0;

    size_t s = ntg_xy_get(object->ro.size, orient);

    struct ntg_insets border_size = object->ro.border_size;
    struct ntg_insets padding_size = object->ro.padding_size;

    size_t sub = ntg_insets_sum(border_size, orient) +
            ntg_insets_sum(padding_size, orient);

    return _sub2_size(s, sub);
}

size_t ntg_object_get_for_size_cont(const ntg_object* object, enum ntg_orient orient)
{
    if(!object) return 0;

    return (orient == NTG_ORIENT_H) ?
            NTG_SIZE_MAX :
            ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);
}

struct ntg_xy ntg_object_get_min_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->ro.min_size, sub);
}

struct ntg_xy ntg_object_get_nat_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
        ntg_insets_hsum(pref_border_size),
        ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->ro.nat_size, sub);
}

struct ntg_xy ntg_object_get_max_size_pad(const ntg_object* object)
{
    if(!object) return ntg_xy(0, 0);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;

    struct ntg_xy sub = ntg_xy(
            ntg_insets_hsum(pref_border_size),
            ntg_insets_vsum(pref_border_size)
    );
    return ntg_xy_sub(object->ro.max_size, sub);
}

struct ntg_object_measure
ntg_object_get_measure_pad(const ntg_object* object, enum ntg_orient orient)
{
    if(!object)
    {
        return (struct ntg_object_measure) {0};
    }

    struct ntg_object_measure m = ntg_object_get_measure(object, orient);

    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;

    size_t sub = ntg_insets_sum(pref_border_size, orient);

    m.min_size = _sub2_size(m.min_size, sub);
    m.nat_size = _sub2_size(m.nat_size, sub);
    m.max_size = _sub2_size(m.max_size, sub);

    return m;
}

size_t ntg_object_get_size_1d_pad(const ntg_object* object, enum ntg_orient orient)
{
    if(!object) return 0;

    size_t s = ntg_xy_get(object->ro.size, orient);

    struct ntg_insets border_size = object->ro.border_size;

    size_t sub = ntg_insets_sum(border_size, orient);

    return _sub2_size(s, sub);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__object_layout_prepare(ntg_object* object, sarena* arena)
{
    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    if(object->priv.vtable->layout_prepare_fn)
    {
        return object->priv.vtable->layout_prepare_fn(
                object, object->priv.layout_dt, arena);
    }

    return 0;
}

int ntg__object_hmeasure(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    struct ntg_object_measure measure = {0};
    int _status = 0;
    if(object->priv.vtable->measure_fn)
    {
        _status = object->priv.vtable->measure_fn(
                object,
                object->priv.layout_dt,
                NTG_ORIENT_H,
                arena,
                relayout,
                &measure);
        if(_status)
        {
            set_hmeasure_helper(object, measure);
            return _status;
        }

        size_t extra = ntg_insets_hsum(object->ro.padding_opts.pref_size) +
                       ntg_insets_hsum(object->ro.border_opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->ro.layout_opts.min_cont_size.x + extra,
                object->ro.layout_opts.max_cont_size.x + extra,
                object->ro.layout_opts.grow.x);

        if(measure.min_size == extra)
            measure.min_size = 0;

        if(measure.nat_size == extra)
            measure.nat_size = 0;

        if(measure.max_size == extra)
            measure.max_size = 0;
    }

    set_hmeasure_helper(object, measure);
    return 0;
}

int ntg__object_hconstrain(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    if(object->priv.skip_hborder)
    {
        object->ro.border_size.w = 0;
        object->ro.border_size.e = 0;
    }
    else
    {
        size_t w, e;
        int status = calculate_border_hsize(object, &w, &e);
        if(status != 0)
            return status;

        object->ro.border_size.w = w;
        object->ro.border_size.e = e;
    }
    if(object->priv.skip_hpadding)
    {
        object->ro.padding_size.w = 0;
        object->ro.padding_size.e = 0;
    }
    else
    {
        size_t w, e;
        int status = calculate_padding_hsize(object, &w, &e);
        if(status != 0)
            return status;

        object->ro.padding_size.w = w;
        object->ro.padding_size.e = e;
    }

    ntg_object_mark_dirty(object,
            NTG_OBJECT_DIRTY_VCONSTRAIN |
            NTG_OBJECT_DIRTY_ARRANGE |
            NTG_OBJECT_DIRTY_DRAW);

    int _status;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_H);

    ntg_object_size_map map;
    _status = size_map_init(&map, &object->ro.children, arena);
    if(_status != 0)
    {
        for(i = 0; i < object->ro.children.size; i++)
            set_hsize_helper(object->ro.children.data[i], 0);

        return _status;
    }

    if(object->priv.vtable->constrain_fn)
    {
        _status = object->priv.vtable->constrain_fn(
                object,
                object->priv.layout_dt,
                NTG_ORIENT_H,
                &map,
                arena,
                relayout);
        if(_status)
        {
            for(i = 0; i < object->ro.children.size; i++)
                set_hsize_helper(object->ro.children.data[i], 0);

            return _status;
        }
    }

    ntg_object* it_child;
    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = _min2_size(content_size, map.vals[i]);

        set_hsize_helper(it_child, it_size);
    }
    return 0;
}

int ntg__object_vmeasure(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    struct ntg_object_measure measure = {0};
    int _status = 0;
    if(object->priv.vtable->measure_fn)
    {
        _status = object->priv.vtable->measure_fn(
                object,
                object->priv.layout_dt,
                NTG_ORIENT_V,
                arena,
                relayout,
                &measure);
        if(_status)
        {
            set_vmeasure_helper(object, measure);
            return _status;
        }

        size_t extra = ntg_insets_vsum(object->ro.padding_opts.pref_size) +
                ntg_insets_vsum(object->ro.border_opts.pref_size);

        measure.min_size += extra;
        measure.nat_size += extra;
        measure.max_size += extra;

        measure = incorporate_user_measure(
                measure,
                object->ro.layout_opts.min_cont_size.y + extra,
                object->ro.layout_opts.max_cont_size.y + extra,
                object->ro.layout_opts.grow.y);

        if(measure.min_size == extra)
            measure.min_size = 0;

        if(measure.nat_size == extra)
            measure.nat_size = 0;

        if(measure.max_size == extra)
            measure.max_size = 0;
    }

    set_vmeasure_helper(object, measure);
    return 0;
}

int ntg__object_vconstrain(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    ntg_object_mark_dirty(object,
            NTG_OBJECT_DIRTY_ARRANGE |
            NTG_OBJECT_DIRTY_DRAW);

    bool repeat_border = false;
    bool repeat_padding = false;
    int _status = vconstrain_border(object, &repeat_border);
    if(_status != 0)
        return _status;
    _status = vconstrain_padding(object, &repeat_padding);
    if(_status != 0)
        return _status;
    if(relayout && (repeat_border || repeat_padding))
        (*relayout) |= NTG_OBJECT_DIRTY_HCONSTRAIN;

    size_t i;
    size_t content_size = ntg_object_get_size_1d_cont(object, NTG_ORIENT_V);

    ntg_object_size_map map;
    _status = size_map_init(&map, &object->ro.children, arena);
    if(_status)
    {
        for(i = 0; i < object->ro.children.size; i++)
            set_vsize_helper(object->ro.children.data[i], 0);

        return _status;
    }

    if(object->priv.vtable->constrain_fn)
    {
        _status = object->priv.vtable->constrain_fn(
                object,
                object->priv.layout_dt,
                NTG_ORIENT_V,
                &map,
                arena,
                relayout);
        if(relayout && (repeat_border || repeat_padding))
            (*relayout) |= NTG_OBJECT_DIRTY_HCONSTRAIN;
        if(_status)
        {
            for(i = 0; i < object->ro.children.size; i++)
                set_vsize_helper(object->ro.children.data[i], 0);

            return _status;
        }
    }

    ntg_object* it_child;
    size_t it_size;
    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_size = _min2_size(content_size, map.vals[i]);

        set_vsize_helper(it_child, it_size);
    }
    return 0;
}

int ntg__object_arrange(ntg_object* object, sarena* arena, uint32_t* relayout)
{
    ntg_set_out(relayout, 0);

    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    int _status;
    size_t i;

    ntg_object_pos_map map;
    _status = pos_map_init(&map, &object->ro.children, arena);
    if(_status)
    {
        for(i = 0; i < object->ro.children.size; i++)
            set_pos_helper(object->ro.children.data[i], ntg_xy(0, 0));

        return _status;
    }

    if(object->priv.vtable->arrange_fn)
    {
        _status = object->priv.vtable->arrange_fn(
                object,
                object->priv.layout_dt,
                &map,
                arena,
                relayout);
        if(_status)
        {
            for(i = 0; i < object->ro.children.size; i++)
                set_pos_helper(object->ro.children.data[i], ntg_xy(0, 0));

            return _status;
        }
    }

    struct ntg_xy dcr_sum = ntg_xy(
            object->ro.border_size.w + object->ro.padding_size.w,
            object->ro.border_size.n + object->ro.padding_size.n);

    ntg_object* it_child;
    struct ntg_xy it_pos;

    for(i = 0; i < map.size; i++)
    {
        it_child = map.keys[i];
        it_pos = ntg_xy_add(map.vals[i], dcr_sum);
        it_pos = ntg_xy_pos_clamp(it_pos, it_child->ro.size, object->ro.size);

        set_pos_helper(it_child, it_pos);
    }
    return 0;
}

void ntg__object_layout_finalize(ntg_object* object, sarena* arena)
{
    if(!object) return;

    (void)arena;

    struct ntg_xy old_size = object->priv.old_size;
    struct ntg_xy new_size = object->ro.size;
    if(!ntg_xy_are_eql(old_size, new_size))
    {
        if(object->priv.vtable->resize_fn)
            object->priv.vtable->resize_fn(object, arena);

        struct ntg_event_object_szchg_dt event_dt = {
            .old_size = &old_size,
            .new_size = &new_size
        };

        ntg_event_raise(
                &object->ro.event_dlgt,
                ntg_event_new(NTG_EVENT_OBJECT_SZCHG, object, &event_dt));
    }

    struct ntg_xy old_cont_size = object->priv.old_cont_size;
    struct ntg_xy new_cont_size = ntg_object_get_size_cont(object);
    if(!ntg_xy_are_eql(old_cont_size, new_cont_size))
    {
        if(object->priv.vtable->cont_resize_fn)
            object->priv.vtable->cont_resize_fn(object, arena);

        struct ntg_event_object_contszchg_dt event_dt = {
            .old_size = &old_size,
            .new_size = &new_size
        };

        ntg_event_raise(
                &object->ro.event_dlgt,
                ntg_event_new(NTG_EVENT_OBJECT_CONTSZCHG, object, &event_dt));
    }

    struct ntg_xy old_pos = object->priv.old_pos;
    struct ntg_xy new_pos = ntg_object_get_abs_pos(object);
    if(!ntg_xy_are_eql(old_pos, new_pos))
    {
        if(object->priv.vtable->pos_chng_fn)
            object->priv.vtable->pos_chng_fn(object, arena);

        struct ntg_event_object_poschg_dt event_dt = {
            .old_pos = &old_pos,
            .new_pos = &new_pos
        };

        ntg_event_raise(
                &object->ro.event_dlgt,
                ntg_event_new(NTG_EVENT_OBJECT_POSCHG, object, &event_dt));
    }
}

int ntg__object_draw(ntg_object* object, sarena* arena)
{
    if(!object || !arena)
        return NTG_ERR_INV_ARG;

    ntg_object_mark_dirty(object, NTG__OBJECT_DIRTY_RENDER);

    const ntg_scene* scene = ntg_object_get_scene_(object);
    if(!scene)
        return NTG_ERR_INV_ARG;

    int _status = ntg_object_draw_set_size(
            &object->ro.drawing, object->ro.size);
    struct ntg_xy drawing_size = ntg_object_draw_get_size(&object->ro.drawing);

    /* Even if the alloc fails, set the cells to defaults */

    size_t i, j;
    for(i = 0; i < drawing_size.y; i++)
    {
        for(j = 0; j < drawing_size.x; j++)
        {
            ntg_object_draw_set(&object->ro.drawing, ntg_vcell_new_default(), ntg_xy(j, i));
        }
    }

    /* Now return if alloc failed */

    if(_status != 0)
        return _status;

    if(ntg_insets_hsum(object->ro.border_size) || ntg_insets_vsum(object->ro.border_size))
        _status = draw_unoptimized(object, arena);
    else
        _status = draw_optimized(object, arena);

    return _status;
}

void ntg__object_root_set_hsize(ntg_object* object, size_t size)
{
    if(!object) return;
    if(object->ro.parent) return;

    set_hsize_helper(object, size);
}

void ntg__object_root_set_vsize(ntg_object* object, size_t size)
{
    if(!object) return;
    if(object->ro.parent) return;

    set_vsize_helper(object, size);
}

void ntg__object_root_set_pos(ntg_object* object, struct ntg_xy pos)
{
    if(!object) return;
    if(object->ro.parent) return;

    set_pos_helper(object, pos);
}

void ntg__object_clean(ntg_object* object, uint32_t clean)
{
    if(!object) return;

    object->ro.dirty &= (~clean);
}

void ntg__object_scene_enter(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    layout_reset(object);
    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);

    if(object->priv.vtable->enter_scene_fn)
        object->priv.vtable->enter_scene_fn(object, scene);
}

void ntg__object_on_scene_enter(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    struct ntg_event_object_scnset_dt event_dt = { .scene = scene };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_SCNSET, object, &event_dt));
}

void ntg__object_scene_leave(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    layout_reset(object);

    if(object->priv.vtable->rm_scene_fn)
        object->priv.vtable->rm_scene_fn(object, scene);
}

void ntg__object_on_scene_leave(ntg_object* object, ntg_scene* scene)
{
    if(!object) return;

    struct ntg_event_object_scnrm_dt event_dt = { .scene = scene };
    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_SCNRM, object, &event_dt));

}

void ntg__object_focus(ntg_object* object)
{
    if(!object) return;

    if(object->priv.vtable->focus_fn)
        object->priv.vtable->focus_fn(object);

    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_FCS, object, NULL));

}

void ntg__object_unfocus(ntg_object* object)
{
    if(!object) return;

    if(object->priv.vtable->unfocus_fn)
        object->priv.vtable->unfocus_fn(object);

    ntg_event_raise(
            &object->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_OBJECT_UNFCS, object, NULL));

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
/* LAYOUT OBJECT INIT */
/* ------------------------------------------------------ */

static int size_map_init(
        ntg_object_size_map* map,
        const struct ntg_objptr_vec* children,
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
    map->keys = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
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
        ntg_object_pos_map* map,
        const struct ntg_objptr_vec* children,
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
    map->keys = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
    if(!map->keys) return NTG_ERR_ALLOC_FAIL;
    map->vals = sarena_malloc(arena, sizeof(struct ntg_xy) * children->size);
    if(!map->vals) return NTG_ERR_ALLOC_FAIL;

    size_t i;
    for(i = 0; i < children->size; i++)
    {
        map->keys[i] = children->data[i];
        map->vals[i] = ntg_xy(0, 0);
    }

    return 0;
}

static int tmp_draw_init(
        struct ntg_object_tmp_draw* drawing,
        struct ntg_xy size,
        struct ntg_vcell base_bg,
        sarena* arena)
{
    if(!drawing || !arena)
        return NTG_ERR_INV_ARG;

    drawing->data = NULL;
    drawing->size = ntg_xy(0, 0);

    drawing->data = sarena_malloc(arena, 1 +
        sizeof(struct ntg_vcell) * size.x * size.y);
    if(!drawing->data) return NTG_ERR_ALLOC_FAIL;
    drawing->size = size;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_draw_set(drawing, base_bg, ntg_xy(j, i));
        }
    }

    return 0;
}

static void layout_reset(ntg_object* object)
{
    if(!object) return;

    object->ro.min_size = ntg_xy(0, 0);
    object->ro.nat_size = ntg_xy(0, 0);
    object->ro.max_size = ntg_xy(0, 0);
    object->ro.grow = ntg_xy(0, 0);
    object->priv.skip_hborder = false;
    ntg__object_clean(object, NTG_OBJECT_DIRTY_FULL | NTG__OBJECT_DIRTY_RENDER);
    object->priv.skip_hpadding = false;
    object->priv.special_repeat = false;
    object->ro.size = ntg_xy(0, 0);
    object->ro.border_size = ntg_insets(0, 0, 0, 0);
    object->ro.padding_size = ntg_insets(0, 0, 0, 0);
    object->priv.old_size = ntg_xy(0, 0);
    object->priv.old_pos = ntg_xy(0, 0);
    object->priv.old_cont_size = ntg_xy(0, 0);

    ntg_object_draw_set_size(&object->ro.drawing, ntg_xy(0, 0));

    if(object->priv.layout_dt && object->priv.layout_dt->reset_fn)
        object->priv.layout_dt->reset_fn(object->priv.layout_dt);
}

static bool set_hmeasure_helper(ntg_object* object, struct ntg_object_measure measure)
{
    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_H);

    if(!ntg_object_measure_are_eql(measure, old))
    {
        object->ro.min_size.x = measure.min_size;
        object->ro.nat_size.x = measure.nat_size;
        object->ro.max_size.x = measure.max_size;
        object->ro.grow.x = measure.grow;
        ntg_object_mark_dirty(object,
                NTG_OBJECT_DIRTY_HCONSTRAIN |
                NTG_OBJECT_DIRTY_VMEASURE);
        if(object->ro.parent)
        {
            ntg_object_mark_dirty(object->ro.parent,
                    NTG_OBJECT_DIRTY_MEASURE |
                    NTG_OBJECT_DIRTY_CONSTRAIN);
        }
        return true;
    }

    return false;
}

static bool set_vmeasure_helper(ntg_object* object, struct ntg_object_measure measure)
{
    struct ntg_object_measure old = ntg_object_get_measure(object, NTG_ORIENT_V);

    if(!ntg_object_measure_are_eql(measure, old))
    {
        object->ro.min_size.y = measure.min_size;
        object->ro.nat_size.y = measure.nat_size;
        object->ro.max_size.y = measure.max_size;
        object->ro.grow.y = measure.grow;

        ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN);
        if(object->ro.parent)
        {
            ntg_object_mark_dirty(object->ro.parent,
                    NTG_OBJECT_DIRTY_VMEASURE |
                    NTG_OBJECT_DIRTY_VCONSTRAIN);
        }

        return true;
    }
    return false;
}

static bool set_hsize_helper(ntg_object* object, size_t size)
{
    if(object->ro.size.x != size)
    {
        object->ro.size.x = size;
        ntg_object_mark_dirty(object, 
                NTG_OBJECT_DIRTY_HCONSTRAIN |
                NTG_OBJECT_DIRTY_VMEASURE |
                NTG_OBJECT_DIRTY_VCONSTRAIN);
        object->priv.skip_hborder = false;
        object->priv.skip_hpadding = false;
        return true;
    }
    else return false;
}

static bool set_vsize_helper(ntg_object* object, size_t size)
{
    if(object->ro.size.y != size)
    {
        object->ro.size.y = size;

        bool hborder_missing = 
            (ntg_insets_hsum(object->ro.border_opts.pref_size) > 0) &&
            (ntg_insets_hsum(object->ro.border_size) == 0);
        bool hpadding_missing = 
            (ntg_insets_hsum(object->ro.padding_opts.pref_size) > 0) &&
            (ntg_insets_hsum(object->ro.padding_size) == 0);

        if(hborder_missing || hpadding_missing)
        {
            object->priv.skip_hborder = false;
            object->priv.skip_hpadding = false;
            object->priv.special_repeat = true;
            ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN);
        }
        else
        {
            ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_VCONSTRAIN);
        }
        return true;
    }
    else return false;
}

static inline bool set_pos_helper(ntg_object* object, struct ntg_xy pos)
{
    if(!ntg_xy_are_eql(object->ro.pos, pos))
    {
        object->ro.pos = pos;
        return true;
    }
    else return false;
}

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static struct ntg_object_measure incorporate_user_measure(
        struct ntg_object_measure measure,
        size_t user_min_size,
        size_t user_max_size,
        size_t user_grow)
{
    user_min_size = _min2_size(user_min_size, user_max_size);
 
    if(user_max_size < measure.min_size)
        measure.min_size = user_max_size;
    else
        measure.min_size = _max2_size(measure.min_size, user_min_size);
 
    if(user_min_size > measure.max_size)
        measure.max_size = user_min_size;
    else
        measure.max_size = _min2_size(measure.max_size, user_max_size);
 
    measure.nat_size = _clamp_size(
            measure.min_size,
            measure.nat_size,
            measure.max_size);
 
    measure.grow = (user_grow != NTG_OBJECT_GROW_UNSET) ?
            user_grow : measure.grow;

    return measure;
}

static int get_dcr_size(
        enum ntg_object_dcr_enable enable,
        size_t pref_size[2],
        size_t size,
        struct ntg_object_measure inner_measure,
        size_t out_size[2])
{
    size_t extra;

    if(enable == NTG_OBJECT_DCR_ENABLE_MIN)
        extra = _sub2_size(size, inner_measure.min_size);
    else if(enable == NTG_OBJECT_DCR_ENABLE_NAT)
        extra = _sub2_size(size, inner_measure.nat_size);
    else extra = size;

    out_size[0] = 0;
    out_size[1] = 0;
    double scratch_buffer[2];
    return ntg_sap_cap_round_robin(pref_size, NULL, extra, 2,
                                   scratch_buffer, out_size, NULL);
}

static int calculate_border_hsize(
        ntg_object* object, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->ro.border_opts.pref_size.w;
    we_pref_size[1] = object->ro.border_opts.pref_size.e;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            object->ro.border_opts.enable,
            we_pref_size,
            object->ro.size.x,
            ntg_object_get_measure_pad(object, NTG_ORIENT_H),
            _sizes);
    if(status != 0)
        return status;

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
    return 0;
}

static int vconstrain_border(ntg_object* object, bool* out_repeat)
{
    struct ntg_insets pref_border_size = object->ro.border_opts.pref_size;

    bool hborder_missing = 
            (ntg_insets_hsum(pref_border_size) > 0) &&
            (ntg_insets_hsum(object->ro.border_size) == 0);

    if(!out_repeat) return NTG_ERR_INV_ARG;
    *out_repeat = false;

    size_t n = 0, s = 0;
    int status = calculate_border_vsize(object, &n, &s);
    if(status != 0) return status;

    object->ro.border_size.n = n;
    object->ro.border_size.s = s;

    bool vborder_missing = 
            (ntg_insets_vsum(pref_border_size) > 0) &&
            (ntg_insets_vsum(object->ro.border_size) == 0);

    if(hborder_missing)
    {
        if(object->priv.skip_hborder) // Missing cuz skipped
        {
            // object->priv.skip_hborder = false;
            if(vborder_missing) // Vborder missing
                return 0;
            else // Vborder present
            {
                object->ro.border_size.n = 0;
                object->ro.border_size.s = 0;
                return 0;
            }
        }
        else
        {
            if(vborder_missing) // Vborder missing
                return 0;
            else // Vborder present
            {
                object->ro.border_size.n = 0;
                object->ro.border_size.s = 0;
                if(object->priv.special_repeat)
                {
                    object->priv.special_repeat = false;
                    *out_repeat = true;
                    return 0;
                }
                else return 0;
            }
        }
    }
    else
    {
        if(object->priv.skip_hborder)  // INVALID STATE
            assert(0);
        else
        {
            if(vborder_missing)
            {
                object->priv.skip_hborder = true;
                *out_repeat = true;
                    return 0;
            }
            else return 0;
        }
    }
}

static int vconstrain_padding(ntg_object* object, bool* out_repeat)
{
    struct ntg_insets pref_padding_size = object->ro.padding_opts.pref_size;

    bool hpadding_missing = 
            (ntg_insets_hsum(pref_padding_size) > 0) &&
            (ntg_insets_hsum(object->ro.padding_size) == 0);

    if(!out_repeat) return NTG_ERR_INV_ARG;
    *out_repeat = false;

    size_t n = 0, s = 0;
    int status = calculate_padding_vsize(object, &n, &s);
    if(status != 0) return status;

    object->ro.padding_size.n = n;
    object->ro.padding_size.s = s;

    bool vpadding_missing = 
            (ntg_insets_vsum(pref_padding_size) > 0) &&
            (ntg_insets_vsum(object->ro.padding_size) == 0);

    if(hpadding_missing)
    {
        if(object->priv.skip_hpadding) // Missing cuz skipped
        {
            // object->priv.skip_hpadding = false;
            if(vpadding_missing) // Vpadding missing
                return 0;
            else // Vpadding present
            {
                object->ro.padding_size.n = 0;
                object->ro.padding_size.s = 0;
                return 0;
            }
        }
        else
        {
            if(vpadding_missing) // Vpadding missing
                return 0;
            else // Vpadding present
            {
                object->ro.padding_size.n = 0;
                object->ro.padding_size.s = 0;
                if(object->priv.special_repeat)
                {
                    object->priv.special_repeat = false;
                    *out_repeat = true;
                    return 0;
                }
                else return 0;
            }
        }
    }
    else
    {
        if(object->priv.skip_hpadding) // INVALID STATE
            assert(0);
        else
        {
            if(vpadding_missing)
            {
                object->priv.skip_hpadding = true;
                *out_repeat = true;
                return 0;
            }
            else return 0;
        }
    }
}

static int calculate_border_vsize(ntg_object* object, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->ro.border_opts.pref_size.n;
    ns_pref_size[1] = object->ro.border_opts.pref_size.s;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            object->ro.border_opts.enable,
            ns_pref_size, 
            object->ro.size.y,
            ntg_object_get_measure_pad(object, NTG_ORIENT_V),
            _sizes);
    if(status != 0) return status;

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
    return 0;
}

static int calculate_padding_hsize(ntg_object* object, size_t* out_w, size_t* out_e)
{
    size_t we_pref_size[2];
    we_pref_size[0] = object->ro.padding_opts.pref_size.w;
    we_pref_size[1] = object->ro.padding_opts.pref_size.e;
    size_t _sizes[2] = {0, 0};

    int status = get_dcr_size(
            object->ro.padding_opts.enable,
            we_pref_size,  
            ntg_object_get_size_pad(object).x,
            ntg_object_get_measure_cont(object, NTG_ORIENT_H),
            _sizes);
    if(status != 0) return status;

    (*out_w) = _sizes[0];
    (*out_e) = _sizes[1];
    return 0;
}

static int calculate_padding_vsize(ntg_object* object, size_t* out_n, size_t* out_s)
{
    size_t ns_pref_size[2];
    ns_pref_size[0] = object->ro.padding_opts.pref_size.n;
    ns_pref_size[1] = object->ro.padding_opts.pref_size.s;

    size_t _sizes[2] = {0, 0};

     int status = get_dcr_size(
             object->ro.padding_opts.enable,
             ns_pref_size,  
             ntg_object_get_size_pad(object).y,
             ntg_object_get_measure_cont(object, NTG_ORIENT_V),
             _sizes);
    if(status != 0) return status;

    (*out_n) = _sizes[0];
    (*out_s) = _sizes[1];
    return 0;
}
static int draw_optimized(ntg_object* object, sarena* arena)
{
    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = ntg_object_draw_get_size(&object->ro.drawing);

    struct ntg_vcell bg = object->priv.base_bg;
    struct ntg_insets psize = object->ro.padding_size;

    struct ntg_object_tmp_draw content_drawing;
    int _status = tmp_draw_init(&content_drawing, content_size, bg, arena);
    if(_status != 0)
        return _status;

    if(object->priv.vtable->draw_fn)
    {
        _status = object->priv.vtable->draw_fn(
                object,
                object->priv.layout_dt,
                &content_drawing,
                arena);
        if(_status != 0) return _status;
    }

    struct ntg_vcell it_src_cell;

    struct ntg_xy offset = ntg_xy(psize.w, psize.n);

    size_t i, j;
    struct ntg_xy ji;
    for(i = 0; i < object_size.y; i++)
    {
        for(j = 0; j < object_size.x; j++)
        {
            ji = ntg_xy(j, i);
            ntg_object_draw_set(&object->ro.drawing, bg, ji);
        }
    }

    for(i = 0; i < content_size.y; i++)
    {
        for(j = 0; j < content_size.x; j++)
        {
            ji = ntg_xy(j, i);

            it_src_cell = ntg_object_tmp_draw_get(&content_drawing, ji);
            ntg_object_draw_set(&object->ro.drawing, it_src_cell, ntg_xy_add(offset, ji));
        }
    }
    return 0;
}

static int 
draw_unoptimized(ntg_object* object, sarena* arena)
{
    struct ntg_xy content_size = ntg_object_get_size_cont(object);
    struct ntg_xy object_size = ntg_object_draw_get_size(&object->ro.drawing);

    struct ntg_vcell bg = object->priv.base_bg;
    struct ntg_insets bsize = object->ro.border_size;
    struct ntg_insets psize = object->ro.padding_size;
    const struct ntg_border_style* border_style = object->ro.border_opts.style;

    struct ntg_object_tmp_draw content_drawing;
    int _status = tmp_draw_init(&content_drawing, content_size, bg, arena);
    if(_status != 0)
        return _status;

    struct ntg_object_tmp_draw object_drawing;
    _status = tmp_draw_init(&object_drawing, object_size, bg, arena);
    if(_status != 0)
        return _status;

    ntg__border_style_draw(border_style, object_size, bsize, &object_drawing);

    size_t i, j;

    for(i = bsize.n; i < (object_size.y - bsize.s); i++)
    {
        for(j = bsize.w; j < (object_size.x - bsize.e); j++)
        {
            ntg_object_tmp_draw_set(&object_drawing, bg, ntg_xy(j, i));
        }
    }

    if(object->priv.vtable->draw_fn)
    {
        _status = object->priv.vtable->draw_fn(
                object,
                object->priv.layout_dt,
                &content_drawing,
                arena);

        if(_status != 0)
            return _status;
    }

    struct ntg_vcell it_src_cell;

    struct ntg_xy offset = ntg_xy(bsize.w + psize.w, bsize.n + psize.n);
    struct ntg_xy ji;
    for(i = 0; i < content_size.y; i++)
    {
        for(j = 0; j < content_size.x; j++)
        {
            ji = ntg_xy(j, i);

            it_src_cell = ntg_object_tmp_draw_get(&content_drawing, ji);

            ntg_object_tmp_draw_set(
                    &object_drawing,
                    it_src_cell,
                    ntg_xy_add(offset, ji));
        }
    }

    for(i = 0; i < object_size.y; i++)
    {
        for(j = 0; j < object_size.x; j++)
        {
            ji = ntg_xy(j, i);

            it_src_cell = ntg_object_tmp_draw_get(&object_drawing, ji);
            ntg_object_draw_set(&object->ro.drawing, it_src_cell, ji);
        }
    }

    return 0;
}
