#ifndef NTG_LABEL_H
#define NTG_LABEL_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

enum ntg_label_wrap
{
    NTG_LABEL_WRAP_NONE,
    NTG_LABEL_WRAP_CHAR,
    NTG_LABEL_WRAP_WORD
};

enum ntg_label_text_mode
{
    NTG_LABEL_TEXT_ALIGN,
    NTG_LABEL_TEXT_JUSTIFY
};

enum ntg_label_bg_mode
{
    NTG_LABEL_BG_FULL,
    NTG_LABEL_BG_FLT
};

struct ntg_label_opts
{
    ntg_orient orient;
    struct nt_gfx gfx;
    ntg_label_text_mode text_mode;
    ntg_align prim_align; // Active only if NTG_LABEL_TEXT_ALIGN
    ntg_align sec_align;
    ntg_label_bg_mode bg_mode;
    ntg_label_wrap wrap;
    bool autotrim;
    size_t indent;
};

struct ntg_label_opts ntg_label_opts_def();

struct ntg_label
{
    ntg_object __base;

    struct ntg_label_opts _opts;

    struct
    {
        char* data;
        size_t len;
    } _text;

    struct ntg_label_priv* __priv;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_label_init(ntg_label* label);
void ntg_label_deinit(ntg_label* label);
void ntg_label_deinit_(void* _label);

void ntg_label_set_opts(ntg_label* label, const struct ntg_label_opts* opts);
void ntg_label_set_text_safe(ntg_label* label, const char* text, size_t len);
void ntg_label_set_text(ntg_label* label, const char* text);

#endif // NTG_LABEL_H
