#include "object/ntg_label.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_string.h"
#include <assert.h>

static void __init_default_values(ntg_label* label)
{
    label->_text = (struct ntg_str) {0};
    label->_gfx = NT_GFX_DEFAULT;
    label->_orientation = NTG_ORIENTATION_HORIZONTAL;
    label->_alignment = NTG_TEXT_ALIGNMENT_1;
    label->_wrap_mode = NTG_TEXT_WRAP_NOWRAP;
    label->_indent = 0;
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

    if(label->_text.data != NULL)
        free(label->_text.data);

    __init_default_values(label);
}

void ntg_label_set_text(ntg_label* label, struct ntg_str_view view)
{
    assert(label != NULL);

    char* new_text = (char*)realloc(label->_text.data, view.len + 1);
    assert(new_text != NULL);

    new_text[view.len] = '\0';
    label->_text.data = new_text;
    label->_text.len = view.len;
}

void ntg_label_set_gfx(ntg_label* label, struct nt_gfx gfx)
{
    assert(label != NULL);

    label->_gfx = gfx;
}

void ntg_label_set_wrap_mode(ntg_label* label, ntg_text_wrap_mode wrap_mode)
{
    assert(label != NULL);

    label->_wrap_mode = wrap_mode;
}

void ntg_label_set_indent(ntg_label* label, size_t indent)
{
    assert(label != NULL);

    label->_indent = indent;
}

struct ntg_measure_result _ntg_label_measure_fn(const ntg_object* _label,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(_label != NULL);

    ntg_label* label = NTG_LABEL(_label);
    if(label->_text.data == NULL) return (struct ntg_measure_result) {0};
    size_t len = label->_text.len;
    if(len == 0) return (struct ntg_measure_result) {0};
    struct ntg_str_view _text = {
        .data = label->_text.data,
        .len = label->_text.len
    };

    struct ntg_measure_result result = {0};
    if(orientation == label->_orientation)
    {
        switch(label->_wrap_mode)
        {
            case NTG_TEXT_WRAP_NOWRAP:
                // result = __measure_nowrap();
                break;
            case NTG_TEXT_WRAP_WRAP:
                // result = __measure_wrap();
                break;
            case NTG_TEXT_WRAP_WORD_WRAP:
                // result = __measure_word_wrap();
                break;
            case NTG_TEXT_WRAP_SCROLL:
                // result = __measure_scroll();
                break;
        }
    }
    else
    {
    }

    return result;
}

void _ntg_label_arrange_drawing_fn(const ntg_object* _label,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_label != NULL);

    // TODO
    assert(0);
}
