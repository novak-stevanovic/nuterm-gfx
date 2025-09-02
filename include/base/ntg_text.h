#ifndef _NTG_TEXT_H_
#define _NTG_TEXT_H_

#include <stddef.h>
#include <stdint.h>

typedef enum ntg_text_wrap_mode
{
    NTG_TEXT_WRAP_NOWRAP,
    NTG_TEXT_WRAP_WRAP,
    NTG_TEXT_WRAP_WORD_WRAP,
} ntg_text_wrap_mode;

typedef enum ntg_text_alignment
{
    NTG_TEXT_ALIGNMENT_1,
    NTG_TEXT_ALIGNMENT_2,
    NTG_TEXT_ALIGNMENT_3,
    NTG_TEXT_ALIGNMENT_JUSTIFY
} ntg_text_alignment;

/* content_len - length of text inside row(excluding padding)
 * 
 * If `content_len` + `indent` > `row_total_len`, the text will be clipped.
 *
 * Function assumes that `content_len` <= `row_total_len`,
 * `indent` <= `row_total_len`,
 * `row` is not NULL */
void ntg_text_row_apply_alignment_and_indent(uint32_t* row, size_t indent, size_t content_len,
        size_t row_total_len, ntg_text_alignment alignment);

#endif // _NTG_TEXT_H_
