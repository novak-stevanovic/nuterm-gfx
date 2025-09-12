#ifndef _NTG_LABEL_H_
#define _NTG_LABEL_H_

#include "object/ntg_object.h"
#include "shared/ntg_string.h"
#include "base/ntg_text.h"

typedef struct ntg_label ntg_label;

#define NTG_LABEL(label_ptr) ((ntg_label*)(label_ptr))

struct ntg_label_opts
{
    ntg_text_alignment primary_alignment;
    ntg_alignment secondary_alignment;
    size_t indent;
    ntg_text_wrap_mode wrap_mode;
};
struct ntg_label_opts ntg_label_opts_default();

struct ntg_label
{
    ntg_object __base;

    struct nt_gfx _gfx;

    struct ntg_str _text;
    ntg_orientation _orientation;
    struct ntg_label_opts _opts;
};

void __ntg_label_init__(ntg_label* label, ntg_orientation orientation);
void __ntg_label_deinit__(ntg_label* label);

void ntg_label_set_text(ntg_label* label, struct ntg_str_view text);
void ntg_label_set_opts(ntg_label* label, struct ntg_label_opts opts);
void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx);

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing);

#endif // _NTG_LABEL_H_
