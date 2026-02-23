#ifndef NTG_ATTACH_POLICY_H
#define NTG_ATTACH_POLICY_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

struct ntg_attach_constrain_ctx
{
    const ntg_object* attacher_layer;
    const ntg_object* base_layer;
    size_t scene_size;
};

struct ntg_attach_arrange_ctx
{
    const ntg_object* attacher_layer;
    const ntg_object* base_layer;
    struct ntg_xy size;
    struct ntg_xy scene_size;
};

struct ntg_attach_policy
{
    size_t (*constrain_fn)(
            const void* data,
            ntg_orient orient,
            const struct ntg_attach_constrain_ctx* ctx,
            sarena* arena);
    struct ntg_xy (*arrange_fn)(
            const void* data,
            const struct ntg_attach_arrange_ctx* ctx,
            sarena* arena);
    void* data;
    void (*free_fn)(void* data);
};

const struct ntg_attach_policy* ntg_attach_policy_root();

/* -------------------------------------------------------------------------- */
/* FLOAT */
/* -------------------------------------------------------------------------- */

enum ntg_attach_policy_flt_enable
{
    NTG_ATTACH_POLICY_FLT_ENABLE_MIN,
    NTG_ATTACH_POLICY_FLT_ENABLE_BASE_MIN,
    NTG_ATTACH_POLICY_FLT_ENABLE_NAT,
    NTG_ATTACH_POLICY_FLT_ENABLE_BASE_NAT,
    NTG_ATTACH_POLICY_FLT_ENABLE_ALWAYS,
};

struct ntg_attach_policy_flt_opts
{
    enum ntg_attach_policy_flt_enable enable;
    struct ntg_insets shrink;
    ntg_align prim_align, sec_align;
};

struct ntg_attach_policy_flt_opts ntg_attach_policy_flt_opts_def();

/* -------------------------------------------------------------------------- */
/* SIDE FLOAT */
/* -------------------------------------------------------------------------- */

enum ntg_attach_policy_sflt_orient
{
    NTG_ATTACH_POLICY_SFLT_ORIENT_N,
    NTG_ATTACH_POLICY_SFLT_ORIENT_E,
    NTG_ATTACH_POLICY_SFLT_ORIENT_S,
    NTG_ATTACH_POLICY_SFLT_ORIENT_W,
};

enum ntg_attach_policy_sflt_thresh
{
    NTG_ATTACH_POLICY_SFLT_THRESH_MIN,
    NTG_ATTACH_POLICY_SFLT_THRESH_NAT,
    NTG_ATTACH_POLICY_SFLT_THRESH_ALWAYS,
};

enum ntg_attach_policy_sflt_enable
{
    NTG_ATTACH_POLICY_SFLT_ENABLE_STATIC,
    NTG_ATTACH_POLICY_SFLT_ENABLE_DYNAMIC
};

struct ntg_attach_policy_sflt_opts
{
    ntg_align align;
    enum ntg_attach_policy_sflt_orient orient;
    enum ntg_attach_policy_sflt_thresh thresh;
    enum ntg_attach_policy_sflt_enable enable;
};

struct ntg_attach_policy_sflt_opts ntg_attach_policy_sflt_opts_def();

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

void ntg_attach_policy_deinit(struct ntg_attach_policy* policy);
void ntg_attach_policy_deinit_(void* _policy);

/* -------------------------------------------------------------------------- */
/* PRESETS */
/* -------------------------------------------------------------------------- */

void ntg_attach_policy_init_flt(
        struct ntg_attach_policy* policy,
        ntg_object* base,
        const struct ntg_attach_policy_flt_opts* opts);

void ntg_attach_policy_init_sflt(
        struct ntg_attach_policy* policy,
        ntg_object* base,
        const struct ntg_attach_policy_sflt_opts* opts);

#endif // NTG_ATTACH_POLICY_H
