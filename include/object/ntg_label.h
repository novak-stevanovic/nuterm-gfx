#ifndef _NTG_LABEL_H_
#define _NTG_LABEL_H_

#include "object/ntg_object.h"
#include "shared/ntg_string.h"
#include "shared/ntg_text.h"

typedef struct ntg_label ntg_label;

#define NTG_LABEL(label_ptr) ((ntg_label*)(label_ptr))

struct ntg_label
{
    ntg_object __base;

    struct ntg_str _text;
    struct nt_gfx _gfx;
    ntg_orientation _orientation;
    ntg_text_alignment _alignment;
    ntg_text_wrap_mode _wrap_mode;
    size_t _indent;
};

void __ntg_label_init__(ntg_label* label, ntg_orientation orientation);
void __ntg_label_deinit__(ntg_label* label);

void ntg_label_set_text(ntg_label* label, struct ntg_str_view view);
void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx);
void ntg_label_set_wrap_mode(ntg_label* label, ntg_text_wrap_mode wrap_mode);
void ntg_label_set_indent(ntg_label* label, size_t indent);

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing);

#endif // _NTG_LABEL_H_
