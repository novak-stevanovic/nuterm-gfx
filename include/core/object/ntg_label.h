#ifndef _NTG_LABEL_H_
#define _NTG_LABEL_H_

#include "core/object/ntg_object.h"
#include "base/ntg_xy.h"
#include "base/ntg_text.h"
#include "shared/ntg_string.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_label_opts
{
    ntg_orientation orientation;
    struct nt_gfx gfx;
    ntg_text_alignment palignment;
    ntg_alignment salignment;
    ntg_text_wrap_mode wrap;
    bool autotrim;
    size_t indent;
};

struct ntg_label_opts ntg_label_opts_def();

struct ntg_label
{
    ntg_object __base;

    ntg_label_post_draw_fn __post_draw_fn;

    struct ntg_str __text;
    struct ntg_label_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_label* ntg_label_new(ntg_entity_system* system);
void _ntg_label_init_(ntg_label* label, ntg_label_post_draw_fn post_draw_fn);

void ntg_label_set_opts(ntg_label* label, struct ntg_label_opts opts);

struct ntg_strv ntg_label_get_text(const ntg_label* label);
void ntg_label_set_text(ntg_label* label, struct ntg_strv text);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_label_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_label_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

void _ntg_label_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena);

#endif // _NTG_LABEL_H_
