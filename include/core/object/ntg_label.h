#ifndef _NTG_LABEL_H_
#define _NTG_LABEL_H_

#include "core/object/ntg_object.h"
#include "shared/ntg_string.h"
#include "base/ntg_text.h"

typedef struct ntg_label ntg_label;

#define NTG_LABEL(label_ptr) ((ntg_label*)(label_ptr))

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

void __ntg_label_init__(
        ntg_label* label,
        ntg_orientation orientation,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn);
void __ntg_label_deinit__(ntg_label* label);

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

struct ntg_measure_output __ntg_label_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

void __ntg_label_draw_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size, ntg_drawing* out_drawing);

#endif // _NTG_LABEL_H_
