#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include "nt.h"
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

struct ntg_task
{
    void (*task_fn)(void* data);
    void* data;

    unsigned long long exec_time_ms;
};

GENC_LIST_INLINE(ntg_task_list, struct ntg_task)

struct ntg_loop
{
    enum ntg_loop_state state;

    ntg_renderer* renderer;
    bool _init_renderer, _owns_renderer;
    bool (*on_event_fn)(const struct nt_event* event);
    sarena* arena;

    unsigned int framerate;
    ntg_stage* stage;
    ntg_stage* pending_stage;
    struct ntg_xy app_size;

    struct ntg_task_list task_list;
    pthread_mutex_t lock;
};

static struct ntg_loop loop = {0};

// prerequisite: (tv_sec * 1 000) + (tv_nsec / 1 000 000) <= ULLONG_MAX
// valid, non-negative, normalized ts timespec
static inline unsigned long long
timespec_to_ms(struct timespec ts)
{
    /*
    unsigned long long sec_ull = ts.tv_sec;
    unsigned long long nsec_ull = ts.tv_nsec;

    if((ULLONG_MAX / 1000) < sec_ull)
        sec_ull = ULLONG_MAX / 1000;

    unsigned long long sec_ull_ms = sec_ull * 1000;
    unsigned long long nsec_ull_ms = nsec_ull / 1000000;

    if((ULLONG_MAX - nsec_ull_ms) < sec_ull_ms)
        sec_ull_ms = (ULLONG_MAX - nsec_ull_ms);

    return sec_ull_ms + nsec_ull_ms; */

    return ((unsigned long long)ts.tv_sec * 1000) +
           ((unsigned long long)ts.tv_nsec / 1000000);
}

static int update_stage(void);
static void execute_ready_tasks(void);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_loop_init_opts ntg_loop_init_opts_default(void)
{
    return (struct ntg_loop_init_opts) {
        .arena_size = NTG_LOOP_ARENA_SIZE_AUTO
    };
}

struct ntg_loop_start_opts ntg_loop_start_opts_default(void)
{
    return (struct ntg_loop_start_opts) {
        .mouse_mode = NTG_LOOP_MOUSE_DISABLE,
        .framerate = NTG_LOOP_FRAMERATE_AUTO
    };
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

static void init_default(void)
{
    loop.state = NTG_LOOP_DEINIT;

    loop = (struct ntg_loop) {0};
    // loop.frame_count = 0;
}

static void deinit(void)
{
    if(loop.renderer && loop._owns_renderer)
    {
        if(loop._init_renderer)
            ntg_renderer_vdeinit(loop.renderer);

        free(loop.renderer);
    }

    pthread_mutex_destroy(&loop.lock);
    (void)ntg_task_list_deinit(&loop.task_list);

    if(loop.arena)
        sarena_destroy(loop.arena);

    init_default();
}

int ntg_loop_init(
        ntg_renderer* renderer,
        bool (*on_event_fn)(const struct nt_event* event),
        ntg_stage* init_stage,
        const struct ntg_loop_init_opts* opts)
{
    if(loop.state != NTG_LOOP_DEINIT)
        return NTG_ERR_LOOP_INV_STATE;

    /* Opts */

    struct ntg_loop_init_opts opts_final;
    if(opts)
    {
        if(opts->arena_size == 0)
            return NTG_ERR_INV_ARG;

        opts_final = (*opts);
    }
    else
    {
        opts_final = ntg_loop_init_opts_default();
    }

    loop.task_list = (struct ntg_task_list) {0}; /* Can't fail */
    pthread_mutex_init(&loop.lock, NULL); /* Can't fail */

    /* Arena */

    loop.arena = sarena_create(opts_final.arena_size);
    if(!loop.arena)
    {
        deinit();
        return NTG_ERR_ALLOC_FAIL;
    }

    /* Renderer */

    if(renderer) // CUSTOM RENDERER
    {
        loop.renderer = renderer;
        loop._owns_renderer = false;
    }
    else
    {
        loop.renderer = malloc(sizeof(ntg_default_renderer));
        if(!loop.renderer)
        {
            deinit();
            return NTG_ERR_ALLOC_FAIL; 
        }

        int _status = ntg_default_renderer_init(
                (ntg_default_renderer*)loop.renderer,
                NTG_DEFAULT_RENDERER_TERM_SIZE_AUTO);
        if(_status != 0)
        {
            deinit();
            return _status;
        }

        loop._init_renderer = true;
        loop._owns_renderer = true;
    }

    loop.on_event_fn = (
            on_event_fn ?
            on_event_fn :
            ntg_loop_dispatch_event_fn_default);
    loop.state = NTG_LOOP_READY;

    loop.stage = init_stage;
    loop.pending_stage = init_stage;
    return 0;
}

// What if there are active tasks?
int ntg_loop_deinit(void)
{
    if(ntg_loop_is_running())
        return NTG_ERR_LOOP_INV_STATE;

    deinit();
    return 0;
}

enum ntg_loop_state ntg_loop_get_state(void)
{
    return loop.state;
}

bool ntg_loop_is_running(void)
{
    return ((loop.state == NTG_LOOP_RUNNING) || (loop.state == NTG_LOOP_STOPPING));
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_loop_dispatch_event_fn_default(const struct nt_event* event)
{
    if(!event) return false;

    ntg_stage* stage = loop.stage;
    if(stage)
    {
        if(event->type == NT_EVENT_KEY)
        {
            struct nt_key_event key;
            NT_EVENT_FILL_DATA((*event), &key);
            return ntg_stage_feed_key(stage, key);
        }
        else if(event->type == NT_EVENT_MOUSE)
        {
            struct nt_mouse_event mouse;
            NT_EVENT_FILL_DATA((*event), &mouse);
            return ntg_stage_feed_mouse(stage, mouse);
        }
        else return false;
    }
    else return false;
}

/* ------------------------------------------------------ */
/* START/STOP */
/* ------------------------------------------------------ */

int ntg_loop_start(const struct ntg_loop_start_opts* opts)
{
    if(loop.state != NTG_LOOP_READY)
        return NTG_ERR_LOOP_INV_STATE;

    struct ntg_loop_start_opts opts_final;
    if(opts)
    {
        opts_final = (*opts);
        opts_final.framerate = _min2_uint(opts_final.framerate, NTG_LOOP_FRAMERATE_MAX);
    }
    else
    {
        opts_final = ntg_loop_start_opts_default();
    }

    loop.framerate = opts_final.framerate;

    sarena_rewind(loop.arena);

    int _status = 0 ,_tmp_status;

    unsigned int timeout = 1000 / loop.framerate;
    struct timespec ts_start, ts_end;
    unsigned long long process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event = {0};
    struct nt_resize_event resize_event;
    unsigned int event_elapsed;

    nt_get_term_size(&loop.app_size.x, &loop.app_size.y);
    loop.app_size.x = _clamp_size(0, loop.app_size.x, NTG_SIZE_MAX);
    loop.app_size.y = _clamp_size(0, loop.app_size.y, NTG_SIZE_MAX);;

    if(opts_final.mouse_mode == NTG_LOOP_MOUSE_ENABLE)
        (void)nt_mouse_mode_enable();

    bool drain_events = true;

    loop.state = NTG_LOOP_RUNNING;
    if(loop.stage)
    {
        _tmp_status = _ntg_stage_set_size(loop.stage, loop.app_size);
        if(_tmp_status != 0)
            _status = _tmp_status;
    }

    execute_ready_tasks();
    
    while(true)
    {
        _tmp_status = update_stage();
        if(_tmp_status != 0)
        {
            _status = _tmp_status;
            break;
        }
        if(loop.state == NTG_LOOP_STOPPING)
            break;

        _tmp_status = nt_event_wait(&event, timeout, &event_elapsed);
        if(_tmp_status)
        {
            _status = _tmp_status;
            drain_events = false;
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_RESIZE)
        {
            NT_EVENT_FILL_DATA(event, &resize_event);
            loop.app_size.x = _clamp_size(0, resize_event.new_x, NTG_SIZE_MAX);
            loop.app_size.y = _clamp_size(0, resize_event.new_y, NTG_SIZE_MAX);;

            if(loop.stage)
            {
                _tmp_status = _ntg_stage_set_size(loop.stage, loop.app_size);
                if(_tmp_status != 0)
                {
                    _status = _tmp_status;
                    break;
                }
            }
            // resize_counter++;
        }
        else if(event.type == NT_EVENT_SIGNAL)
        {
            // sigwinch_counter++;
        }

        if(loop.on_event_fn)
            loop.on_event_fn(&event);

        if(event.type == NT_EVENT_TIMEOUT)
        {
            timeout = 1000 / loop.framerate;

            execute_ready_tasks();

            if(loop.stage)
            {
                if(loop.stage->_dirty)
                {
                    if(!ntg_stage_compose(loop.stage, loop.arena))
                    {
                        _ntg_stage_clean(loop.stage);
                    }
                }
                drawing = &(loop.stage->_drawing);
            }
            else drawing = NULL;

            _tmp_status = ntg_renderer_render(loop.renderer, drawing, loop.arena);
            if(_tmp_status)
                ntg_log_log("RENDER ERR");

            sarena_rewind(loop.arena);
            // (loop.frame_count)++;
        }
        else
        {
            timeout = _sub2_uint(timeout, event_elapsed);
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);
        
        process_elapsed_ms = _sub2_ull(timespec_to_ms(ts_end), timespec_to_ms(ts_start));

        timeout = ((timeout > process_elapsed_ms) ? (timeout - process_elapsed_ms) : 0);
    }

    _tmp_status = ntg_renderer_render(loop.renderer, NULL, loop.arena);
    if(_tmp_status) ntg_log_log("RENDER ERR");

    bool make_ready = true;

    if(drain_events)
    {
        while(true)
        {
            _tmp_status = nt_event_wait(&event, 0, NULL);

            if(_tmp_status)
            {
                _status = _tmp_status;
                make_ready = false;
                break;
            }

            if(event.type == NT_EVENT_TIMEOUT)
                break;
        }
    }

    sarena_rewind(loop.arena);

    if(opts_final.mouse_mode == NTG_LOOP_MOUSE_ENABLE)
        (void)nt_mouse_mode_disable();

    loop.pending_stage = loop.stage;
    loop.framerate = 0;
    loop.app_size = ntg_xy(0, 0);

    if(make_ready)
        loop.state = NTG_LOOP_READY;

    return _status;
}

void ntg_loop_stop(void)
{
    if(loop.state != NTG_LOOP_RUNNING)
        return;

    loop.state = NTG_LOOP_STOPPING;
}

/* ------------------------------------------------------ */
/* EXECUTE*/
/* ------------------------------------------------------ */

int ntg_loop_schedule(
        void (*task_fn)(void* data),
        void* data,
        unsigned long delay_ms)
{
    if(!task_fn)
        return NTG_ERR_INV_ARG;

    if(delay_ms > NTG_LOOP_DELAY_MS_MAX)
        return NTG_ERR_INV_ARG;

    if(loop.state == NTG_LOOP_DEINIT)
        return NTG_ERR_LOOP_INV_STATE;

    int _status;

    pthread_mutex_lock(&loop.lock);

    struct timespec _time;
    clock_gettime(CLOCK_MONOTONIC, &_time); 

    unsigned long long now_ms = timespec_to_ms(_time);

    struct ntg_task task = {
        .task_fn = task_fn,
        .data = data,
        .exec_time_ms = now_ms + delay_ms
    };

    struct ntg_task_list_node* it = loop.task_list.head;
    struct ntg_task* it_data;

    while(it)
    {
        it_data = &it->data;

        if(task.exec_time_ms < it_data->exec_time_ms)
        {
            _status = ntg_task_list_ins_before(&loop.task_list, task, it);

            break;
        }

        it = it->next;
    }

    // Must be appended to tail
    if(!it)
        _status = ntg_task_list_pushb(&loop.task_list, task);

    pthread_mutex_unlock(&loop.lock);

    switch(_status)
    {
        case 0:
            return 0;
        case GENC_ERR_ALLOC_FAIL:
            return NTG_ERR_ALLOC_FAIL;
        default:
            return NTG_ERR_UNEXPECTED;
    }
}

void ntg_loop_tasks_clear(void)
{
    if(loop.state == NTG_LOOP_DEINIT)
        return;

    pthread_mutex_lock(&loop.lock);

    while(loop.task_list.size > 0)
        (void)ntg_task_list_popf(&loop.task_list);

    pthread_mutex_unlock(&loop.lock);
}

bool ntg_loop_has_tasks(void)
{
    if(loop.state == NTG_LOOP_DEINIT)
        return false;

    bool running;

    pthread_mutex_lock(&loop.lock);

    running = (loop.task_list.size > 0); 

    pthread_mutex_unlock(&loop.lock);

    return running;
}

/* ------------------------------------------------------ */
/* IN-LOOP ONLY */
/* ------------------------------------------------------ */

ntg_stage* ntg_loop_get_stage(void)
{
    if(!ntg_loop_is_running())
        return NULL;
    else
        return loop.stage;
}

int ntg_loop_set_stage(ntg_stage* stage)
{
    if(loop.state == NTG_LOOP_DEINIT)
        return NTG_ERR_LOOP_INV_STATE;

    if((loop.state == NTG_LOOP_RUNNING) || (loop.state == NTG_LOOP_STOPPING))
    {
        loop.pending_stage = stage;
    }
    else
    {
        ntg_stage* old_stage = loop.stage;

        if(old_stage == stage) return 0;

        if(old_stage)
        {
            _ntg_stage_leave_loop(old_stage);
            // _ntg_stage_set_size(old_stage, ntg_xy(0, 0));
        }
        if(stage)
        {
            _ntg_stage_enter_loop(stage);

            int _status = _ntg_stage_set_size(stage, loop.app_size);
            if(_status != 0)
                return _status;
        }

        loop.stage = stage;
        loop.pending_stage = stage;
    }

    return 0;
}

struct ntg_xy ntg_loop_get_app_size(void)
{
    if((loop.state != NTG_LOOP_RUNNING) && (loop.state != NTG_LOOP_STOPPING))
        return ntg_xy(0, 0);
    else
        return loop.app_size;
}

unsigned int ntg_loop_get_framerate(void)
{
    if((loop.state != NTG_LOOP_RUNNING) && (loop.state != NTG_LOOP_STOPPING))
        return 0;
    else
        return loop.framerate;
}

static int update_stage(void)
{
    ntg_stage* old = loop.stage;
    ntg_stage* new = loop.pending_stage;

    if(old == new) return 0;

    if(old)
    {
        _ntg_stage_leave_loop(old);
        int status = _ntg_stage_set_size(old, ntg_xy(0, 0));
        if(status != 0)
            return status;
    }
    if(new)
    {
        _ntg_stage_enter_loop(new);
        int status = _ntg_stage_set_size(new, loop.app_size);
        if(status != 0)
            return status;
        ntg_stage_mark_dirty(new);
    }

    loop.stage = new;
    loop.pending_stage = new;
    return 0;
}

static void execute_ready_tasks(void)
{
    struct timespec _time;
    clock_gettime(CLOCK_MONOTONIC, &_time);

    unsigned long long now_ms = timespec_to_ms(_time);

    while(true)
    {
        pthread_mutex_lock(&loop.lock);

        struct ntg_task_list_node* head = loop.task_list.head;
        if(!head || (now_ms < head->data.exec_time_ms))
        {
            pthread_mutex_unlock(&loop.lock);
            break;
        }

        struct ntg_task task = head->data;
        (void)ntg_task_list_popf(&loop.task_list);

        pthread_mutex_unlock(&loop.lock);

        if(task.task_fn)
            task.task_fn(task.data);
    }
}
