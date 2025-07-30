#include <assert.h>
#include <stdlib.h>

#include "object/ntg_label.h"
#include "base/ntg_event_types.h"

static void __init_default_values(ntg_label* label);
static void __nsize_fn(ntg_object* __label);
static void __measure_fn(ntg_object* __label);
static void __arrange_fn(ntg_object* __label);

void __ntg_label_init__(ntg_label* label, ntg_orientation orientation)
{
    assert(label != NULL);

    ntg_pane* _label = NTG_PANE(label);
    __ntg_pane_init__(_label, __nsize_fn, __measure_fn, __arrange_fn);

    __init_default_values(label);
    label->_orientation = orientation;
}

void __ntg_label_deinit__(ntg_label* label)
{
    assert(label != NULL);

    ntg_pane* _label = NTG_PANE(label);

    __ntg_pane_deinit__(_label);

    __init_default_values(label);
}

ntg_label* ntg_label_new(ntg_orientation orientation)
{
    ntg_label* new = (ntg_label*)malloc(sizeof(struct ntg_label));

    assert(new != NULL);

    __ntg_label_init__(new, orientation);

    return new;
}

void ntg_label_set_text(ntg_label* label, char* text)
{
    assert(label != NULL);

    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(label));
    ntg_event e1;
    __ntg_event_init__(&e1, NTG_ETYPE_OBJECT_LAYOUT_INVALID, label, NULL);
    ntg_listenable_raise(listenable, &e1);

    label->_text = text;
}

void ntg_label_set_fg_color(ntg_label* label, nt_color color)
{
    assert(label != NULL);
    
    label->_fg_color = color;
}

void ntg_label_set_bg_color(ntg_label* label, nt_color color)
{
    assert(label != NULL);

    label->_bg_color = color;
}

void ntg_label_set_style(ntg_label* label, nt_style style)
{
    assert(label != NULL);

    label->_style = style;
}

void ntg_label_set_padding(ntg_label* label,
        struct ntg_padding_size padding)
{
    assert(label != NULL);

    label->_padding = padding;
}

void ntg_label_set_orientation(ntg_label* label,
        ntg_orientation orientation)
{
    assert(label != NULL);

    label->_orientation = orientation;
}

void ntg_label_set_alignment(ntg_label* label,
        ntg_alignment alignment)
{
    assert(label != NULL);

    label->_alignment = alignment;
}

void ntg_label_set_overflow_mode(ntg_label* label,
        ntg_label_overflow_mode overflow_mode)
{
    assert(label != NULL);

    label->_overflow_mode = overflow_mode;
}

/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_label* label)
{
    label->_orientation = NTG_ORIENTATION_HORIZONTAL;
    label->_alignment = NTG_ALIGNMENT_1;
    label->_padding = NTG_PADDING_SIZE_ZERO;
    label->_text = "";
    label->_fg_color = nt_color_new(0, 0, 0);
    label->_bg_color = nt_color_new(255, 255, 255);
    label->_style = NT_STYLE_DEFAULT;
    label->_overflow_mode = NTG_LABEL_OVERFLOW_MODE_CLIP_NOWRAP;
}

static void __nsize_fn(ntg_object* __label)
{
    // TODO:
    assert(0);
}

static void __measure_fn(ntg_object* __label)
{
    // TODO:
    assert(0);
}

static void __arrange_fn(ntg_object* __label)
{
    // TODO:
    assert(0);
}
