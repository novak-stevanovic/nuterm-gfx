#include "object/ntg_label.h"
#include <assert.h>

static void __init_default_values(ntg_label* label)
{
    label->_text = NULL;
    label->_gfx = NT_GFX_DEFAULT;
    label->_orientation = NTG_ORIENTATION_HORIZONTAL;
    label->_alignment = NTG_ALIGNMENT_1;
    label->_wrap_mode = NTG_LABEL_WRAP_MODE_NOWRAP;
}

void __ntg_label_init__(ntg_label* label, ntg_orientation orientation)
{
    assert(label != NULL);

    __ntg_object_init__(NTG_OBJECT(label),
            NTG_OBJECT_WIDGET,
            _ntg_label_measure_fn,
            NULL, NULL,
            _ntg_label_arrange_drawing_fn);

    __init_default_values(label);
}

void __ntg_label_deinit__(ntg_label* label)
{
    assert(label != NULL);

    __ntg_object_deinit__(NTG_OBJECT(label));

    if(label->_text != NULL)
        free(label->_text);

    __init_default_values(label);
}

void ntg_label_set_text(ntg_label* label, const char* text)
{
    assert(label != NULL);

    size_t len = strlen(text);

    char* new_text = (char*)realloc(label->_text, len + 1);
    assert(new_text != NULL);

    new_text[len] = '\0';
    label->_text = new_text;
}

void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx)
{
    assert(label != NULL);

    label->_gfx = gfx;
}

void ntg_label_set_wrap_mode(ntg_label* label, ntg_label_wrap_mode wrap_mode)
{
    assert(label != NULL);

    label->_wrap_mode = wrap_mode;
}

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(_label != NULL);

    // TODO
    assert(0);
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);

    // TODO
    assert(0);
}
