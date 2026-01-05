#ifndef __NTG_LOOP_H__
#define __NTG_LOOP_H__

#include "core/entity/ntg_entity.h"

struct ntg_task_runner
{
    ntg_entity __base;
};

struct ntg_loop
{
    ntg_entity __base;
};

ntg_loop* _ntg_loop_new(ntg_entity_system* system);
void _ntg_loop_init(ntg_loop* loop);
void _ntg_loop_deinit_fn(ntg_entity* entity);

struct ntg_platform
{
    ntg_entity __base;
};

ntg_platform* _ntg_platform_new(ntg_entity_system* system);
void _ntg_platform_init(ntg_platform* platform);
void _ntg_platform_deinit_fn(ntg_entity* entity);

#endif // __NTG_LOOP_H__
