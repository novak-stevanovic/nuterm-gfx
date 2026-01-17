#ifndef NTG_LABEL_H
#define NTG_LABEL_H

#include "core/object/ntg_object.h"
#include "shared/ntg_xy.h"
#include "shared/ntg_string.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

enum ntg_label_text_wrap
{
    NTG_LABEL_TEXT_WRAP_NONE,
    NTG_LABEL_TEXT_WRAP_CHAR,
    NTG_LABEL_TEXT_WRAP_WORD
};

enum ntg_label_text_align
{
    NTG_LABEL_TEXT_ALIGN_1,
    NTG_LABEL_TEXT_ALIGN_2,
    NTG_LABEL_TEXT_ALIGN_3,
    NTG_LABEL_TEXT_ALIGN_JUSTIFY
};

struct ntg_label_opts
{
    ntg_orientation orientation;
    struct nt_gfx gfx;
    enum ntg_label_text_align palignment;
    ntg_alignment salignment;
    enum ntg_label_text_wrap wrap;
    bool autotrim;
    size_t indent;
};

struct ntg_label_opts ntg_label_opts_def();

struct ntg_label
{
    ntg_object __base;

    struct ntg_str __text;
    struct ntg_label_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_label* ntg_label_new(ntg_entity_system* system);
void ntg_label_init(ntg_label* label);

void ntg_label_set_opts(ntg_label* label, struct ntg_label_opts opts);

struct ntg_strv ntg_label_get_text(const ntg_label* label);
void ntg_label_set_text(ntg_label* label, struct ntg_strv text);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_label_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_label_measure_fn(
        const ntg_object* _label,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

void _ntg_label_draw_fn(
        const ntg_object* _label,
        void* _layout_data,
        struct ntg_xy size,
        ntg_temp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_LABEL_H
