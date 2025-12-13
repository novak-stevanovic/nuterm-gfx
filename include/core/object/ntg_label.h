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
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        void* data);
void _ntg_label_deinit_(ntg_label* label);

void ntg_label_set_text(ntg_label* label, struct ntg_str_view text);
void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx);
void ntg_label_set_primary_alignment(ntg_label* label, ntg_text_alignment alignment);
void ntg_label_set_secondary_alignment(ntg_label* label, ntg_alignment alignment);
void ntg_label_set_wrap_mode(ntg_label* label, ntg_text_wrap_mode wrap_mode);
void ntg_label_set_indent(ntg_label* label, size_t indent);
void ntg_label_set_autotrim(ntg_label* label, bool autotrim);

struct ntg_str_view ntg_label_get_text(const ntg_label* label);
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
        sarena* arena);

void _ntg_label_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena);

#endif // _NTG_LABEL_H_
