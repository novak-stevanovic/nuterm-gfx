#ifndef _NTG_LABEL_H_
#define _NTG_LABEL_H_

#include "core/object/ntg_object.h"
#include "shared/ntg_string.h"
#include "base/ntg_text.h"

typedef struct ntg_label ntg_label;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void _ntg_label_init_(
        ntg_label* label,
        ntg_orientation orientation,
        struct ntg_object_event_ops event_ops,
        ntg_object_deinit_fn deinit_fn,
        void* data,
        ntg_object_container* container);

void ntg_label_set_text(ntg_label* label, struct ntg_strv text);
void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx);
void ntg_label_set_primary_alignment(ntg_label* label, ntg_text_alignment alignment);
void ntg_label_set_secondary_alignment(ntg_label* label, ntg_alignment alignment);
void ntg_label_set_wrap_mode(ntg_label* label, ntg_text_wrap_mode wrap_mode);
void ntg_label_set_indent(ntg_label* label, size_t indent);
void ntg_label_set_autotrim(ntg_label* label, bool autotrim);

struct ntg_strv ntg_label_get_text(const ntg_label* label);
struct nt_gfx ntg_label_get_gfx(const ntg_label* label);
ntg_text_alignment ntg_label_get_primary_alignment(const ntg_label* label);
ntg_alignment ntg_label_get_secondary_alignment(const ntg_label* label);
ntg_text_wrap_mode ntg_label_get_wrap_mode(const ntg_label* label);
size_t ntg_label_get_indent(const ntg_label* label);
bool ntg_label_get_autotrim(const ntg_label* label);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_label
{
    ntg_object __base;

    ntg_orientation __orientation;
    struct ntg_str __text;
    struct nt_gfx __gfx_base, __gfx_adjusted;
    ntg_text_alignment __primary_alignment;
    ntg_alignment __secondary_alignment;
    ntg_text_wrap_mode __wrap_mode;
    bool __autotrim;
    size_t __indent;
};

void _ntg_label_deinit_fn(ntg_object* object);

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
