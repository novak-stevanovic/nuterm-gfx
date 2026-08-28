#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include "nt.h"
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
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
    bool init_renderer, owns_renderer;
    bool (*on_event_fn)(const struct nt_event* event);
    sarena* arena;

    unsigned int fps;
    ntg_stage* stage;
    ntg_stage* pending_stage;
    ntg_xy app_size;

    struct ntg_task_list task_list;
    pthread_mutex_t lock;
};

static struct ntg_loop loop = {0};

static inline unsigned long long
timespec_to_ms(struct timespec ts)
{
    return ((unsigned long long)ts.tv_sec * 1000) +
           ((unsigned long long)ts.tv_nsec / 1000000);
}

static int update_stage(void);
static void execute_ready_tasks(void);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

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
    if(loop.renderer && loop.owns_renderer)
    {
        if(loop.init_renderer)
            ntg_object_vdeinit(ntg_obj(loop.renderer));

        free(loop.renderer);
    }

    pthread_mutex_destroy(&loop.lock);
    ntg_task_list_deinit(&loop.task_list);

    if(loop.arena)
        sarena_destroy(loop.arena);

    init_default();
}

int ntg_loop_init(const struct ntg_loop_init_opts* opts)
{
    if(loop.state != NTG_LOOP_DEINIT)
        return NTG_ERR_INV_STATE;

    int status;

    /* Set up final opts */

    struct ntg_loop_init_opts opts_final = NTG_LOOP_INIT_OPTS_ZERO;
    if(opts) opts_final = (*opts);
    if(opts_final.arena_size == 0)
        opts_final.arena_size = NTG_LOOP_ARENASZ_AUTO;

    loop.task_list = (struct ntg_task_list) {0};
    if(pthread_mutex_init(&loop.lock, NULL))
        return NTG_ERR_MUTEX_INIT_FAIL;

    /* Arena */

    loop.arena = sarena_create(opts_final.arena_size);
    if(!loop.arena)
    {
        deinit();
        return NTG_ERR_ALLOC_FAIL;
    }

    /* Renderer */

    if(opts_final.renderer) // CUSTOM RENDERER
    {
        loop.renderer = opts_final.renderer;
        loop.owns_renderer = false;
    }
    else
    {
        loop.renderer = malloc(sizeof(ntg_db_renderer));
        if(!loop.renderer)
        {
            deinit();
            return NTG_ERR_ALLOC_FAIL; 
        }

        status = ntg_db_renderer_init((ntg_db_renderer*)loop.renderer, NULL);
        if(status != 0)
        {
            deinit();
            return status;
        }

        loop.init_renderer = true;
        loop.owns_renderer = true;
    }

    loop.on_event_fn = (
            opts_final.on_event_fn ?
            opts_final.on_event_fn :
            ntg_loop_dispatch_event_fn_default);
    loop.state = NTG_LOOP_READY;

    loop.stage = opts_final.stage;
    loop.pending_stage = opts_final.stage;
    return 0;
}

int ntg_loop_deinit(void)
{
    if(ntg_loop_is_running())
        return NTG_ERR_INV_STATE;

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
            nt_key key;
            NT_EVENT_FILL_DATA((*event), &key);
            return ntg_stage_feed_key(stage, key);
        }
        else if(event->type == NT_EVENT_MOUSE)
        {
            nt_mouse mouse;
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
        return NTG_ERR_INV_STATE;

    /* Set up final opts */

    struct ntg_loop_start_opts opts_final = {0};
    if(opts) opts_final = (*opts);
    if(opts_final.fps == 0)
        opts_final.fps = NTG_LOOP_FPS_AUTO;

    loop.fps = opts_final.fps;

    sarena_rewind(loop.arena);

    int _status = 0 ,_tmp_status;

    unsigned int timeout = 1000 / loop.fps;
    struct timespec ts_start, ts_end;
    unsigned long long process_elapsed_ms;
    const ntg_stage_draw* drawing;

    struct nt_event event = {0};
    struct nt_resize resize_event;
    unsigned int event_elapsed;

    nt_get_term_size(&loop.app_size.x, &loop.app_size.y);
    loop.app_size.x = ntg_clamp_size(0, loop.app_size.x, NTG_SIZE_MAX);
    loop.app_size.y = ntg_clamp_size(0, loop.app_size.y, NTG_SIZE_MAX);;

    if(opts_final.mouse_mode == NTG_LOOP_MOUSE_ENABLE)
        nt_mouse_mode_enable();

    bool drain_events = true;

    loop.state = NTG_LOOP_RUNNING;
    if(loop.stage)
    {
        _tmp_status = ntg__stage_set_size(loop.stage, loop.app_size);
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
            loop.app_size.x = ntg_clamp_size(0, resize_event.new_x, NTG_SIZE_MAX);
            loop.app_size.y = ntg_clamp_size(0, resize_event.new_y, NTG_SIZE_MAX);;

            if(loop.stage)
            {
                _tmp_status = ntg__stage_set_size(loop.stage, loop.app_size);
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
            timeout = 1000 / loop.fps;

            execute_ready_tasks();

            if(loop.stage)
            {
                if(loop.stage->ro.dirty)
                {
                    if(!ntg_stage_compose(loop.stage, loop.arena))
                    {
                        ntg__stage_clean(loop.stage);
                    }
                }
                drawing = &(loop.stage->ro.drawing);
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
            timeout = ntg_sub2_uint(timeout, event_elapsed);
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);
        
        process_elapsed_ms = ntg_sub2_ull(timespec_to_ms(ts_end), timespec_to_ms(ts_start));

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
        nt_mouse_mode_disable();

    loop.pending_stage = loop.stage;
    loop.fps = 0;
    loop.app_size = ntg_xy_new(0, 0);

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
/* EXECUTE */
/* ------------------------------------------------------ */

int ntg_loop_schedule(
        void (*task_fn)(void* data),
        void* data,
        unsigned long delay_ms)
{
    if(!task_fn)
        return NTG_ERR_INV_ARG;

    if(delay_ms > NTG_LOOP_DELAY_MAX)
        return NTG_ERR_INV_ARG;

    if(loop.state == NTG_LOOP_DEINIT)
        return NTG_ERR_INV_STATE;

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
        ntg_task_list_popf(&loop.task_list);

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
        return NTG_ERR_INV_STATE;

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
            ntg__stage_leave_loop(old_stage);
            // ntg__stage_set_size(old_stage, ntg_xy(0, 0));
        }
        if(stage)
        {
            ntg__stage_enter_loop(stage);

            int _status = ntg__stage_set_size(stage, loop.app_size);
            if(_status != 0)
                return _status;
        }

        loop.stage = stage;
        loop.pending_stage = stage;
    }

    return 0;
}

ntg_xy ntg_loop_get_app_size(void)
{
    if((loop.state != NTG_LOOP_RUNNING) && (loop.state != NTG_LOOP_STOPPING))
        return ntg_xy_new(0, 0);
    else
        return loop.app_size;
}

unsigned int ntg_loop_get_framerate(void)
{
    if((loop.state != NTG_LOOP_RUNNING) && (loop.state != NTG_LOOP_STOPPING))
        return 0;
    else
        return loop.fps;
}

static int update_stage(void)
{
    ntg_stage* old = loop.stage;
    ntg_stage* new = loop.pending_stage;

    if(old == new) return 0;

    if(old)
    {
        ntg__stage_leave_loop(old);
        int status = ntg__stage_set_size(old, ntg_xy_new(0, 0));
        if(status != 0)
            return status;
    }
    if(new)
    {
        ntg__stage_enter_loop(new);
        int status = ntg__stage_set_size(new, loop.app_size);
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
        ntg_task_list_popf(&loop.task_list);

        pthread_mutex_unlock(&loop.lock);

        if(task.task_fn)
            task.task_fn(task.data);
    }
}
