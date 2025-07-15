#ifndef _NTG_PROG_BAR_H_
#define _NTG_PROG_BAR_H_

#include "object/ntg_pane.h"

#define NTG_PROG_BAR(prog_bar_ptr) ((ntg_prog_bar*)(prog_bar_ptr))

typedef struct ntg_prog_bar
{
    ntg_pane __base;

    ntg_orientation _orientation;
    uint _total_jobs, _completed_jobs;

    ntg_cell _completed_style, _uncompleted_style,
             _threshold_style;
} ntg_prog_bar;

void __ntg_prog_bar_init__(ntg_prog_bar* prog_bar,
        ntg_orientation orientation, uint total_jobs,
        ntg_cell completed_style,
        ntg_cell uncompleted_style,
        ntg_cell threshold_style);

void __ntg_prog_bar_deinit__(ntg_prog_bar* prog_bar);

ntg_prog_bar* ntg_prog_bar_new(ntg_orientation orientation,
        uint total_job_count,
        ntg_cell completed_style,
        ntg_cell uncompleted_style,
        ntg_cell threshold_style);
void ntg_prog_bar_destroy(ntg_prog_bar* prog_bar);

void ntg_prog_bar_set_total_job_count(ntg_prog_bar* prog_bar,
        uint total_jobs);
void ntg_prog_bar_set_completed_job_count(ntg_prog_bar* prog_bar,
        uint completed_jobs);
void ntg_prog_bar_increase_completed_job_count(ntg_prog_bar* prog_bar,
        uint completed_job_increase);

#endif // _NTG_PROG_BAR_H_
