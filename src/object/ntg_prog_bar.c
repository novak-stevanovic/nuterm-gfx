#include <stdlib.h>
#include <assert.h>

#include "object/ntg_prog_bar.h"

// TODO: finish

#define PROG_BAR_DEFAULT_PRIMARY_AXIS 10
#define PROG_BAR_DEFAULT_SECONDARY_AXIS 1

static void __natural_size_fn(ntg_object* __prog_bar)
{
}

static void __measure_fn(ntg_object* __prog_bar)
{
}

static void __arrange_fn(ntg_object* __prog_bar)
{
}

void __ntg_prog_bar_init__(ntg_prog_bar* prog_bar,
        ntg_orientation orientation,
        uint total_job_count)
{
    assert(prog_bar != NULL);

    __ntg_pane_init__(NTG_PANE(prog_bar), __natural_size_fn,
            __measure_fn, __arrange_fn);

    prog_bar->_orientation = orientation;
    prog_bar->_total_job_count = total_job_count;
    prog_bar->_completed_job_count = 0;
}

void __ntg_prog_bar_deinit__(ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    __ntg_pane_deinit__(NTG_PANE(prog_bar));

    prog_bar->_orientation = NTG_ORIENTATION_HORIZONTAL;
    prog_bar->_total_job_count = 0;
    prog_bar->_completed_job_count = 0;
}

ntg_prog_bar* ntg_prog_bar_new(ntg_orientation orientation,
        uint total_job_count)
{
    ntg_prog_bar* new = (ntg_prog_bar*)malloc(sizeof(struct ntg_prog_bar));
    assert(new != NULL);

    __ntg_prog_bar_init__(new, orientation, total_job_count);

    return new;
}

void ntg_prog_bar_destroy(ntg_prog_bar* prog_bar, uint job_count)
{
    assert(prog_bar != NULL);

    __ntg_prog_bar_deinit__(prog_bar);

    free(prog_bar);
}

void ntg_prog_bar_set_total_job_count(ntg_prog_bar* prog_bar,
        uint total_job_count)
{
    assert(prog_bar != NULL);

    prog_bar->_total_job_count = total_job_count;
}

void ntg_prog_bar_set_completed_job_count(ntg_prog_bar* prog_bar,
        uint completed_job_count)
{
    assert(prog_bar != NULL);

    prog_bar->_completed_job_count = completed_job_count;
}
