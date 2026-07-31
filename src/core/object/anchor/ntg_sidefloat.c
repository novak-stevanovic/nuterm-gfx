#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static size_t sidefloat_constrain_fn(
        const ntg_anchor_policy* ap,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy sidefloat_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static void sidefloat_deinit_fn(ntg_anchor_policy* ap);

static const struct ntg_anchor_policy_vtable VTABLE = {
    .constrain_fn = sidefloat_constrain_fn,
    .arrange_fn = sidefloat_arrange_fn,
    .deinit_fn = sidefloat_deinit_fn
};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_sidefloat_opts ntg_sidefloat_opts_def()
{
    return (struct ntg_sidefloat_opts) {
        .align = NTG_ALIGN_1,
        .side = NTG_SIDE_N,
        .thresh = NTG_SIDEFLOAT_THRESH_MIN,
        .size_cap = NTG_SIDEFLOAT_SIZE_CAP_NONE
    };
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_sidefloat_init(
        ntg_sidefloat* sidefloat_ap,
        const struct ntg_sidefloat_opts* opts,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!sidefloat_ap)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    int _status;

    ntg_anchor_policy_init_inherit(&sidefloat_ap->__base, &VTABLE, &_status);
    if(_status)
        ntg_vreturn(out_status, _status);

    sidefloat_ap->_opts = opts ? (*opts) : ntg_sidefloat_opts_def();
}

void ntg_sidefloat_deinit(ntg_sidefloat* sidefloat_ap)
{
    if(!sidefloat_ap) return;

    sidefloat_ap->_opts = ntg_sidefloat_opts_def();
    ntg_anchor_policy_deinit(&sidefloat_ap->__base);
}

void ntg_sidefloat_deinit_void(void* _sidefloat_ap)
{
    ntg_sidefloat_deinit(_sidefloat_ap);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static size_t sidefloat_constrain_fn(
        const ntg_anchor_policy* ap,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx || !ctx->root || !ctx->base) return 0;

    const ntg_sidefloat* sidefloat_ap = (const ntg_sidefloat*)ap;
    const struct ntg_sidefloat_opts* opts = &sidefloat_ap->_opts;
    const ntg_object* root = ctx->root;
    const ntg_object* base = ctx->base;

    struct ntg_xy base_pos_xy = ntg_xy_from_dxy(
            ntg_object_map_to_scene(base, ntg_dxy(0, 0)));
    size_t base_pos = ntg_xy_get(base_pos_xy, orient);

    struct ntg_object_measure root_measure;
    root_measure = ntg_object_get_measure(root, orient);

    const ntg_scene* scene = ntg_object_get_scene(root);
    size_t scene_size = scene ? ntg_xy_get(scene->_size, orient) : 0;

    size_t base_size = ntg_xy_get(base->_size, orient);
    if(base_size == 0) return 0;

    bool capped = opts->size_cap == NTG_SIDEFLOAT_SIZE_CAP_ANCHOR;
    size_t available_space;

    switch(opts->side)
    {
        case NTG_SIDE_N:
            available_space = (orient == NTG_ORIENT_H) ?
                    (capped ? base_size : scene_size) : base_pos;
            break;
        case NTG_SIDE_E:
            available_space = (orient == NTG_ORIENT_V) ?
                    (capped ? base_size : scene_size) :
                    _sub2_size(scene_size, base_size + base_pos);
            break;
        case NTG_SIDE_S:
            available_space = (orient == NTG_ORIENT_H) ?
                    (capped ? base_size : scene_size) :
                    _sub2_size(scene_size, base_size + base_pos);
            break;
        case NTG_SIDE_W:
            available_space = (orient == NTG_ORIENT_V) ?
                    (capped ? base_size : scene_size) : base_pos;
            break;
        default:
            return 0;
    }

    size_t thresh_size;
    switch(opts->thresh)
    {
        case NTG_SIDEFLOAT_THRESH_MIN:
            thresh_size = root_measure.min_size;
            break;
        case NTG_SIDEFLOAT_THRESH_NAT:
            thresh_size = root_measure.nat_size;
            break;
        case NTG_SIDEFLOAT_THRESH_ALWAYS:
            thresh_size = 0;
            break;
        default:
            return 0;
    }

    if(available_space < thresh_size)
        return 0;

    return _min2_size(available_space, root_measure.nat_size);
}

static struct ntg_xy sidefloat_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx || !ctx->root || !ctx->base) return ntg_xy(0, 0);

    const ntg_sidefloat* sidefloat_ap = (const ntg_sidefloat*)ap;
    const struct ntg_sidefloat_opts* opts = &sidefloat_ap->_opts;
    const ntg_object* root = ctx->root;
    const ntg_object* base = ctx->base;

    struct ntg_xy pos = ntg_xy(0, 0);
    struct ntg_dxy size_diff = ntg_dxy(
            (ssize_t)root->_size.x - (ssize_t)base->_size.x,
            (ssize_t)root->_size.y - (ssize_t)base->_size.y);

    switch(opts->side)
    {
        case NTG_SIDE_N:
            pos.x = (size_diff.x > 0) ?
                    _sub2_size(base->_pos.x, size_diff.x) :
                    base->_pos.x + ntg_align_offset_d(
                            root->_size.x,
                            base->_size.x,
                            opts->align);
            pos.y = _sub2_size(base->_pos.y, root->_size.y);
            break;
        case NTG_SIDE_E:
            pos.x = base->_pos.x + base->_size.x;
            pos.y = (size_diff.y > 0) ?
                    _sub2_size(base->_pos.y, size_diff.y) :
                    base->_pos.y + ntg_align_offset_d(
                            root->_size.y,
                            base->_size.y,
                            opts->align);
            break;
        case NTG_SIDE_S:
            pos.x = (size_diff.x > 0) ?
                    _sub2_size(base->_pos.x, size_diff.x) :
                    base->_pos.x + ntg_align_offset_d(
                            root->_size.x,
                            base->_size.x,
                            opts->align);
            pos.y = base->_pos.y + base->_size.y;
            break;
        case NTG_SIDE_W:
            pos.x = _sub2_size(base->_pos.x, root->_size.x);
            pos.y = (size_diff.y > 0) ?
                    _sub2_size(base->_pos.y, size_diff.y) :
                    base->_pos.y + ntg_align_offset_d(
                            root->_size.y,
                            base->_size.y,
                            opts->align);
            break;
        default:
            break;
    }

    return pos;
}

static void sidefloat_deinit_fn(ntg_anchor_policy* ap)
{
    ntg_sidefloat_deinit((ntg_sidefloat*)ap);
}
