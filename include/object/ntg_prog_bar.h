#ifndef _NTG_PROG_BAR_H_
#define _NTG_PROG_BAR_H_

#include "object/ntg_pane.h"

typedef struct ntg_prog_bar
{
    ntg_pane __base;

    ntg_orientation _orientation;
    uint _total_job_count, _completed_job_count;
} ntg_prog_bar;

void __ntg_prog_bar_init__(ntg_prog_bar* prog_bar,
        ntg_orientation orientation,
        uint total_job_count);
void __ntg_prog_bar_deinit__(ntg_prog_bar* prog_bar);

ntg_prog_bar* ntg_prog_bar_new(ntg_orientation orientation,
        uint total_job_count);
void ntg_prog_bar_destroy(ntg_prog_bar* prog_bar, uint job_count);

void ntg_prog_bar_set_total_job_count(ntg_prog_bar* prog_bar,
        uint total_job_count);
void ntg_prog_bar_set_completed_job_count(ntg_prog_bar* prog_bar,
        uint completed_job_count);

#endif // _NTG_PROG_BAR_H_
