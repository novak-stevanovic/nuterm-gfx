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
bool ntg_main_panel_opts_are_eq(
        const struct ntg_main_panel_opts* opts1,
        const struct ntg_main_panel_opts* opts2);

enum ntg_main_panel_pos
{
    NTG_MAIN_PANEL_NORTH = 0,
    NTG_MAIN_PANEL_EAST,
    NTG_MAIN_PANEL_SOUTH,
    NTG_MAIN_PANEL_WEST,
    NTG_MAIN_PANEL_CENTER
};

struct ntg_main_panel_hooks
{
    void (*on_child_chng_fn)(
            ntg_main_panel* panel,
            ntg_object* old_child,
            ntg_object* new_child,
            enum ntg_main_panel_pos pos);

    void (*on_opts_chng_fn)(
            ntg_main_panel* panel,
            const struct ntg_main_panel_opts* old_opts,
            const struct ntg_main_panel_opts* new_opts);
};

struct ntg_main_panel
{
    ntg_object __base;

    struct ntg_main_panel_opts _opts;
    ntg_object* _children[5];
    struct ntg_main_panel_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_main_panel_init(ntg_main_panel* panel, const struct ntg_main_panel_opts* opts);
void ntg_main_panel_deinit(ntg_main_panel* panel);
void ntg_main_panel_deinit_(void* _panel);

void ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_object* object,
        enum ntg_main_panel_pos pos);

void ntg_main_panel_set_opts(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts);

#endif // NTG_MAIN_PANEL_H
