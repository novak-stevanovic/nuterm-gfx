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

GENC_LIST_GENERATE(ntg_task_list, struct ntg_task, NULL);

struct ntg_loop
{
    enum ntg_loop_state state;

    struct
    {
        ntg_renderer* renderer;
        bool _init_renderer, _owns_renderer;
        bool (*on_event_fn)(const struct nt_event* event);
        sarena* arena;
    } init;

    struct
    {
        unsigned int framerate;
        ntg_stage* stage;
        ntg_stage* pending_stage;
        struct ntg_xy app_size;
    } running;

    struct
    {
        struct ntg_task_list task_list;
        pthread_mutex_t lock;
    };
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

static void update_stage();
static void execute_ready_tasks();

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_loop_init_opts ntg_loop_init_opts_default()
{
    return (struct ntg_loop_init_opts) {
        .arena_size = NTG_LOOP_ARENA_SIZE_AUTO
    };
}

struct ntg_loop_start_opts ntg_loop_start_opts_default()
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

static void init_default()
{
    loop.state = NTG_LOOP_DEINIT;

    loop = (struct ntg_loop) {0};
    // loop.frame_count = 0;
}

static void deinit()
{
    if(loop.init.renderer && loop.init._owns_renderer)
    {
        if(loop.init._init_renderer)
            ntg_renderer_vdeinit(loop.init.renderer);

        free(loop.init.renderer);
    }

    pthread_mutex_destroy(&loop.lock);
    ntg_task_list_deinit(&loop.task_list, NULL);

    if(loop.init.arena)
        sarena_destroy(loop.init.arena);

    init_default();
}

void ntg_loop_init(
        ntg_renderer* renderer,
        bool (*on_event_fn)(const struct nt_event* event),
        ntg_stage* init_stage,
        const struct ntg_loop_init_opts* opts,
        int* out_status)
{
    ntg_set_out(out_status, 0);

    if(loop.state != NTG_LOOP_DEINIT)
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

    /* Opts */

    struct ntg_loop_init_opts opts_final;
    if(opts)
    {
        if(opts->arena_size == 0)
            ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

        opts_final = (*opts);
    }
    else
    {
        opts_final = ntg_loop_init_opts_default();
    }

    ntg_task_list_init(&loop.task_list, NULL); /* Can't fail */
    pthread_mutex_init(&loop.lock, NULL); /* Can't fail */

    /* Arena */

    loop.init.arena = sarena_create(opts_final.arena_size);
    if(!loop.init.arena)
    {
        deinit();
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    /* Renderer */

    if(renderer) // CUSTOM RENDERER
    {
        loop.init.renderer = renderer;
        loop.init._owns_renderer = false;
    }
    else
    {
        loop.init.renderer = malloc(sizeof(ntg_default_renderer));
        if(!loop.init.renderer)
        {
            deinit();
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL); 
        }

        int _status;
        ntg_default_renderer_init(
                (ntg_default_renderer*)loop.init.renderer,
                NTG_DEFAULT_RENDERER_TERM_SIZE_AUTO,
                &_status);  
        switch(_status)
        {
            case 0: break;
            case NTG_ERR_ALLOC_FAIL:
                deinit();
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL); 
            default:
                deinit();
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED); 
        }

        loop.init._init_renderer = true;
        loop.init._owns_renderer = true;
    }

    loop.init.on_event_fn = (
            on_event_fn ?
            on_event_fn :
            ntg_loop_dispatch_event_fn_default);
    loop.state = NTG_LOOP_READY;

    loop.running.stage = init_stage;
    loop.running.pending_stage = init_stage;
}

// What if there are active tasks?
void ntg_loop_deinit(int* out_status)
{
    ntg_set_out(out_status, 0);

    if(ntg_loop_is_running())
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

    deinit();
}

ntg_loop_state ntg_loop_get_state()
{
    return loop.state;
}

bool ntg_loop_is_running()
{
    return ((loop.state == NTG_LOOP_RUNNING) || (loop.state == NTG_LOOP_STOPPING));
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_loop_dispatch_event_fn_default(const struct nt_event* event)
{
    if(!event) return false;

    ntg_stage* stage = loop.running.stage;
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

void ntg_loop_start(const struct ntg_loop_start_opts* opts, int* out_status)
{
    ntg_set_out(out_status, 0);

    if(loop.state != NTG_LOOP_READY)
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

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

    loop.running.framerate = opts_final.framerate;

    sarena_rewind(loop.init.arena);

    int _status = 0 ,_tmp_status;

    unsigned int timeout = 1000 / loop.running.framerate;
    struct timespec ts_start, ts_end;
    unsigned long long process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event = {0};
    struct nt_resize_event resize_event;
    unsigned int event_elapsed;

    nt_get_term_size(&loop.running.app_size.x, &loop.running.app_size.y);
    loop.running.app_size.x = _clamp_size(0, loop.running.app_size.x, NTG_SIZE_MAX);
    loop.running.app_size.y = _clamp_size(0, loop.running.app_size.y, NTG_SIZE_MAX);;

    if(opts_final.mouse_mode == NTG_LOOP_MOUSE_ENABLE)
        nt_mouse_mode_enable(NULL);

    bool drain_events = true;

    loop.state = NTG_LOOP_RUNNING;
    if(loop.running.stage)
    {
        /* Can't fail unless size overflows */
        _ntg_stage_set_size(loop.running.stage, loop.running.app_size, NULL);
    }

    execute_ready_tasks();
    
    while(true)
    {
        update_stage();
        if(loop.state == NTG_LOOP_STOPPING)
            break;

        event_elapsed = nt_event_wait(&event, timeout, &_tmp_status);
        if(_tmp_status)
        {
            _status = NTG_ERR_UNEXPECTED;
            drain_events = false;
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_RESIZE)
        {
            NT_EVENT_FILL_DATA(event, &resize_event);
            loop.running.app_size.x = _clamp_size(0, resize_event.new_x, NTG_SIZE_MAX);
            loop.running.app_size.y = _clamp_size(0, resize_event.new_y, NTG_SIZE_MAX);;

            if(loop.running.stage)
            {
                /* Can't fail unless size overflows */
                _ntg_stage_set_size(loop.running.stage, loop.running.app_size, NULL);
            }
            // resize_counter++;
        }
        else if(event.type == NT_EVENT_SIGNAL)
        {
            // sigwinch_counter++;
        }

        if(loop.init.on_event_fn)
            loop.init.on_event_fn(&event);

        if(event.type == NT_EVENT_TIMEOUT)
        {
            timeout = 1000 / loop.running.framerate;

            execute_ready_tasks();

            if(loop.running.stage)
            {
                if(loop.running.stage->_dirty)
                {
                    if(!ntg_stage_compose(loop.running.stage, loop.init.arena))
                    {
                        _ntg_stage_clean(loop.running.stage);
                    }
                }
                drawing = &(loop.running.stage->_drawing);
            }
            else drawing = NULL;

            ntg_renderer_render(loop.init.renderer, drawing, loop.init.arena, &_tmp_status);
            if(_tmp_status)
                ntg_log_log("RENDER ERR");

            sarena_rewind(loop.init.arena);
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

    ntg_renderer_render(loop.init.renderer, NULL, loop.init.arena, &_tmp_status);
    if(_tmp_status) ntg_log_log("RENDER ERR");

    bool make_ready = true;

    if(drain_events)
    {
        while(true)
        {
            nt_event_wait(&event, 0, &_tmp_status);

            if(_tmp_status)
            {
                _status = NTG_ERR_UNEXPECTED;
                make_ready = false;
                break;
            }

            if(event.type == NT_EVENT_TIMEOUT)
                break;
        }
    }

    sarena_rewind(loop.init.arena);

    if(opts_final.mouse_mode == NTG_LOOP_MOUSE_ENABLE)
        nt_mouse_mode_disable(NULL);

    loop.running.stage = NULL;
    loop.running.pending_stage = NULL;
    loop.running.framerate = 0;
    loop.running.app_size = ntg_xy(0, 0);

    if(make_ready)
        loop.state = NTG_LOOP_READY;

    ntg_set_out(out_status, _status);
}

void ntg_loop_stop()
{
    if(loop.state != NTG_LOOP_RUNNING)
        return;

    loop.state = NTG_LOOP_STOPPING;
}

/* ------------------------------------------------------ */
/* EXECUTE*/
/* ------------------------------------------------------ */

void ntg_loop_schedule(
        void (*task_fn)(void* data),
        void* data,
        unsigned long delay_ms,
        int* out_status)
{
    ntg_set_out(out_status, 0);

    if(!task_fn)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(delay_ms > NTG_LOOP_DELAY_MS_MAX)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(loop.state == NTG_LOOP_DEINIT)
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

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
        it_data = it->data;

        if(task.exec_time_ms < it_data->exec_time_ms)
        {
            ntg_task_list_ins_before_node(&loop.task_list, task, it, &_status);

            switch(_status)
            {
                case 0: break;
                case GENC_ERR_ALLOC_FAIL:
                        ntg_set_out(out_status, NTG_ERR_ALLOC_FAIL);
                        break;
                default:
                        ntg_set_out(out_status, NTG_ERR_ALLOC_FAIL);
                        break;
            }
            break;
        }

        it = it->next;
    }

    // Must be appended to tail
    if(!it)
        ntg_task_list_pushb(&loop.task_list, task, &_status);

    pthread_mutex_unlock(&loop.lock);

    switch(_status)
    {
        case 0: break;
        case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }
}

void ntg_loop_tasks_clear()
{
    if(loop.state == NTG_LOOP_DEINIT)
        return;

    pthread_mutex_lock(&loop.lock);

    while(loop.task_list.size > 0)
        ntg_task_list_popf(&loop.task_list, NULL);

    pthread_mutex_unlock(&loop.lock);
}

bool ntg_loop_has_tasks()
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

ntg_stage* ntg_loop_get_stage()
{
    if(!ntg_loop_is_running())
        return NULL;
    else
        return loop.running.stage;
}

void ntg_loop_set_stage(ntg_stage* stage, int* out_status)
{
    ntg_init_status(out_status);
    
    if(loop.state == NTG_LOOP_DEINIT)
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

    if((loop.state == NTG_LOOP_RUNNING) || (loop.state == NTG_LOOP_STOPPING))
    {
        loop.running.pending_stage = stage;
    }
    else
    {
        ntg_stage* old_stage = loop.running.stage;

        if(old_stage == stage) return;

        if(old_stage)
        {
            _ntg_stage_leave_loop(old_stage);
            // _ntg_stage_set_size(old_stage, ntg_xy(0, 0));
        }
        if(stage)
        {
            _ntg_stage_enter_loop(stage);

            /* Can only fail if app_size overflows */
            _ntg_stage_set_size(stage, loop.running.app_size, NULL);
        }

        loop.running.stage = stage;
        loop.running.pending_stage = stage;
    }
}

struct ntg_xy ntg_loop_get_app_size()
{
    if((loop.state != NTG_LOOP_RUNNING) && (loop.state != NTG_LOOP_STOPPING))
        return ntg_xy(0, 0);
    else
        return loop.running.app_size;
}

unsigned int ntg_loop_get_framerate()
{
    if((loop.state != NTG_LOOP_RUNNING) && (loop.state != NTG_LOOP_STOPPING))
        return 0;
    else
        return loop.running.framerate;
}

static void update_stage()
{
    ntg_stage* old = loop.running.stage;
    ntg_stage* new = loop.running.pending_stage;

    if(old == new) return;

    // if(new && new->_loop) return;

    int _status;

    if(old)
    {
        _ntg_stage_leave_loop(old);
        /* Can't fail unless app_size exceeds NTG_SIZE_MAX */
        _ntg_stage_set_size(old, ntg_xy(0, 0), NULL);
    }
    if(new)
    {
        _ntg_stage_enter_loop(new);
        /* Can't fail unless app_size exceeds NTG_SIZE_MAX */
        _ntg_stage_set_size(new, loop.running.app_size, &_status);
        ntg_stage_mark_dirty(new);
    }

    loop.running.stage = new;
    loop.running.pending_stage = new;
}

static void execute_ready_tasks()
{
    struct timespec _time;
    clock_gettime(CLOCK_MONOTONIC, &_time);

    unsigned long long now_ms = timespec_to_ms(_time);

    pthread_mutex_lock(&loop.lock);

    struct ntg_task_list_node* head;
    struct ntg_task* data;

    while(loop.task_list.size > 0)
    {
        head = loop.task_list.head;
        data = head->data;

        if(now_ms < data->exec_time_ms)
            break;

        if(data->task_fn)
            data->task_fn(data->data);

        ntg_task_list_popf(&loop.task_list, NULL);
    }

    pthread_mutex_unlock(&loop.lock);
}
