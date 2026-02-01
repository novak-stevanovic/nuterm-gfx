#ifndef NTG_PBAR_H
#define NTG_PBAR_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_pbar_opts
{
    ntg_orient orient;
    struct ntg_vcell complete_style;
    struct ntg_vcell uncomplete_style;
    struct ntg_vcell threshold_style;
    double percentage; // [0, 1]
};

struct ntg_pbar_opts ntg_pbar_opts_def();

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_pbar_init(ntg_pbar* pbar);
void ntg_pbar_deinit(ntg_pbar* pbar);

void ntg_pbar_set_opts(ntg_pbar* pbar, struct ntg_pbar_opts opts);

struct ntg_pbar
{
    ntg_object __base;

    struct ntg_pbar_opts _opts;
};

#endif // NTG_PBAR_H
