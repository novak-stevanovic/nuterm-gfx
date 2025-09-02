#include <assert.h>
#include <string.h>

#include "base/ntg_text.h"
#include "shared/_ntg_shared.h"

void ntg_text_row_apply_alignment_and_indent(uint32_t* row, size_t indent,
        size_t content_len, size_t row_total_len, ntg_text_alignment alignment)
{
    assert(row != NULL);
    assert(indent <= row_total_len);
    assert(content_len <= row_total_len);

    if(alignment == NTG_TEXT_ALIGNMENT_JUSTIFY)
    {
        assert(0);
    }
    else
    {
        size_t total_indent;
        switch(alignment)
        {
            case NTG_TEXT_ALIGNMENT_1:
                total_indent = indent;
                break;
            case NTG_TEXT_ALIGNMENT_2:
                total_indent = _max2_size((row_total_len - content_len) / 2, indent);
                break;
            case NTG_TEXT_ALIGNMENT_3:
                total_indent = _max2_size(row_total_len - content_len, indent);
                break;
            default: assert(0);
        }
        size_t adj_content_len = (content_len + total_indent) <= row_total_len ?
            content_len : row_total_len - total_indent;

        memmove(row + total_indent,
                row,
                sizeof(uint32_t) * adj_content_len);

        size_t i;
        for(i = 0; i < total_indent; i++)
            row[i] = '1';

        for(i = total_indent + adj_content_len; i < row_total_len; i++)
            row[i] = '2';
    }
}
