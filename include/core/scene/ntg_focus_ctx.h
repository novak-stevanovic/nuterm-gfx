#ifndef NTG_FOCUS_CTX_H
#define NTG_FOCUS_CTX_H

#include "shared/ntg_typedef.h"

enum ntg_focus_ctx_window_mode
{
    NTG_FOCUS_CTX_WINDOW_MODAL,
    NTG_FOCUS_CTX_WINDOW_MODELESS
};

enum ntg_focus_ctx_push_mode
{
    NTG_FOCUS_CTX_PUSH_ALLOW,
    NTG_FOCUS_CTX_PUSH_FORBID
};

struct ntg_focus_ctx
{
    ntg_focus_mgr* mgr;
    ntg_widget* root;
    ntg_focus_ctx_window_mode window_mode;
    ntg_focus_ctx_push_mode push_mode;
};

GENC_SIMPLE_LIST_GENERATE(ntg_focus_ctx_list, struct ntg_focus_ctx);

#endif //  NTG_FOCUS_CTX_H
