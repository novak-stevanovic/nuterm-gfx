#include <stdlib.h>
#include "base/ntg_cell.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/ntg_log.h"
#include "object/ntg_color_block.h"

static bool __object_process_key_fn(ntg_object* _color_block,
        struct nt_key_event key_event)
{
    if(key_event.type == NT_KEY_EVENT_UTF32)
    {
        switch(key_event.utf32_data.codepoint)
        {
            case 'w':
                _ntg_object_scroll(_color_block, ntg_dxy(0, -1));
                return true;
            case 'a':
                _ntg_object_scroll(_color_block, ntg_dxy(-1, 0));
                return true;
            case 's':
                _ntg_object_scroll(_color_block, ntg_dxy(0, 1));
                return true;
            case 'd':
                _ntg_object_scroll(_color_block, ntg_dxy(1, 0));
                return true;
            case 'r':
                ntg_color_block_set_color(NTG_COLOR_BLOCK(_color_block),
                        nt_color_new(255, 0, 0));
                return true;
            case 'g':
                ntg_color_block_set_color(NTG_COLOR_BLOCK(_color_block),
                        nt_color_new(0, 255, 0));
                return true;
            case 'b':
                ntg_color_block_set_color(NTG_COLOR_BLOCK(_color_block),
                        nt_color_new(0, 0, 255));
                return true;

        }
    }
    return false;
}

static void inline __update_color(ntg_color_block* block, nt_color color)
{
    if(nt_color_cmp(block->_color, color)) return;

    block->_color = color;

    _ntg_solid_block_set_cell_bp(NTG_SOLID_BLOCK(block), ntg_cell_bg(color));
}

void __ntg_color_block_init__(ntg_color_block* block, nt_color color)
{
    __ntg_solid_block_init__(NTG_SOLID_BLOCK(block), ntg_cell_default());

    __update_color(block, color);

    _ntg_object_set_process_key_fn(NTG_OBJECT(block), __object_process_key_fn);
}

void __ntg_color_block_deinit__(ntg_color_block* block)
{
    __ntg_solid_block_deinit__(NTG_SOLID_BLOCK(block));
}

ntg_color_block* ntg_color_block_new(nt_color color)
{
    ntg_color_block* new = (ntg_color_block*)malloc(
            sizeof(struct ntg_color_block));

    if(new == NULL) return NULL;

    __ntg_color_block_init__(new, color);

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

    __update_color(block, color);
}
