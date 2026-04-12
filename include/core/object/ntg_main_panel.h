#ifndef NTG_MAIN_PANEL_H
#define NTG_MAIN_PANEL_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_main_panel_opts
{
    size_t placeholder;
};

struct ntg_main_panel_opts ntg_main_panel_opts_def();

enum ntg_main_panel_pos
{
    NTG_MAIN_PANEL_NORTH = 0,
    NTG_MAIN_PANEL_EAST,
    NTG_MAIN_PANEL_SOUTH,
    NTG_MAIN_PANEL_WEST,
    NTG_MAIN_PANEL_CENTER
};

struct ntg_main_panel
{
    ntg_object __base;

    ntg_object* _children[5];
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_main_panel_init(ntg_main_panel* box);
void ntg_main_panel_deinit(ntg_main_panel* box);
void ntg_main_panel_deinit_(void* _box);

void ntg_main_panel_set(
        ntg_main_panel* box,
        ntg_object* object,
        enum ntg_main_panel_pos pos);

#endif // NTG_MAIN_PANEL_H
