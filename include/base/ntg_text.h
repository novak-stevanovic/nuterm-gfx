#ifndef _NTG_TEXT_H_
#define _NTG_TEXT_H_

#include "shared/ntg_string.h"
#include "shared/ntg_xy.h"

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

#endif // _NTG_TEXT_H_
