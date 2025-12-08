#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "shared/ntg_xy.h"

typedef struct ntg_stage ntg_stage;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_stage_drawing ntg_stage_drawing;
typedef struct ntg_loop_ctx ntg_loop_ctx;
struct nt_key_event;

typedef void (*ntg_stage_compose_fn)(
        ntg_stage* stage,
        struct ntg_xy size);

struct ntg_stage
{
    ntg_scene* __scene;
    ntg_stage_compose_fn __compose_fn;
    ntg_stage_drawing* __drawing;
    struct ntg_xy __size;

    void* _data;
};

void __ntg_stage_init__(
        ntg_stage* stage,
        ntg_scene* scene,
        ntg_stage_compose_fn compose_fn,
        void* data);
void __ntg_stage_deinit__(ntg_stage* stage);

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size);

const ntg_stage_drawing* ntg_stage_get_drawing(const ntg_stage* stage);
ntg_scene* ntg_stage_get_scene(ntg_stage* stage);

bool ntg_stage_feed_key_event(
        ntg_stage* stage,
        struct nt_key_event key_event,
        ntg_loop_ctx* loop_ctx);

#endif // _NTG_STAGE_H_
