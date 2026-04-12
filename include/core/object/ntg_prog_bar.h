#ifndef NTG_PROG_BAR_H
#define NTG_PROG_BAR_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_prog_bar_style
{
    struct ntg_vcell complete_style;
    struct ntg_vcell uncomplete_style;
    struct ntg_vcell threshold_style;
};

struct ntg_prog_bar_style ntg_prog_bar_style_def();

struct ntg_prog_bar_opts
{
    struct ntg_prog_bar_style style;
    ntg_orient orient;
};

struct ntg_prog_bar_opts ntg_prog_bar_opts_def();

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_prog_bar_init(ntg_prog_bar* prog_bar);
void ntg_prog_bar_deinit(ntg_prog_bar* prog_bar);
void ntg_prog_bar_deinit_(void* _prog_bar);

void ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts);
void ntg_prog_bar_set_percentage(ntg_prog_bar* prog_bar, double percentage);

struct ntg_prog_bar
{
    ntg_object __base;

    double _percentage; // [0, 1]
    struct ntg_prog_bar_opts _opts;
};

#endif // NTG_PROG_BAR_H
