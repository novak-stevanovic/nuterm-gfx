#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "object/ntg_prog_bar.h"
#include "base/ntg_event_types.h"
#include "core/ntg_scene.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_log.h"

#define PROG_BAR_DEFAULT_PRIMARY_AXIS 10
#define PROG_BAR_DEFAULT_SECONDARY_AXIS 1

static bool __process_key_event_fn(ntg_object* __prog_bar,
        struct nt_key_event key_event)
{
    ntg_prog_bar* prog_bar = NTG_PROG_BAR(__prog_bar);
    ntg_listenable* listenable = _ntg_object_get_listenable(__prog_bar);

    if(key_event.type == NT_KEY_EVENT_UTF32)
    {
        switch(key_event.utf32_data.codepoint)
        {
            case 'a':
                ntg_prog_bar_increase_completed_job_count(prog_bar, 2);
                return true;
        }
    }

    return false;
}

static inline double __calculate_percentage(uint completed_jobs, uint total_jobs)
{
    return ((1.0 * completed_jobs) / (1.0 * total_jobs));
}

static void __natural_size_fn(ntg_object* __prog_bar)
{
    ntg_prog_bar* prog_bar = NTG_PROG_BAR(__prog_bar);
    ntg_orientation orientation = prog_bar->_orientation;
    
    struct ntg_xy pref_size = ntg_object_get_pref_content_size(__prog_bar);
    struct ntg_oxy _pref_size = ntg_oxy_from_xy(pref_size, orientation);

    size_t nsize_prim_val = _pref_size.prim_val != NTG_PREF_SIZE_UNSET ?
        _pref_size.prim_val :
        PROG_BAR_DEFAULT_PRIMARY_AXIS;
    size_t nsize_sec_val = _pref_size.sec_val != NTG_PREF_SIZE_UNSET ?
        _pref_size.sec_val :
        PROG_BAR_DEFAULT_SECONDARY_AXIS;

    struct ntg_oxy _nsize = ntg_oxy(nsize_prim_val, nsize_sec_val);
    struct ntg_xy nsize = ntg_xy_from_oxy(_nsize, orientation);

    _ntg_object_set_natural_content_size(__prog_bar, nsize);
}

static void __measure_fn(ntg_object* __prog_bar)
{
    ntg_prog_bar* prog_bar = NTG_PROG_BAR(__prog_bar);
    ntg_orientation orientation = prog_bar->_orientation;

    struct ntg_constr constr = ntg_object_get_content_constr(__prog_bar);

    struct ntg_oxy _min_size = ntg_oxy_from_xy(constr.min_size, orientation);
    // struct ntg_oxy _max_size = ntg_oxy_from_xy(constr.max_size, orientation);

    struct ntg_oxy _size;

    if(_min_size.prim_val < PROG_BAR_DEFAULT_PRIMARY_AXIS)
        _size.prim_val = PROG_BAR_DEFAULT_PRIMARY_AXIS;
    else
        _size.prim_val = _min_size.prim_val;

    if(_min_size.sec_val < PROG_BAR_DEFAULT_SECONDARY_AXIS)
        _size.sec_val = PROG_BAR_DEFAULT_SECONDARY_AXIS;
    else
        _size.sec_val = _min_size.sec_val;

    _ntg_object_set_content_size(__prog_bar, ntg_xy_from_oxy(_size, orientation));
}

static void __arrange_fn(ntg_object* __prog_bar)
{
    ntg_prog_bar* prog_bar = NTG_PROG_BAR(__prog_bar);
    ntg_orientation orientation = prog_bar->_orientation;
    ntg_object_drawing* drawing = __prog_bar->_virtual_drawing;

    struct ntg_xy size = ntg_object_get_content_size(__prog_bar);
    struct ntg_oxy _size = ntg_oxy_from_xy(size, orientation);

    double percentage = __calculate_percentage(
            prog_bar->_completed_jobs,
            prog_bar->_total_jobs);

    size_t completed_len = round(_size.prim_val * percentage);

    size_t p, s;
    struct ntg_xy it_xy;
    struct ntg_oxy _it_xy;
    ntg_cell* it_cell;
    for(p = 0; p < _size.prim_val; p++)
    {
        for(s = 0; s < _size.sec_val; s++)
        {
            _it_xy = ntg_oxy(p, s);
            it_xy = ntg_xy_from_oxy(_it_xy, orientation);
            it_cell = ntg_object_drawing_at_(drawing, it_xy);

            if(completed_len == _size.prim_val)
                (*it_cell) = prog_bar->_completed_style;
            else if(completed_len > 0)
            {
                if(p < (completed_len - 1))
                    (*it_cell) = prog_bar->_completed_style;
                else if(p == (completed_len - 1))
                    (*it_cell) = prog_bar->_threshold_style;
                else // p > (completed_len - 1)
                    (*it_cell) = prog_bar->_uncompleted_style;
            }
            else
                (*it_cell) = prog_bar->_uncompleted_style;
        }
    }
}

void __ntg_prog_bar_init__(ntg_prog_bar* prog_bar,
        ntg_orientation orientation, uint total_jobs,
        ntg_cell completed_style,
        ntg_cell uncompleted_style,
        ntg_cell threshold_style)
{
    assert(prog_bar != NULL);

    __ntg_pane_init__(NTG_PANE(prog_bar), __natural_size_fn,
            __measure_fn, __arrange_fn);

    prog_bar->_orientation = orientation;
    prog_bar->_total_jobs = total_jobs;
    prog_bar->_completed_jobs = 0;
    prog_bar->_completed_style = completed_style;
    prog_bar->_uncompleted_style = uncompleted_style;
    prog_bar->_threshold_style = threshold_style;

    _ntg_object_set_process_key_fn(NTG_OBJECT(prog_bar), __process_key_event_fn);
}

void __ntg_prog_bar_deinit__(ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    __ntg_pane_deinit__(NTG_PANE(prog_bar));

    prog_bar->_orientation = NTG_ORIENTATION_HORIZONTAL;
    prog_bar->_total_jobs = 0;
    prog_bar->_completed_jobs = 0;
}

ntg_prog_bar* ntg_prog_bar_new(ntg_orientation orientation,
        uint total_job_count,
        ntg_cell completed_style,
        ntg_cell uncompleted_style,
        ntg_cell threshold_style)
{
    ntg_prog_bar* new = (ntg_prog_bar*)malloc(sizeof(struct ntg_prog_bar));
    assert(new != NULL);

    __ntg_prog_bar_init__(new, orientation, total_job_count,
            completed_style, uncompleted_style, threshold_style);

    return new;
}

void ntg_prog_bar_destroy(ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    __ntg_prog_bar_deinit__(prog_bar);

    free(prog_bar);
}

void ntg_prog_bar_set_total_job_count(ntg_prog_bar* prog_bar,
        uint total_jobs)
{
    assert(prog_bar != NULL);

    if(prog_bar->_total_jobs == total_jobs) return;

    uint old_total = prog_bar->_total_jobs;
    prog_bar->_total_jobs = total_jobs;

    ntg_event e1, e2;
    struct ntg_uint_change data1 = {
        .old = old_total,
        .new = prog_bar->_total_jobs
    };

    ntg_object* __prog_bar = NTG_OBJECT(prog_bar);
    ntg_listenable* listenable = _ntg_object_get_listenable(__prog_bar);
    __ntg_event_init__(&e1, NTG_ETYPE_PROG_BAR_TOTAL_JOB_CHANGE, __prog_bar, &data1);
    ntg_listenable_raise(listenable, &e1);
    __ntg_event_init__(&e2, NTG_ETYPE_OBJECT_CONTENT_INVALID, __prog_bar, NULL);
    ntg_listenable_raise(listenable, &e2);

    uint new_completed = _min2_uint(prog_bar->_completed_jobs, prog_bar->_total_jobs);
    ntg_prog_bar_set_completed_job_count(prog_bar, new_completed);

}

void ntg_prog_bar_set_completed_job_count(ntg_prog_bar* prog_bar,
        uint completed_jobs)
{
    assert(prog_bar != NULL);

    uint old = prog_bar->_completed_jobs;
    uint new = (completed_jobs <= prog_bar->_total_jobs) ?
        completed_jobs : prog_bar->_total_jobs;

    if(old == new) return;

    prog_bar->_completed_jobs = new;

    ntg_event e1, e2;
    struct ntg_uint_change data = {
        .old = old,
        .new = new
    };
    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(prog_bar));
    __ntg_event_init__(&e1, NTG_ETYPE_PROG_BAR_CURRENT_JOB_CHANGE, prog_bar, &data);
    ntg_listenable_raise(listenable, &e1);
    __ntg_event_init__(&e2, NTG_ETYPE_OBJECT_CONTENT_INVALID, prog_bar, NULL);
    ntg_listenable_raise(listenable, &e2);

}

void ntg_prog_bar_increase_completed_job_count(ntg_prog_bar* prog_bar,
        uint completed_job_increase)
{
    assert(prog_bar != NULL);

    ntg_prog_bar_set_completed_job_count(prog_bar, prog_bar->_completed_jobs +
            completed_job_increase);
}

void ntg_prog_bar_set_completed_style(ntg_prog_bar* prog_bar,
        ntg_cell completed_style)
{
    assert(prog_bar != NULL);

    if(ntg_cell_are_equal(completed_style, prog_bar->_completed_style)) return;

    prog_bar->_completed_style = completed_style;

    ntg_event e1, e2;
    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(prog_bar));
    __ntg_event_init__(&e2, NTG_ETYPE_OBJECT_CONTENT_INVALID, prog_bar, NULL);
    ntg_listenable_raise(listenable, &e2);
}

void ntg_prog_bar_set_uncompleted_style(ntg_prog_bar* prog_bar,
        ntg_cell uncompleted_style)
{
    assert(prog_bar != NULL);

    if(ntg_cell_are_equal(uncompleted_style, prog_bar->_uncompleted_style)) return;

    prog_bar->_uncompleted_style = uncompleted_style;

    ntg_event e1, e2;
    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(prog_bar));
    __ntg_event_init__(&e2, NTG_ETYPE_OBJECT_CONTENT_INVALID, prog_bar, NULL);
    ntg_listenable_raise(listenable, &e2);
}

void ntg_prog_bar_set_threshold_style(ntg_prog_bar* prog_bar,
        ntg_cell threshold_style)
{
    assert(prog_bar != NULL);

    if(ntg_cell_are_equal(threshold_style, prog_bar->_threshold_style)) return;

    prog_bar->_threshold_style = threshold_style;

    ntg_event e1, e2;
    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(prog_bar));
    __ntg_event_init__(&e2, NTG_ETYPE_OBJECT_CONTENT_INVALID, prog_bar, NULL);
    ntg_listenable_raise(listenable, &e2);
}

void ntg_prog_bar_set_orientation(ntg_prog_bar* prog_bar,
        ntg_orientation orientation)
{
    assert(prog_bar != NULL);

    if(prog_bar->_orientation == orientation) return;

    prog_bar->_orientation = orientation;

    ntg_event e1, e2;
    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(prog_bar));
    __ntg_event_init__(&e2, NTG_ETYPE_OBJECT_LAYOUT_INVALID, prog_bar, NULL);
    ntg_listenable_raise(listenable, &e2);
}
