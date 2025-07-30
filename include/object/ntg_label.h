#ifndef _NTG_LABEL_H_
#define _NTG_LABEL_H_

#include "object/ntg_padding.h"
#include "object/ntg_pane.h"

typedef enum ntg_label_overflow_mode
{
    NTG_LABEL_OVERFLOW_MODE_CLIP_NOWRAP,
    NTG_LABEL_OVERFLOW_MODE_CLIP_WRAP,
    NTG_LABEL_OVERFLOW_MODE_CLIP_WORDWRAP,
    NTG_LABEL_OVERFLOW_MODE_SCROLL
} ntg_label_overflow_mode;

typedef struct ntg_label
{
    ntg_pane __base;

    char* _text;
    nt_color _fg_color, _bg_color;
    nt_style _style;

    struct ntg_padding_size _padding;
    ntg_orientation _orientation;
    ntg_alignment _alignment;

    ntg_label_overflow_mode _overflow_mode;

} ntg_label;

void __ntg_label_init__(ntg_label* label, ntg_orientation orientation);
void __ntg_label_deinit__(ntg_label* label);

ntg_label* ntg_label_new(ntg_orientation orientation);

void ntg_label_set_text(ntg_label* label, char* text);
void ntg_label_set_fg_color(ntg_label* label, nt_color color);
void ntg_label_set_bg_color(ntg_label* label, nt_color color);
void ntg_label_set_style(ntg_label* label, nt_style style);

void ntg_label_set_padding(ntg_label* label,
        struct ntg_padding_size padding);
void ntg_label_set_orientation(ntg_label* label,
        ntg_orientation orientation);
void ntg_label_set_alignment(ntg_label* label,
        ntg_alignment alignment);

void ntg_label_set_overflow_mode(ntg_label* label,
        ntg_label_overflow_mode overflow_mode);

#endif // _NTG_LABEL_H_
