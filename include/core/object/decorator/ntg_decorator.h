#ifndef NTG_DECORATOR_H
#define NTG_DECORATOR_H

#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_decorator_width
{
    size_t north, east, south, west;
};

struct ntg_decorator_width 
ntg_decorator_width(size_t north, size_t east,
                    size_t south, size_t west);

enum ntg_decorator_enable_mode
{
    NTG_DECORATOR_ENABLE_ON_MIN,
    NTG_DECORATOR_ENABLE_ON_NAT,
    NTG_DECORATOR_ENABLE_ALWAYS
};

struct ntg_decorator_opts
{
    ntg_decorator_enable_mode mode;
    struct ntg_decorator_width width;
};

struct ntg_decorator_opts ntg_decorator_opts_def();

struct ntg_decorator
{
    ntg_object __base;

    ntg_widget* _widget;
    struct ntg_decorator_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_decorator_set_opts(ntg_decorator* decorator,
                            struct ntg_decorator_opts opts);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_decorator_init(ntg_decorator* decorator, ntg_object_draw_fn draw_fn);
void ntg_decorator_deinit(ntg_decorator* decorator);

// Called internally by ntg_widget. Updates only the decorator's state
void _ntg_decorator_decorate(ntg_decorator* decorator, ntg_widget* widget);

void* _ntg_decorator_ldata_init_fn(const ntg_object* _decorator);
void _ntg_decorator_ldata_deinit_fn(void* data, const ntg_object* _decorator);

struct ntg_object_measure _ntg_decorator_measure_fn(
        const ntg_object* _decorator,
        void* _ldata,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void _ntg_decorator_constrain_fn(
        const ntg_object* _decorator,
        void* _ldata,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena);

void _ntg_decorator_arrange_fn(
        const ntg_object* _decorator,
        void* _ldata,
        ntg_object_xy_map* out_pos_map,
        sarena* arena);

#endif // NTG_DECORATOR_H
