#include <stdlib.h>
#include "base/ntg_cell.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/ntg_log.h"
#include "object/ntg_color_block.h"

bool __process_key_fn(ntg_object* _cb, struct nt_key_event key_event)
{
    if(key_event.type != NT_KEY_EVENT_UTF32) return false;

    ntg_color_block* cb = (ntg_color_block*)_cb;

    switch(key_event.utf32_data.codepoint)
    {
        case 'b':
            cb->__color = nt_color_new(0, 0, 255);
            return true;
        case 'g':
            cb->__color = nt_color_new(0, 255, 0);
            return true;
        case 'r':
            cb->__color = nt_color_new(255, 0, 0);
            return true;
        case 'w':
            _ntg_object_scroll_enable(_cb);
            _ntg_object_scroll(_cb, ntg_dxy(0, -1));
            return true;
        case 'a':
            _ntg_object_scroll_enable(_cb);
            _ntg_object_scroll(_cb, ntg_dxy(-1, 0));
            return true;
        case 's':
            _ntg_object_scroll_enable(_cb);
            _ntg_object_scroll(_cb, ntg_dxy(0, 1));
            return true;
        case 'd':
            _ntg_object_scroll_enable(_cb);
            _ntg_object_scroll(_cb, ntg_dxy(1, 0));
            return true;
        default:
            return false;
    }
}

static void __measure_fn(ntg_object* _block)
{
    if(_block == NULL) return;

    struct ntg_xy size = _block->_constr.min_size;

    ntg_xy_size_(&size);

    _ntg_object_set_size(_block, size);
}

static void __arrange_fn(ntg_object* _block)
{
    if(_block == NULL) return;
    ntg_color_block* block = (ntg_color_block*)_block;

    ntg_object_drawing* drawing = _block->_drawing;
    struct ntg_xy size = ntg_object_drawing_get_size(drawing);
    ntg_xy_size_(&size);
    size_t i, j;

    struct ntg_cell* it_cell;

    nt_color it_color;
    int it_letter;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_letter = ((i + j) % 26) + 'a';

            it_cell = ntg_object_drawing_at_(drawing, ntg_xy(j, i));

            *it_cell = ntg_cell_full(it_letter, nt_color_new(0, 0, 0),
                    block->__color, NT_STYLE_DEFAULT);
        }
    }
}

static void __nsize_fn(ntg_object* _block)
{
    struct ntg_xy size = _block->_pref_size;
    ntg_xy_size_(&size);
    _ntg_object_set_nsize(_block, size);
}

void __ntg_color_block_init__(ntg_color_block* block)
{
    __ntg_pane_init__(NTG_PANE(block), __nsize_fn, __measure_fn, __arrange_fn);

    _ntg_object_set_process_key_fn(NTG_OBJECT(block), __process_key_fn);

    block->__color = NT_COLOR_DEFAULT;
}

void __ntg_color_block_deinit__(ntg_color_block* block)
{
    __ntg_pane_deinit__((ntg_pane*)block);

    block->__color = NT_COLOR_DEFAULT;
}

ntg_color_block* ntg_color_block_new(nt_color color)
{
    ntg_color_block* new = (ntg_color_block*)malloc(
            sizeof(struct ntg_color_block));

    if(new == NULL) return NULL;

    __ntg_color_block_init__(new);
    new->__color = color;

    return new;
}

void ntg_color_block_destroy(ntg_color_block* block)
{
    __ntg_color_block_deinit__(block);

    if(block != NULL)
        free(block);
}

void ntg_color_block_set_color(ntg_color_block* block, nt_color color)
{
    if(block == NULL) return;

    block->__color = color;
}

nt_color ntg_color_block_get_color(ntg_color_block* block)
{
    return (block != NULL) ? block->__color : NT_COLOR_DEFAULT;
}
